#include "fsm_table.h"

#include "backend.h"

/*fsm table*/
action fsm_table[8][8] = {
    {spawn, NULL, exitstate, NULL, NULL, NULL, NULL, NULL},
    {spawn, stop, spawn, spawn, spawn, spawn, spawn, spawn},
    {NULL, stop, exitstate, moveleft, moveright, movedown, rotate, shifting},
    {shifting, stop, shifting, shifting, shifting, shifting, shifting,
     shifting},
    {attach, stop, attach, attach, attach, attach, attach, attach},
    {spawn, NULL, exitstate, NULL, NULL, NULL, NULL, NULL},
    {exitstate, exitstate, exitstate, exitstate, exitstate, exitstate,
     exitstate, exitstate},
    {NULL, stop, exitstate, NULL, NULL, NULL, NULL, NULL}};

void userInput(UserAction_t signal, GameInfo_t *game, State_t *state) {
  action act = NULL;
  Params_t prms;
  prms.game = game;
  prms.state = state;

  if (*state != FILE_ERROR) {
    act = fsm_table[*state][signal];
  }
  if (act) {
    act(&prms);
  }
}

UserAction_t get_signal(char input) {
  UserAction_t output = Nosig;

  if (input == '\033') {
    getch();
    char c = getch();
    if (c == 'C') {
      output = Right;
    } else if (c == 'D') {
      output = Left;
    } else if (c == 'B') {
      output = Down;
    }
  } else if (input == ' ') {
    output = Pause;
  } else if (input == 'q' || input == 'Q') {
    output = Terminate;
  } else if (input == '\n') {
    output = Action;
  } else if (input == 'S' || input == 's') {
    output = Start;
  }
  return output;
}

void spawn(Params_t *prms) {
  wtimeout(stdscr, (TIMEOUT - prms->game->speed * 15));
  if (prms->game->id == 0) {
    generate_piece(prms->game);
  }
  *prms->state = MOVE;
  spawn_piece(prms->game);
  if (generate_piece(prms->game) != SUCCESS) {
    *prms->state = FILE_ERROR;
  }
}

void shifting(Params_t *prms) {
  if (check_boundaryY(prms->game) == SUCCESS) {
    shift(prms->game);
    *prms->state = MOVE;
  } else {
    collide(prms->game);
    *prms->state = ATTACH;
  }
}

void movedown(Params_t *prms) {
  while (check_boundaryY(prms->game) == SUCCESS) {
    shift(prms->game);
  }
  *prms->state = MOVE;
}

void moveright(Params_t *prms) {
  if (check_boundaryX(prms->game, 1) == SUCCESS) {
    move_right(prms->game);
  }
  *prms->state = SHIFT;
}

void moveleft(Params_t *prms) {
  if (check_boundaryX(prms->game, -1) == SUCCESS) {
    move_left(prms->game);
  }
  *prms->state = SHIFT;
}

void rotate(Params_t *prms) {
  rotate_piece(prms->game);
  *prms->state = SHIFT;
}

void attach(Params_t *prms) {
  if (prms->game->y == SPAWN_Y) {
    *prms->state = GAMEOVER;
  } else {
    count_points(prms->game);
    *prms->state = SPAWN;
  }
}

void stop(Params_t *prms) {
  if (prms->game->pause) {
    prms->game->pause = 0;
    *prms->state = SHIFT;
  } else {
    prms->game->pause = 1;
    *prms->state = PAUSE;
  }
}

void exitstate(Params_t *prms) { *prms->state = EXIT; }