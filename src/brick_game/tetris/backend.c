#include "backend.h"

#include "fsm_table.h"

GameInfo_t updateCurrentState(GameInfo_t game, char input, State_t *state) {
  GameInfo_t temp = game;
  userInput(get_signal(input), &temp, state);
  if (temp.score >= temp.high_score) {
    temp.high_score = temp.score;
  }
  FILE *fptr = NULL;
  fptr = fopen(SCORE_PATH, "w");
  if (fptr) {
    fprintf(fptr, "%d", temp.high_score);
    fclose(fptr);
  } else {
    *state = FILE_ERROR;
  }
  return temp;
}

void init_stats(GameInfo_t *game, State_t *state) {
  FILE *fptr = NULL;
  *state = START;
  game->level = 1;
  game->speed = 1;
  fptr = fopen(SCORE_PATH, "r");
  if (fptr) {
    fscanf(fptr, "%d", &game->high_score);
    fclose(fptr);
  } else {
    *state = FILE_ERROR;
  }
}

void shift(GameInfo_t *game) {
  game->y++;
  for (int i = ROWS + LENGTH_MAX - 1; i >= 0; i--) {
    for (int j = COLUMNS - 1; j >= 0; j--) {
      if (game->field[i][j] == MOVING) {
        game->field[i + 1][j] = MOVING;
        game->field[i][j] = EMPTY;
      }
    }
  }
}

void collide(GameInfo_t *game) {
  for (int i = 0; i < ROWS + LENGTH_MAX; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      if (game->field[i][j] == MOVING) {
        game->field[i][j] = STATIC;
      }
    }
  }
}

void move_left(GameInfo_t *game) {
  for (int i = 0; i < ROWS + LENGTH_MAX; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      if (game->field[i][j] == MOVING) {
        game->field[i][j - 1] = MOVING;
        game->field[i][j] = EMPTY;
      }
    }
  }
  game->x--;
}

void move_right(GameInfo_t *game) {
  for (int i = ROWS + LENGTH_MAX - 1; i >= 0; i--) {
    for (int j = COLUMNS - 1; j >= 0; j--) {
      if (game->field[i][j] == MOVING) {
        game->field[i][j + 1] = MOVING;
        game->field[i][j] = EMPTY;
      }
    }
  }
  game->x++;
}

int generate_piece(GameInfo_t *game) {
  int output = SUCCESS, random = rand() % FILE_COUNT;
  int file = (random > 0 ? random : 1);
  FILE *fptr = NULL;
  char buffer[300];
  sprintf(buffer, "%s%d.txt", FILES_PATH, file);
  fptr = fopen(buffer, "r");
  if (fptr) {
    for (int i = 0; i < LENGTH_MAX; i++) {
      for (int j = 0; j < LENGTH_MAX; j++) {
        fscanf(fptr, "%1d", &game->next[i][j]);
      }
    }
    game->id = file;
    fclose(fptr);
  } else {
    output = FAILURE;
  }
  return output;
}

void spawn_piece(GameInfo_t *game) {
  for (int i = 0; i < LENGTH_MAX; i++) {
    for (int j = 0; j < LENGTH_MAX; j++) {
      if (game->next[i][j]) {
        game->field[i][j + SPAWN_X] = MOVING;
      }
    }
  }
  game->y = SPAWN_Y;
  game->x = SPAWN_X;
  game->length = 3;
  if (game->id == SQUARE) {
    game->length = 2;
  } else if (game->id == LINE) {
    game->length = 4;
  }
}

void rotate_piece(GameInfo_t *game) {
  int size = game->length;
  int **rotated = calloc(size, sizeof(int *));

  for (int i = 0; i < size; i++) {
    rotated[i] = calloc(size, sizeof(int));
  }
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (game->field[i + game->y][j + game->x] != STATIC) {
        rotated[j][i] = game->field[i + game->y][j + game->x];  // transposing
      }
    }
  }
  reverse_rows(rotated, size);
  int check = check_rotation(game, rotated, size);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (game->field[i + game->y][j + game->x] != STATIC && check == SUCCESS) {
        game->field[i + game->y][j + game->x] = rotated[i][j];
      }
    }
  }
  for (int i = 0; i < size; i++) {
    free(rotated[i]);
  }
  free(rotated);
}

void reverse_rows(int **matrix, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size / 2; j++) {
      int temp = matrix[i][j];
      matrix[i][j] = matrix[i][size - 1 - j];
      matrix[i][size - 1 - j] = temp;
    }
  }
}

void count_points(GameInfo_t *game) {
  for (int i = 0; i < ROWS + LENGTH_MAX; i++) {
    int count = 0;
    for (int j = 0; j < COLUMNS; j++) {
      if (game->field[i][j]) {
        count++;
      }
    }
    if (count == COLUMNS) {
      erase_line(game, i);
      game->score *= 2;
      game->score += POINTS;
    }
  }
  game->level = game->score / 600 + 1;
  if (game->level > 10) {
    game->level = 10;
  }
  game->speed = game->level;
}

void erase_line(GameInfo_t *game, int index) {
  for (int j = 0; j < COLUMNS; j++) {
    game->field[index][j] = EMPTY;
    for (int i = index; i > LENGTH_MAX; i--) {
      game->field[i][j] = game->field[i - 1][j];
    }
  }
}

int check_boundaryY(GameInfo_t *game) {
  int result = SUCCESS;
  for (int i = 0; i < ROWS + LENGTH_MAX; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      if (game->field[i][j] == MOVING &&
          (i + 1 >= ROWS + LENGTH_MAX || game->field[i + 1][j] == STATIC)) {
        result = FAILURE;
      }
    }
  }
  return result;
}

int check_boundaryX(GameInfo_t *game, int shift) {
  int result = SUCCESS;
  for (int i = 0; i < ROWS + LENGTH_MAX; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      if (game->field[i][j] == MOVING &&
          (j + shift >= COLUMNS || j + shift < 0 ||
           game->field[i][j + shift] == STATIC)) {
        result = FAILURE;
      }
    }
  }
  return result;
}

int check_rotation(GameInfo_t *game, int **rotated, int size) {
  int result = SUCCESS;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (rotated[i][j] == MOVING &&
          (game->field[i + game->y][j + game->x] == STATIC || game->x < 0 ||
           game->x > COLUMNS - game->length)) {
        result = FAILURE;
      }
    }
  }
  return result;
}