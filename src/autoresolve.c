#include "include/common.h"

passing_points *passingPoints;
// char **currentMap;

void init_autoresolve();
void printScreen(char **);
void push(passing_points *);
void pop(passing_points *);

void autoresolve()
{
	init_autoresolve();

	clear();
	mvprintw(10, 5, "autoresolve!");
	refresh();

	getch();
	// printScreen(currentMap);
	printScreen(passingPoints->map);
	getch();

	soukoban_end();
	exit(EXIT_FAILURE);
}

void init_autoresolve()
{
	// currentMap = (char **)malloc(sizeof(char *) * LINES);
	// if (currentMap == NULL)
	// {
	// 	perror("malloc");
	// 	soukoban_end();
	// }

	// for (int i = 0; i < LINES; i++)
	// {
	// 	currentMap[i] = (char *)malloc(sizeof(char) * (COLS + 1));
	// 	if (currentMap[i] == NULL)
	// 	{
	// 		perror("malloc");
	// 		soukoban_end();
	// 	}
	// }

	passingPoints = (passing_points *)malloc(sizeof(passing_points));
	if (passingPoints == NULL)
	{
		perror("malloc");
		soukoban_end();
		exit(EXIT_FAILURE);
	}

	passingPoints->map = (char **)malloc(sizeof(char *) * LINES);
	if (passingPoints->map == NULL)
	{
		perror("malloc");
		soukoban_end();
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < LINES; i++)
	{
		passingPoints->map[i] = (char *)malloc(sizeof(char) * (COLS + 1));

		char buf[COLS + 1];
		mvinstr(i, 0, buf);
		strncpy(passingPoints->map[i], buf, COLS);
		passingPoints->map[i][COLS] = '\0';
	}

	passingPoints->head = passingPoints->tail = NULL;
	passingPoints->push = push;
	passingPoints->pop = pop;

	// for (int i = 0; i < LINES - 1; i++)
	// {
	// 	char buf[COLS + 1];
	// 	mvinstr(i, 0, buf);
	// 	strncpy(currentMap[i], buf, COLS);
	// 	currentMap[i][COLS] = '\0';
	// }
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

void push(passing_points *passingPoints)
{
	if (passingPoints->head == NULL || passingPoints->tail == NULL)
	{
		return;
	}
}

void pop(passing_points *passingPoints)
{
	if (passingPoints->head == NULL || passingPoints->tail == NULL)
	{
		return;
	}

	if (passingPoints->head == passingPoints->tail)
	{
		SAFE_FREE(passingPoints->head);
		passingPoints->head = passingPoints->tail = NULL;
		return;
	}

	coord *nextTail = passingPoints->tail->prev;
	SAFE_FREE(passingPoints->tail);
	passingPoints->tail = nextTail;
}
