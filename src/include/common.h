#ifndef _COMMON_
#define _COMMON_

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

typedef struct Player
{
	int y;
	int x;
} Player;

typedef struct MAP_LIST
{
	struct MAP_LIST *prev;
	struct MAP_LIST *next;
	char **map;
} map_list;

typedef struct Coordinate
{
	struct Coordinate *prev;
	struct Coordinate *next;
	int y;
	int x;
} coord;

typedef struct PassingPoints
{
	coord *first;
	coord *latest;
	void (*pop)(struct PassingPoints *);
};

Player *player;
static int GAME_MODE;
static int num_of_rocks;
static map_list *map_list_head;
static map_list *map_list_tail;

void pop(struct PassingPoints *passingPoints)
{
}

#endif
