#include <stdlib.h>
#include <ncurses.h>

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 40

struct object {
  int x;
  int y;
  char ch;
};

struct tile {
  char ch;
};

void object_move(struct object *o, int x, int y) {
  o->x += x;
  o->y += y;
}

void object_draw(struct object *o) {
  mvaddch(o->y, o->x, o->ch);
}

void object_clear(struct object *o) {
  mvaddch(o->y, o->x, ' ');
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
      object_clear(p);
      p->y--;
      break;

    case KEY_DOWN:
    case 'j':
    case 'J':
      object_clear(p);
      p->y++;
      break;

    case KEY_LEFT:
    case 'h':
    case 'H':
      object_clear(p);
      p->x--;
      break;

    case KEY_RIGHT:
    case 'l':
    case 'L':
      object_clear(p);
      p->x++;
      break;

    case 'y':
    case 'Y':
      object_clear(p);
      p->x--;
      p->y--;
      break;

    case 'u':
    case 'U':
      object_clear(p);
      p->x++;
      p->y--;
      break;

    case 'b':
    case 'B':
      object_clear(p);
      p->x--;
      p->y++;
      break;

    case 'n':
    case 'N':
      object_clear(p);
      p->x++;
      p->y++;
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
    object_draw(&player);

    // if handle_keys returns 1, exit
    // otherwise, move character
    if ((handle_keys(&player)))
      break;
  }

  endwin();
  return 0;
}
