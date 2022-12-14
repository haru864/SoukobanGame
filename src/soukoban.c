#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>

#define DEFAULT_DAT_FILENAME "soukoban.map"
#define BUF_SIZE 1024
#define MAXIMUM_UNDO_STEPS 3
#define SAFE_FREE(ptr) \
	{                  \
		free(ptr);     \
		ptr = NULL;    \
	}

enum
{
	MANUAL,
	AUTOMATIC,
};

enum
{
	WALL = '#',
	ROCK = '\'',
	PIT = '^',
	PLAYER = '@',
	SPACE = ' ',
};

enum
{
	KB_UP = 'w',
	KB_DOWN = 's',
	KB_LEFT = 'a',
	KB_RIGHT = 'd',
	KB_UP2 = 'W',
	KB_DOWN2 = 'S',
	KB_LEFT2 = 'A',
	KB_RIGHT2 = 'D',
	KB_QUIT = 'q',
	KB_REFRESH = 'r',
	KB_UNDO = 'u',
	KB_AUTOMATION = 'M',
};

typedef struct MAP_LIST
{
	struct MAP_LIST *prev;
	struct MAP_LIST *next;
	char **map;

} map_list;

static int GAME_MODE;
static int y, x;
static int number_of_rocks_left;
static map_list *map_list_head;
static map_list *map_list_tail;

static void soukoban_init(char *);
static void soukoban_main_loop(char *);
static void soukoban_end(void);
static void load_map(char *);
static void step(int, int);
static void dash(int, int);
static void undo_save_progress(void);
static void undo_do(void);
static void replay(void);
// --------in progress---------
static void debug(void);
// ----------------------------

int main(int argc, char **argv)
{
	char *filename = DEFAULT_DAT_FILENAME;
	if (argc >= 2)
	{
		filename = argv[1];
	}

	soukoban_init(filename);
	soukoban_main_loop(filename);
	soukoban_end();

	return 0;
}

static void soukoban_init(char *filename)
{
	initscr();
	noecho();
	cbreak();
	GAME_MODE = MANUAL;
	load_map(filename);
	move(y, x);
	map_list_head = map_list_tail = NULL;
	undo_save_progress;
}

static void load_map(char *filename)
{
	char buf[BUF_SIZE];
	FILE *fp = fopen(filename, "r");

	if (fgets(buf, sizeof(buf), fp) == NULL && ferror(fp) != 0)
	{
		perror("fgets");
		soukoban_end();
	}

	if (sscanf(buf, "y = %d, x = %d, rocks = %d", &y, &x, &number_of_rocks_left) == EOF)
	{
		perror("sscanf");
		soukoban_end();
	}

	for (int i = 0; i < LINES - 1; i++)
	{
		if (fgets(buf, sizeof(buf), fp) == NULL)
		{
			break;
		}
		move(i, 0);
		printw("%s", buf);
	}

	mvprintw(LINES - 1, 0, "%d rock(s) left", number_of_rocks_left);
	fclose(fp);
}

static void soukoban_end(void)
{
	clear();
	if (number_of_rocks_left == 0)
	{
		mvprintw(1, 1, "Finish!");
		mvprintw(2, 1, "Type R-key to replay,");
		mvprintw(3, 1, "others to end: ");
		int key = getch();
		if (key == 'r' || key == 'R')
		{
			replay();
		}
	}
	clear();
	refresh();
	endwin();
}

static void step(int dy, int dx)
{
	// save current map
	undo_save_progress();

	chtype c;
	int new_y = y + dy, new_x = x + dx;
	move(new_y, new_x);
	c = inch() & A_CHARTEXT;

	if (c == WALL || c == PIT)
	{
		return;
	}
	else if (c == ROCK)
	{
		move(y + dy + dy, x + dx + dx);
		char dist = inch() & A_CHARTEXT;
		if (dist == WALL || dist == ROCK)
		{
			return;
		}
		else if (dist == PIT)
		{
			mvaddch(y + dy + dy, x + dx + dx, SPACE);
			mvaddch(y + dy, x + dx, SPACE);
			number_of_rocks_left--;
			mvprintw(LINES - 1, 0, "%d rock(s) left", number_of_rocks_left);
		}
		else if (dist == SPACE)
		{
			mvaddch(y + dy + dy, x + dx + dx, ROCK);
		}
	}

	move(y, x);
	addch(SPACE);
	y += dy;
	x += dx;
	mvaddch(y, x, PLAYER);
}

static void dash(int dy, int dx)
{
	while (1)
	{
		int prev_y = y, prev_x = x;
		step(dy, dx);
		if (y == prev_y && x == prev_x)
		{
			break;
		}
	}
}

static void soukoban_main_loop(char *filename)
{
	int c;
	refresh();

	while (number_of_rocks_left != 0 && (c = getch()) != KB_QUIT)
	{
		switch (c)
		{
		case KB_UP:
			step(-1, 0);
			break;
		case KB_DOWN:
			step(1, 0);
			break;
		case KB_LEFT:
			step(0, -1);
			break;
		case KB_RIGHT:
			step(0, 1);
			break;
		case KB_UP2:
			dash(-1, 0);
			break;
		case KB_DOWN2:
			dash(1, 0);
			break;
		case KB_LEFT2:
			dash(0, -1);
			break;
		case KB_RIGHT2:
			dash(0, 1);
			break;
		case KB_REFRESH:
			soukoban_init(filename);
			break;
		case KB_UNDO:
			undo_do();
			break;
		case KB_AUTOMATION:
			GAME_MODE = AUTOMATIC;
			break;
		default:
			break;
		}

		move(y, x);
		// debug();
		refresh();
	}
}

static void undo_save_progress(void)
{
	// copy current map
	map_list *currentMapList = (map_list *)malloc(sizeof(map_list));

	if (currentMapList == NULL)
	{
		fprintf(stderr, "Out of memory");
		soukoban_end();
	}

	currentMapList->prev = NULL;
	currentMapList->next = NULL;
	currentMapList->map = (char **)malloc(sizeof(char *) * LINES);

	if (currentMapList->map == NULL)
	{
		fprintf(stderr, "Out of memory");
		soukoban_end();
	}

	for (int i = 0; i < LINES; i++)
	{
		currentMapList->map[i] = (char *)malloc(sizeof(char) * COLS);

		if (currentMapList->map[i] == NULL)
		{
			fprintf(stderr, "Out of memory");
			soukoban_end();
		}
	}

	if (sprintf(currentMapList->map[0], "y=%d, x=%d, rocks=%d", y, x, number_of_rocks_left) < 0)
	{
		perror("sprintf");
		soukoban_end();
	}

	for (int i = 0; i < LINES - 1; i++)
	{
		char buf[COLS];

		mvinstr(i, 0, buf);

		if (sprintf(currentMapList->map[i + 1], buf) < 0)
		{
			perror("sprintf");
			soukoban_end();
		}
	}

	// add current map to list
	if (map_list_head == NULL || map_list_tail == NULL)
	{
		map_list_head = map_list_tail = currentMapList;
	}
	else
	{
		map_list_tail->next = currentMapList;
		currentMapList->prev = map_list_tail;
		map_list_tail = currentMapList;
	}
}

static void undo_do(void)
{
	if (!map_list_head || !map_list_tail)
	{
		return;
	}

	if (sscanf(map_list_tail->map[0], "y = %d, x = %d, rocks = %d", &y, &x, &number_of_rocks_left) == EOF)
	{
		perror("sscanf");
		soukoban_end();
	}

	for (int i = 0; i < LINES - 1; i++)
	{
		move(i, 0);
		printw("%s", map_list_tail->map[i + 1]);
	}

	map_list *formerTail = map_list_tail;
	map_list_tail = formerTail->prev;
	SAFE_FREE(formerTail);
}

static void replay(void)
{
	clear();
	map_list *currentMapList = map_list_head;

	while (currentMapList != NULL)
	{
		for (int i = 1; i < LINES; i++)
		{
			mvprintw(i - 1, 0, currentMapList->map[i]);
		}

		currentMapList = currentMapList->next;
		refresh();
		usleep(5 * 100 * 1000);
	}
}

static void debug()
{
	if (!map_list_tail)
	{
		return;
	}

	for (int i = 0; i < 10; i++)
	{
		mvprintw(20 + i, 0, map_list_tail->map[i]);
	}

	mvprintw(32, 0, "%p", map_list_head);
	mvprintw(33, 0, "%p", map_list_tail);
}
