#include <curses.h>
#include <stdio.h>

static int y = 10, x = 10;

void print_player(void)
{
	mvaddch(y, x, '@');
	move(y, x);
}

int main(int argc, char **argv)
{
	char c, c2;

	initscr();
	noecho();
	cbreak();
	mvaddch(10, 20, '#');
	print_player();

	while ((c = getch()) != 'q')
	{
		int new_y = y, new_x = x;
		switch (c)
		{
		case 'w':
			new_y--;
			break;
		case 's':
			new_y++;
			break;
		case 'a':
			new_x--;
			break;
		case 'd':
			new_x++;
			break;
		default:
			continue;
		}

		c2 = mvinch(new_y, new_x);
		move(y, x);
		if (c2 == '#')
			continue;

		mvaddch(y, x, ' ');
		y = new_y;
		x = new_x;

		print_player();
		refresh();
	}

	endwin();

	return 0;
}
