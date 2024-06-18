.PHONY: tetris install uninstall dvi dist test assets gcov_report check leaks rebuild clean clean_test clean_assets clean_report

CC = gcc -Wall -Wextra -Werror -std=c11 $(shell pkg-config --cflags check)
OS = $(shell uname -s)
GCOVFLAGS = -fprofile-arcs -ftest-coverage
LIBS = $(shell pkg-config --libs check)
PROJECT_DIR = tetris
SRC_DIR = ../../brick_game/tetris
DIST_DIR = tetris-1.0
HEADERS = $(PROJECT_DIR)/backend.h $(PROJECT_DIR)/fsm_table.h
SOURCES = $(wildcard *.c)
SRC = $(PROJECT_DIR)/tetris.c $(PROJECT_DIR)/backend.c $(PROJECT_DIR)/fsm_table.c
TEST_SRC = $(PROJECT_DIR)/backend.c $(PROJECT_DIR)/fsm_table.c

OBJ_LIBRARY := $(patsubst %.c, %.o, $(PROJECT_DIR)/backend.c)

all: clean install tetris test gcov_report

install: 
	mkdir $(PROJECT_DIR)
	cp tetris.c $(PROJECT_DIR)/tetris.c
	cp $(SRC_DIR)/backend.c $(PROJECT_DIR)/backend.c
	cp $(SRC_DIR)/backend.h $(PROJECT_DIR)/backend.h
	cp $(SRC_DIR)/fsm_table.h $(PROJECT_DIR)/fsm_table.h
	cp $(SRC_DIR)/fsm_table.c $(PROJECT_DIR)/fsm_table.c
	cp $(SRC_DIR)/tests.c $(PROJECT_DIR)/tests.c
	cp -r $(SRC_DIR)/assets/ $(PROJECT_DIR)/assets/

uninstall:
	rm -rf tetris/

tetris: $(SRC) $(HEADERS)
	$(CC) $(SRC) $(HEADERS) -o $(PROJECT_DIR)/tetris.out -lncurses

backend.a: $(OBJ_LIBRARY) $(PROJECT_DIR)/backend.h
	ar rcs backend.a $(OBJ_LIBRARY)
	ranlib backend.a
	rm -rf *.o $(PROJECT_DIR)/*.o

%.o: %.c $(HEADER)
	$(CC) -c $< -o $@

test: assets backend.a $(PROJECT_DIR)/tests.c
	$(CC) $(PROJECT_DIR)/tests.c $(PROJECT_DIR)/fsm_table.c backend.a -o test.out $(LIBS) -lncurses
	./test.out

assets: 
	cp -r $(PROJECT_DIR)/assets ./assets

clean_assets:
	rm -rf ./assets

gcov_report: clean_test
	$(CC) $(GCOVFLAGS) $(PROJECT_DIR)/tests.c ${TEST_SRC} -o test.out $(LIBS) -lncurses
	./test.out
	lcov -t "stest" -o s21_test.info -c -d .
	genhtml -o report s21_test.info
	make clean_assets
ifeq ($(OS), Linux)
	links ./report/index.html
else
	open -a safari ./report/index.html
endif

check:
	cppcheck --enable=all --suppress=missingIncludeSystem tetris/
	clang-format -n tetris/*.c
	clang-format -n tetris/*.h

leaks: assets
ifeq ($(OS), Darwin)
	$(CC) $(SRC) $(HEADERS) -o $(PROJECT_DIR)/tetris.out $(LIBS) -lncurses
	leaks --atExit -- $(PROJECT_DIR)/tetris.out
else
	$(CC) $(SRC) $(HEADERS) -o $(PROJECT_DIR)/tetris.out $(LIBS) -lncurses
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes --log-file=valgrind_log.txt tetris/tetris.out
	grep ERROR valgrind_log.txt
endif
	rm -f $(PROJECT_DIR)/tetris.out
	make clean_assets

dvi: tetris_man.dvi

tetris_man.dvi: tetris_man.tex
	texi2dvi tetris_man.tex

dist: 
	mkdir $(DIST_DIR)
	mkdir $(DIST_DIR)/gui
	mkdir $(DIST_DIR)/gui/cli
	mkdir $(DIST_DIR)/brick_game
	mkdir $(DIST_DIR)/brick_game/tetris
	cp tetris.c $(DIST_DIR)/gui/cli/tetris.c
	cp Makefile $(DIST_DIR)/gui/cli/Makefile
	cp tetris_man.tex $(DIST_DIR)/gui/cli/tetris_man.tex
	cp $(SRC_DIR)/backend.c $(DIST_DIR)/brick_game/tetris/backend.c
	cp $(SRC_DIR)/backend.h $(DIST_DIR)/brick_game/tetris/backend.h
	cp $(SRC_DIR)/fsm_table.h $(DIST_DIR)/brick_game/tetris/fsm_table.h
	cp $(SRC_DIR)/fsm_table.c $(DIST_DIR)/brick_game/tetris/fsm_table.c
	cp $(SRC_DIR)/tests.c $(DIST_DIR)/brick_game/tetris/tests.c
	cp -r $(SRC_DIR)/assets/ $(DIST_DIR)/brick_game/tetris/assets/
	tar -czvf $(DIST_DIR).tar.gz $(DIST_DIR)
	rm -rf $(DIST_DIR)

rebuild: clean all

clean:
	rm -f *.o *.a *.gcno *.gcda *.info *.out *.gcov *.aux *.dvi *.log *.toc *.tar *.gz *.txt
	rm -rf test
	rm -rf ./report
	rm -rf *.dSYM 
	rm -rf ./$(PROJECT_DIR) ./assets

clean_test:
	rm -rf *.gcda
	rm -rf *.gcno
	rm -rf *.info
	rm -rf test
	rm -rf report

clean_report:
	rm -f *.o *.a *.gcno *.gcda *.info *.out *.gcov
	rm -rf ./report
	rm -rf *.dSYM 