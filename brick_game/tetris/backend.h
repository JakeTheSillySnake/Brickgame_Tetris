#ifndef BACKEND_H
#define BACKEND_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*field logic*/
#define ROWS 20
#define COLUMNS 10
#define POINTS 100
#define SPAWN_X 3
#define SPAWN_Y 2
#define TIMEOUT 300
#define FILE_COUNT 8
#define SCORE_PATH "assets/high_score.txt"
#define FILES_PATH "assets/"

#define SUCCESS 0
#define FAILURE 1

/*matrix cell states*/
#define EMPTY 0
#define STATIC 1
#define MOVING 2

/*game pieces*/
#define SQUARE 1
#define LINE 2
#define LENGTH_MAX 4

typedef enum {
  Start = 0,
  Pause,
  Terminate,
  Left,
  Right,
  Down,
  Action,
  Nosig
} UserAction_t;

typedef struct {
  int field[ROWS + LENGTH_MAX][COLUMNS];
  int next[LENGTH_MAX][LENGTH_MAX];
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  int id;
  int length;
  int x;
  int y;
} GameInfo_t;

typedef enum {
  START = 0,
  SPAWN,
  MOVE,
  SHIFT,
  ATTACH,
  GAMEOVER,
  EXIT,
  PAUSE,
  FILE_ERROR
} State_t;

GameInfo_t updateCurrentState(GameInfo_t game, char input, State_t *state);

/*start*/
void init_stats(GameInfo_t *game, State_t *state);

/*user actions*/
void shift(GameInfo_t *game);
void collide(GameInfo_t *game);
void move_left(GameInfo_t *game);
void move_right(GameInfo_t *game);
void rotate_piece(GameInfo_t *game);
void reverse_rows(int **matrix, int size);

/*end of round*/
int generate_piece(GameInfo_t *game);
void spawn_piece(GameInfo_t *game);
void count_points(GameInfo_t *game);
void erase_line(GameInfo_t *game, int index);

/*action checks*/
int check_boundaryY(GameInfo_t *game);
int check_boundaryX(GameInfo_t *game, int shift);
int check_rotation(GameInfo_t *game, int **rotated, int size);

#endif