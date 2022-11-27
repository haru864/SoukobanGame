#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>

#define DEFAULT_DAT_FILENAME "soukoban.map"
#define BUF_SIZE 1024

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
	KB_QUIT = 'q',
};

static int y, x;
static int num;

static void load_map(char *filename)
{
	char buf[BUF_SIZE];
	FILE *fp = fopen(filename, "r");

	if (fgets(buf, sizeof(buf), fp) == NULL && ferror(fp) != 0)
	{
		perror("fgets");
		exit(1);
	}
	if (sscanf(buf, "y = %d, x = %d, num = %d", &y, &x, &num) == EOF)
	{
		perror("sscanf");
		exit(1);
	}

	for (int i = 0; i < LINES; i++)
	{
		if (fgets(buf, sizeof(buf), fp) == NULL)
		{
			break;
		}
		move(i, 0);
		printw("%s", buf);
	}

	fclose(fp);
}

static void soukoban_init(char *filename)
{
	initscr();
	noecho();
	cbreak();
	load_map(filename);
	move(y, x);
}

static void soukoban_end(void)
{
	clear();
	refresh();
	endwin();
}

static void step(int dy, int dx)
{
	chtype c;
	int new_y = y + dy, new_x = x + dx;
	move(new_y, new_x);
	c = inch() & A_CHARTEXT;
	if (c == WALL || c == ROCK || c == PIT)
	{
		return;
	}
	move(y, x);
	addch(SPACE);
	y += dy;
	x += dx;
	mvaddch(y, x, PLAYER);
}

static void soukoban_main_loop(void)
{
	int c;
	refresh();

	while (num != 0 && (c = getch()) != KB_QUIT)
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
		default:
			break;
		}

		move(y, x);
		refresh();
	}
}

int main(int argc, char **argv)
{
	char *filename = DEFAULT_DAT_FILENAME;
	if (argc >= 2)
	{
		filename = argv[1];
	}

	soukoban_init(filename);
	soukoban_main_loop();
	soukoban_end();

	return 0;
}
