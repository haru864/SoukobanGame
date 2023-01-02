#include "include/common.h"

int main(int argc, char **argv)
{
	char *MAP_FILE = DEFAULT_DAT_FILENAME;
	if (argc >= 2)
	{
		MAP_FILE = argv[1];
	}

	soukoban_init(MAP_FILE);
	soukoban_main_loop(MAP_FILE);
	soukoban_end();

	return 0;
}

void soukoban_init(char *filename)
{
	initscr();
	noecho();
	cbreak();
	curs_set(0);
	player = (Player *)malloc(sizeof(Player));
	GAME_MODE = MANUAL;
	load_map(filename);
	move(player->y, player->x);
	map_list_head = map_list_tail = NULL;
	undo_save_progress();
}

void load_map(char *filename)
{
	char buf[BUF_SIZE];

	FILE *fp = fopen(filename, "r");
	if (!fp)
	{
		perror("fopen");
		soukoban_end();
		exit(EXIT_FAILURE);
	}

	if (fgets(buf, sizeof(buf), fp) == NULL && ferror(fp) != 0)
	{
		perror("fgets");
		soukoban_end();
	}

	if (sscanf(buf, "y = %d, x = %d, rocks = %d", &(player->y), &(player->x), &num_of_rocks) == EOF)
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

	mvprintw(LINES - 1, 0, "%d rock(s) left", num_of_rocks);
	fclose(fp);
}

void soukoban_end(void)
{
	clear();
	if (num_of_rocks == 0)
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

void step(int dy, int dx)
{
	// save current map
	undo_save_progress();

	chtype c;
	int new_y = player->y + dy, new_x = player->x + dx;
	move(new_y, new_x);
	c = inch() & A_CHARTEXT;

	if (c == WALL || c == PIT)
	{
		return;
	}
	else if (c == ROCK)
	{
		move(player->y + dy + dy, player->x + dx + dx);
		char dist = inch() & A_CHARTEXT;
		if (dist == WALL || dist == ROCK)
		{
			return;
		}
		else if (dist == PIT)
		{
			mvaddch(player->y + dy + dy, player->x + dx + dx, SPACE);
			mvaddch(player->y + dy, player->x + dx, SPACE);
			num_of_rocks--;
			mvprintw(LINES - 1, 0, "%d rock(s) left", num_of_rocks);
		}
		else if (dist == SPACE)
		{
			mvaddch(player->y + dy + dy, player->x + dx + dx, ROCK);
		}
	}

	move(player->y, player->x);
	addch(SPACE);
	player->y += dy;
	player->x += dx;
	mvaddch(player->y, player->x, PLAYER);
}

void dash(int dy, int dx)
{
	while (1)
	{
		int prev_y = player->y, prev_x = player->x;
		step(dy, dx);
		if (player->y == prev_y && player->x == prev_x)
		{
			break;
		}
	}
}

void soukoban_main_loop(char *filename)
{
	int c;
	refresh();

	while (num_of_rocks != 0 && (c = getch()) != KB_QUIT)
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
			autoresolve();
			break;
		default:
			break;
		}

		move(player->y, player->x);
		// debug();
		refresh();
	}
}

void undo_save_progress(void)
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

	if (sprintf(currentMapList->map[0], "y=%d, x=%d, rocks=%d", player->y, player->x, num_of_rocks) < 0)
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

void undo_do(void)
{
	if (!map_list_head || !map_list_tail)
	{
		return;
	}

	if (sscanf(map_list_tail->map[0], "y = %d, x = %d, rocks = %d", &(player->y), &(player->x), &num_of_rocks) == EOF)
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

void replay(void)
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

void debug()
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
