# CLI Tetris

## A Few Words...

All documentation resides in the `src/gui/cli/tetris_man.tex` file. You can open and skim it in your app of choice or convert to .dvi with `make dvi`. Makefile is in the `src/gui/cli` folder to help you with installation, compilation and testing of the tetris game.

If you know what you're doing and want to go straight to playing, set the game up with `make install` and `make tetris`, then go inside the newly created `tetris` folder and run  `./tetris.out`. Use Arrows to move the pieces, Enter to rotate them clockwise, Space to pause and Q to quit. Collect as many points as you can by making rows and don't be afraid to close the game -- your best score will be saved.
