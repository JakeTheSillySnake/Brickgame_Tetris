#include <check.h>
#include <time.h>
#include <unistd.h>

#include "backend.h"
#include "fsm_table.h"

START_TEST(updateCurrentState_1) {
  GameInfo_t game = {0};
  State_t state = START;
  updateCurrentState(game, 's', &state);
  ck_assert_int_eq(state, MOVE);
}
END_TEST

START_TEST(updateCurrentState_2) {
  GameInfo_t game = {0};
  game.score = 300;
  State_t state = SPAWN;
  game = updateCurrentState(game, '\n', &state);
  ck_assert_int_eq(game.score, game.high_score);
}
END_TEST

START_TEST(updateCurrentState_3) {
  GameInfo_t game = {0};
  State_t state = START;
  updateCurrentState(game, '\033', &state);
  ck_assert_int_eq(state, START);
}
END_TEST

START_TEST(updateCurrentState_4) {
  GameInfo_t game = {0};
  State_t state = SHIFT;
  updateCurrentState(game, 't', &state);
  ck_assert_int_eq(state, MOVE);
}
END_TEST

START_TEST(updateCurrentState_5) {
  GameInfo_t game = {0};
  State_t state = START;
  updateCurrentState(game, 'q', &state);
  ck_assert_int_eq(state, EXIT);
}
END_TEST

START_TEST(get_signal_1) {
  char c = ' ';
  int result = get_signal(c);
  ck_assert_int_eq(result, Pause);
}
END_TEST

START_TEST(gameover_1) {
  GameInfo_t game = {0};
  State_t state = ATTACH;
  game.x = SPAWN_X;
  game.y = SPAWN_Y;
  updateCurrentState(game, 'q', &state);
  ck_assert_int_eq(state, GAMEOVER);
}
END_TEST

START_TEST(init_stats_1) {
  GameInfo_t game = {0};
  State_t state = START;
  init_stats(&game, &state);
  ck_assert_int_eq(game.level, 1);
}
END_TEST

START_TEST(shift_1) {
  GameInfo_t game = {0};
  game.field[0][0] = MOVING;
  shift(&game);
  ck_assert_int_eq(game.field[1][0], MOVING);
}
END_TEST

START_TEST(spawn_1) {
  GameInfo_t game = {0};
  game.id = SQUARE;
  spawn_piece(&game);
  ck_assert_int_eq(game.length, 2);
}
END_TEST

START_TEST(spawn_2) {
  GameInfo_t game = {0};
  game.id = LINE;
  spawn_piece(&game);
  ck_assert_int_eq(game.length, 4);
}
END_TEST

START_TEST(rotate_piece_1) {
  GameInfo_t game = {0};
  State_t state = SPAWN;
  updateCurrentState(game, 't', &state);
  updateCurrentState(game, '\n', &state);
  ck_assert_int_eq(state, SHIFT);
}
END_TEST

START_TEST(rotate_piece_2) {
  GameInfo_t game = {0};
  game.field[0][0] = MOVING;
  game.field[1][0] = MOVING;
  game.field[2][0] = MOVING;
  game.x = -1;
  game.length = 3;
  rotate_piece(&game);
  ck_assert_int_eq(game.field[0][0], MOVING);
}
END_TEST

START_TEST(rotate_piece_3) {
  GameInfo_t game = {0};
  game.field[0][COLUMNS - 1] = MOVING;
  game.field[1][COLUMNS - 1] = MOVING;
  game.field[2][COLUMNS - 1] = MOVING;
  game.x = COLUMNS - 1;
  game.length = 3;
  rotate_piece(&game);
  ck_assert_int_eq(game.field[0][COLUMNS - 1], MOVING);
}
END_TEST

START_TEST(rotate_piece_4) {
  GameInfo_t game = {0};
  game.field[0][1] = MOVING;
  game.field[1][1] = MOVING;
  game.field[2][1] = MOVING;
  game.x = 0;
  game.length = 3;
  rotate_piece(&game);
  ck_assert_int_eq(game.field[0][1], EMPTY);
}
END_TEST

START_TEST(collide_1) {
  GameInfo_t game = {0};
  State_t state = SHIFT;
  game.field[ROWS + LENGTH_MAX - 1][0] = MOVING;
  updateCurrentState(game, 't', &state);
  ck_assert_int_eq(state, ATTACH);
}
END_TEST

START_TEST(collide_2) {
  GameInfo_t game = {0};
  game.field[ROWS + LENGTH_MAX - 1][0] = MOVING;
  collide(&game);
  ck_assert_int_eq(game.field[ROWS + LENGTH_MAX - 1][0], STATIC);
}
END_TEST

START_TEST(count_points_1) {
  GameInfo_t game = {0};
  State_t state = ATTACH;
  for (int j = 0; j < COLUMNS; j++) {
    game.field[ROWS + LENGTH_MAX - 1][j] = MOVING;
  }
  game = updateCurrentState(game, 't', &state);
  ck_assert_int_eq(game.score, POINTS);
}
END_TEST

START_TEST(count_points_2) {
  GameInfo_t game = {0};
  game.level = 11;
  game.score = 300000;
  count_points(&game);
  ck_assert_int_eq(game.level, 10);
}
END_TEST

START_TEST(move_left_1) {
  GameInfo_t game = {0};
  game.field[ROWS + LENGTH_MAX - 1][1] = MOVING;
  move_left(&game);
  ck_assert_int_eq(game.field[ROWS + LENGTH_MAX - 1][0], MOVING);
}
END_TEST

START_TEST(move_left_2) {
  GameInfo_t game = {0};
  State_t state = MOVE;
  game.field[ROWS + LENGTH_MAX - 1][1] = MOVING;
  Params_t prms;
  prms.game = &game;
  prms.state = &state;
  moveleft(&prms);
  ck_assert_int_eq(game.field[ROWS + LENGTH_MAX - 1][0], MOVING);
}
END_TEST

START_TEST(move_right_1) {
  GameInfo_t game = {0};
  game.field[ROWS + LENGTH_MAX - 1][0] = MOVING;
  move_right(&game);
  ck_assert_int_eq(game.field[ROWS + LENGTH_MAX - 1][1], MOVING);
}
END_TEST

START_TEST(move_right_2) {
  GameInfo_t game = {0};
  State_t state = MOVE;
  game.field[ROWS + LENGTH_MAX - 1][0] = MOVING;
  Params_t prms;
  prms.game = &game;
  prms.state = &state;
  moveright(&prms);
  ck_assert_int_eq(game.field[ROWS + LENGTH_MAX - 1][1], MOVING);
}
END_TEST

START_TEST(check_boundaryX_1) {
  GameInfo_t game = {0};
  game.field[ROWS + LENGTH_MAX - 1][0] = MOVING;
  int result = check_boundaryX(&game, -1);
  ck_assert_int_eq(result, FAILURE);
}
END_TEST

START_TEST(check_boundaryX_2) {
  GameInfo_t game = {0};
  game.field[ROWS + LENGTH_MAX - 1][COLUMNS - 1] = MOVING;
  int result = check_boundaryX(&game, 1);
  ck_assert_int_eq(result, FAILURE);
}
END_TEST

START_TEST(check_boundaryX_3) {
  GameInfo_t game = {0};
  game.field[0][0] = MOVING;
  int result = check_boundaryX(&game, 1);
  ck_assert_int_eq(result, SUCCESS);
}
END_TEST

START_TEST(check_boundaryX_4) {
  GameInfo_t game = {0};
  game.field[COLUMNS - 1][COLUMNS - 1] = MOVING;
  int result = check_boundaryX(&game, -1);
  ck_assert_int_eq(result, SUCCESS);
}
END_TEST

START_TEST(move_down_1) {
  GameInfo_t game = {0};
  State_t state = MOVE;
  game.field[0][0] = MOVING;
  Params_t prms;
  prms.game = &game;
  prms.state = &state;
  movedown(&prms);
  ck_assert_int_eq(game.field[ROWS + LENGTH_MAX - 1][0], MOVING);
}
END_TEST

START_TEST(check_boundaryY_1) {
  GameInfo_t game = {0};
  game.field[ROWS + LENGTH_MAX - 1][0] = MOVING;
  int result = check_boundaryY(&game);
  ck_assert_int_eq(result, FAILURE);
}
END_TEST

START_TEST(check_boundaryY_2) {
  GameInfo_t game = {0};
  game.field[0][0] = MOVING;
  int result = check_boundaryY(&game);
  ck_assert_int_eq(result, SUCCESS);
}
END_TEST

START_TEST(pause_1) {
  GameInfo_t game = {0};
  State_t state = SPAWN;
  game = updateCurrentState(game, ' ', &state);
  ck_assert_int_eq(game.pause, 1);
}
END_TEST

START_TEST(pause_2) {
  GameInfo_t game = {0};
  State_t state = PAUSE;
  game.pause = 1;
  updateCurrentState(game, ' ', &state);
  ck_assert_int_eq(state, SHIFT);
}
END_TEST

Suite *backend_suite(void) {
  Suite *suite;
  suite = suite_create("backend");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_test(tcase_core, updateCurrentState_1);
  tcase_add_test(tcase_core, updateCurrentState_2);
  tcase_add_test(tcase_core, updateCurrentState_3);
  tcase_add_test(tcase_core, updateCurrentState_4);
  tcase_add_test(tcase_core, updateCurrentState_5);
  tcase_add_test(tcase_core, get_signal_1);
  tcase_add_test(tcase_core, gameover_1);
  tcase_add_test(tcase_core, init_stats_1);
  tcase_add_test(tcase_core, shift_1);
  tcase_add_test(tcase_core, spawn_1);
  tcase_add_test(tcase_core, spawn_2);
  tcase_add_test(tcase_core, rotate_piece_1);
  tcase_add_test(tcase_core, rotate_piece_2);
  tcase_add_test(tcase_core, rotate_piece_3);
  tcase_add_test(tcase_core, rotate_piece_4);
  tcase_add_test(tcase_core, collide_1);
  tcase_add_test(tcase_core, collide_2);
  tcase_add_test(tcase_core, count_points_1);
  tcase_add_test(tcase_core, count_points_2);
  tcase_add_test(tcase_core, move_left_1);
  tcase_add_test(tcase_core, move_left_2);
  tcase_add_test(tcase_core, move_right_1);
  tcase_add_test(tcase_core, move_right_2);
  tcase_add_test(tcase_core, check_boundaryX_1);
  tcase_add_test(tcase_core, check_boundaryX_2);
  tcase_add_test(tcase_core, check_boundaryX_3);
  tcase_add_test(tcase_core, check_boundaryX_4);
  tcase_add_test(tcase_core, move_down_1);
  tcase_add_test(tcase_core, check_boundaryY_1);
  tcase_add_test(tcase_core, check_boundaryY_2);
  tcase_add_test(tcase_core, pause_1);
  tcase_add_test(tcase_core, pause_2);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

int main(void) {
  Suite *suite = backend_suite();
  SRunner *suite_runner = srunner_create(suite);
  srunner_run_all(suite_runner, CK_VERBOSE);
  int failed_count = srunner_ntests_failed(suite_runner);
  srunner_free(suite_runner);
  return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}