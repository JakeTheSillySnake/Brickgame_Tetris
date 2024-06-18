#ifndef FSM_H
#define FSM_H

#include "backend.h"

/*fsm table dependants*/
typedef struct {
  GameInfo_t *game;
  State_t *state;
} Params_t;

typedef void (*action)(Params_t *prms);

/*state functions*/
void spawn(Params_t *prms);
void movedown(Params_t *prms);
void moveright(Params_t *prms);
void moveleft(Params_t *prms);
void shifting(Params_t *prms);
void rotate(Params_t *prms);
void attach(Params_t *prms);
void stop(Params_t *prms);
void exitstate(Params_t *prms);

/*functions for input processing*/
UserAction_t get_signal(char input);
void userInput(UserAction_t signal, GameInfo_t *game, State_t *state);

#endif