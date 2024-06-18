#include <unistd.h>

#include "backend.h"

void set_up();
void gameloop(GameInfo_t *game, State_t *state);
void print_field(GameInfo_t *game, State_t state);
void print_stats(GameInfo_t *game, int i, State_t state);
void gameover(GameInfo_t *game, State_t state);
void terminate(GameInfo_t *game);
void print_error();

int main() {
  GameInfo_t game, init = {0};
  State_t state = 0;
  set_up();
  while (state != EXIT && state != FILE_ERROR) {
    game = init;
    init_stats(&game, &state);
    gameloop(&game, &state);
    while (state == GAMEOVER) {
      game = updateCurrentState(game, getch(), &state);
    }
  }
  gameover(&game, state);
  return 0;
}

void set_up() {
  cbreak();
  initscr();
  noecho();
  nodelay(stdscr, TRUE);
  srand(time(NULL));
}

void gameloop(GameInfo_t *game, State_t *state) {
  while (1) {
    print_field(game, *state);
    if (*state == EXIT || *state == FILE_ERROR || *state == GAMEOVER) {
      break;
    } else if (*state == START) {
      wtimeout(stdscr, (TIMEOUT - game->speed * 15));
    }
    *game = updateCurrentState(*game, getch(), state);
  }
}

void print_field(GameInfo_t *game, State_t state) {
  clear();
  refresh();
  printf("ARROWS: < > move, ⌄ speed up\n\r");
  printf(" __________\n\r");
  for (int i = LENGTH_MAX; i < ROWS + LENGTH_MAX; i++) {
    printf("|");
    for (int j = 0; j < COLUMNS; j++) {
      if (game->field[i][j]) {
        printf("#");
      } else {
        printf(" ");
      }
    }
    printf("|");
    print_stats(game, i, state);
    printf("\n\r");
  }
  printf(" ----------\n\r");
  printf("\n\rSPACE start/pause, ENTER rotate\n\n\rQ quit");
  fflush(stdout);
}

void print_stats(GameInfo_t *game, int i, State_t state) {
  if (i == 4) {
    printf("  BEST SCORE: %d", game->high_score);
  } else if (i == 5) {
    printf("  SCORE: %d", game->score);
  } else if (i == 6) {
    printf("  LEVEL: %d", game->level);
  } else if (i == 8) {
    printf("  NEXT:");
  } else if (i > 9 && i < 12) {
    printf("  ");
    for (int j = 0; j < 4; j++) {
      if (game->next[i - 8][j]) {
        printf("#");
      } else {
        printf(" ");
      }
    }
  } else if (i == 15 && state == START) {
    printf("  PRESS S TO START");
  } else if (i == 15 && state == GAMEOVER) {
    printf("  GAME OVER! PRESS S TO REPLAY");
  }
}

void gameover(GameInfo_t *game, State_t state) {
  refresh();
  endwin();
  if (state == FILE_ERROR) {
    print_error();
  } else if (state == EXIT) {
    terminate(game);
  }
}

void terminate(GameInfo_t *game) {
  printf("\nGood game!\nYour Score: %d\nHighest Score: %d\n\n", game->score,
         game->high_score);
}

void print_error() {
  printf(
      "\nError! Some files are missing or damaged.\nPlease check that ./assets "
      "folder contains\n7 files labelled 1-7.txt.\n1 file labelled "
      "high_score.txt\n\n");
}