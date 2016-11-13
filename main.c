#include <stdlib.h>
#include <ncurses.h>

int screenSetup() {
  initscr(); // creates stdscr
  noecho(); // disables typing on screen
  refresh(); // display screen in terminal
  keypad(stdscr, TRUE); // enables use of function keys (F1, F2, ...)
  curs_set(0); // makes cursor invisible

  return 1;
}

int main(int argc, char *argv[]) {
  char ch;

  screenSetup();

  /* main game loop */
  while ((ch = getch()) != 'q') {

  }

  endwin();
  return 0;
}
