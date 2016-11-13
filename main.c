#include <stdlib.h>
#include <ncurses.h>

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 40

struct object {
  int x;
  int y;
  char ch;
};

void object_move(struct object *o, int x, int y) {
  (*o).x += x;
  (*o).y += y;
}

void screen_setup()
{
  initscr(); // creates stdscr
  noecho(); // disables typing on screen
  refresh(); // display screen in terminal
  // nodelay(stdscr, TRUE); // prevents pausing for getch()
  keypad(stdscr, TRUE); // enables use of function keys (F1, F2, ...)
  curs_set(0); // makes cursor invisible
  /* start_color(); */

  /* color definitions */
  /* init_pair(1, COLOR_RED, 0); */
}

int handle_keys(struct object *p)
{
  switch (getch()) {
    case 'q':
    case 'Q':
      return 1;

    case KEY_UP:
    case 'k':
    case 'K':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).y -= 1;
      break;

    case KEY_DOWN:
    case 'j':
    case 'J':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).y += 1;
      break;

    case KEY_LEFT:
    case 'h':
    case 'H':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).x -= 1;
      break;

    case KEY_RIGHT:
    case 'l':
    case 'L':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).x += 1;
      break;

    case 'y':
    case 'Y':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).x -= 1;
      (*p).y -= 1;
      break;

    case 'u':
    case 'U':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).x += 1;
      (*p).y -= 1;
      break;

    case 'b':
    case 'B':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).x -= 1;
      (*p).y += 1;
      break;

    case 'n':
    case 'N':
      mvaddch((*p).y, (*p).x, ' ');
      (*p).x += 1;
      (*p).y += 1;
      break;

    default:
      break;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int playerx = SCREEN_WIDTH / 2;
  int playery = SCREEN_HEIGHT / 2;

  struct object player = { playerx, playery, '@' };

  /* set up ncurses */
  screen_setup();

  /* main game loop */
  while (1) {
    // move player
    mvaddch(player.y, player.x, player.ch);

    // if handle_keys returns 1, exit
    // otherwise, move character
    if ((handle_keys(&player)))
      break;
  }

  endwin();
  return 0;
}
