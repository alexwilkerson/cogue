#include <stdlib.h>
#include <ncurses.h>

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 40

void screenSetup()
{
  initscr(); // creates stdscr
  noecho(); // disables typing on screen
  refresh(); // display screen in terminal
  // nodelay(stdscr, TRUE); // prevents pausing for getch()
  keypad(stdscr, TRUE); // enables use of function keys (F1, F2, ...)
  curs_set(0); // makes cursor invisible
  /* start_color(); */

  /* color definitions */
  /* init_pair(1, COLOR_RED, COLOR_BLACK); */
}

int handle_keys(int *y, int *x)
{
  switch (getch()) {
    case 'q':
    case 'Q':
      return 1;

    case KEY_UP:
    case 'k':
    case 'K':
      mvaddch(*y, *x, ' ');
      *y -= 1;
      break;

    case KEY_DOWN:
    case 'j':
    case 'J':
      mvaddch(*y, *x, ' ');
      *y += 1;
      break;

    case KEY_LEFT:
    case 'h':
    case 'H':
      mvaddch(*y, *x, ' ');
      *x -= 1;
      break;

    case KEY_RIGHT:
    case 'l':
    case 'L':
      mvaddch(*y, *x, ' ');
      *x += 1;
      break;

    case 'y':
    case 'Y':
      mvaddch(*y, *x, ' ');
      *x -= 1;
      *y -= 1;
      break;

    case 'u':
    case 'U':
      mvaddch(*y, *x, ' ');
      *x += 1;
      *y -= 1;
      break;

    case 'b':
    case 'B':
      mvaddch(*y, *x, ' ');
      *x -= 1;
      *y += 1;
      break;

    case 'n':
    case 'N':
      mvaddch(*y, *x, ' ');
      *x += 1;
      *y += 1;
      break;

    default:
      break;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int playerx = SCREEN_WIDTH / 2;
  int playery = SCREEN_HEIGHT / 2;

  /* set up ncurses */
  screenSetup();

  /* main game loop */
  while (1) {
    // move player
    mvaddch(playery, playerx, '@');

    // if handle_keys returns 1, exit
    // otherwise, move character
    if ((handle_keys(&playery, &playerx)))
      break;
  }

  endwin();
  return 0;
}
