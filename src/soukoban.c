#include <curses.h>
#include <stdio.h>

static int y = 10, x = 10;

void print_player(void)
{
	mvaddch(y, x, '@');
	move(y, x);
}

int main(int argc, char **argv[])
{
	char c, c2;

	initscr();
	noecho();
	cbreak();

	endwin();

	return 0;
}
