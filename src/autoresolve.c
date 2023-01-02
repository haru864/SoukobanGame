#include "include/common.h"

passing_points *passingPoints;
char **currentMap;

void autoresolve()
{
	init_autoresolve();

	clear();
	mvprintw(10, 5, "autoresolve!");
	refresh();

	getch();
	printScreen(currentMap);
	getch();

	soukoban_end();
	exit(EXIT_FAILURE);
}

void init_autoresolve()
{
	currentMap = (char **)malloc(sizeof(char *) * LINES);
	if (currentMap == NULL)
	{
		perror("malloc");
		soukoban_end();
	}

	for (int i = 0; i < LINES; i++)
	{
		currentMap[i] = (char *)malloc(sizeof(char) * (COLS + 1));
		if (currentMap[i] == NULL)
		{
			perror("malloc");
			soukoban_end();
		}
	}

	passingPoints = (passing_points *)malloc(sizeof(passing_points));
	if (passingPoints == NULL)
	{
		perror("malloc");
		soukoban_end();
	}

	for (int i = 0; i < LINES - 1; i++)
	{
		char buf[COLS + 1];
		mvinstr(i, 0, buf);
		strncpy(currentMap[i], buf, COLS);
		currentMap[i][COLS] = '\0';
	}
}

void printScreen(char **screen)
{
	clear();
	for (int i = 0; i < LINES - 1; i++)
	{
		mvprintw(i, 0, screen[i]);
	}
	refresh();
}
