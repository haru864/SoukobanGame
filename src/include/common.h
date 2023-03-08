#ifndef _COMMON_
#define _COMMON_

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <string.h>

#define DEFAULT_DAT_FILENAME "map/soukoban.map"
#define BUF_SIZE 1024
// #define MAXIMUM_UNDO_STEPS 3
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
	KB_AUTOMATION = 'm',
};

typedef struct GamePlayer
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
	coord *head;
	coord *tail;
	char **map;
	int rock;
	coord location;
	void (*push)(struct PassingPoints *);
	void (*pop)(struct PassingPoints *);
} passing_points;

// declared in soukoban.c
extern Player *player;
extern int GAME_MODE;
extern int num_of_rocks;
extern map_list *map_list_head;
extern map_list *map_list_tail;

// soukoban functions
void soukoban_init(char *);
void soukoban_main_loop(char *);
void soukoban_end(void);
void load_map(char *);
void step(int, int);
void dash(int, int);
void undo_save_progress(void);
void undo_do(void);
void replay(void);
void debug(void);

// autoresolve functions
void autoresolve();

#endif
