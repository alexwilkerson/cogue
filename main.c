#include <stdlib.h>
#include <ncurses.h>

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 40

#define MAP_WIDTH  48
#define MAP_HEIGHT 25

struct object {
  int x;
  int y;
  char ch;
};

struct tile {
  char ch;
  short blocked;
  short blocked_sight;
};

void map_make(struct tile map[MAP_HEIGHT][MAP_WIDTH])
{
  int i, j;

  struct tile base_tile = { '.', 0, 0 };

  for (i = 0; i < MAP_HEIGHT; i++) {
    for (j = 0; j < MAP_WIDTH; j++) {
      map[i][j] = base_tile;
    }
  }

  map[10][10].ch = '#';
  map[10][10].blocked = 1;
  map[10][10].blocked_sight = 1;
  map[15][10].ch = '#';
  map[15][10].blocked = 1;
  map[15][10].blocked_sight = 1;

}

void map_draw(WINDOW *win, struct tile map[MAP_HEIGHT][MAP_WIDTH])
{
  int i, j;
  for (i = 0; i < MAP_HEIGHT; i++) {
    for (j = 0; j < MAP_WIDTH; j++) {
      mvwaddch(win, i, j, map[i][j].ch);
    }
  }
}

void object_move(struct tile map[MAP_HEIGHT][MAP_WIDTH], struct object *o, int x, int y) {
  if (((o->x + x) >= 0)           && \
      ((o->x + x) < MAP_WIDTH)    && \
      ((o->y + y) >= 0)           && \
      ((o->y + y) < MAP_HEIGHT)  && \
      (!map[o->y + y][o->x + x].blocked)) {
    o->x += x;
    o->y += y;
  }
}

void object_draw(WINDOW *win, struct object *o) {
  mvwaddch(win, o->y, o->x, o->ch);
}

void object_clear(WINDOW *win, struct object *o) {
  mvwaddch(win, o->y, o->x, ' ');
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

int handle_keys(WINDOW *con, struct tile map[MAP_HEIGHT][MAP_WIDTH], struct object *p)
{
  switch (getch()) {
    case 'q':
    case 'Q':
      return 1;

    case KEY_UP:
    case 'k':
    case 'K':
      object_move(map, p, 0, -1);
      break;

    case KEY_DOWN:
    case 'j':
    case 'J':
      object_move(map, p, 0, 1);
      break;

    case KEY_LEFT:
    case 'h':
    case 'H':
      object_move(map, p, -1, 0);
      break;

    case KEY_RIGHT:
    case 'l':
    case 'L':
      object_move(map, p, 1, 0);
      break;

    case 'y':
    case 'Y':
      object_move(map, p, -1, -1);
      break;

    case 'u':
    case 'U':
      object_move(map, p, 1, -1);
      break;

    case 'b':
    case 'B':
      object_move(map, p, -1, 1);
      break;

    case 'n':
    case 'N':
      object_move(map, p, 1, 1);
      break;

    default:
      break;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int i;

  WINDOW *con;

  int playerx = MAP_WIDTH / 2;
  int playery = MAP_HEIGHT / 2;

  struct object player = { playerx, playery, '@' };
  struct object npc = { playerx, playery - 5, '@' };

  struct object *objects[] = {&npc, &player};
  int object_count = sizeof(objects) / sizeof(struct object *);

  /* set up ncurses */
  screen_setup();
  con = newwin(0, 0, 0, 0);
  if (!con) {
    addstr("Unable to allocate memory for new window.");
    endwin();
    return(1);
  }

  /* create the map */
  struct tile map[MAP_HEIGHT][MAP_WIDTH];
  map_make(map);

  /* main game loop */
  while (1) {
    map_draw(con, map);

    // display player
    for (i = 0; i < object_count; i++) {
      object_draw(con, objects[i]);
    }

    /* redraw the screen */
    wrefresh(con);

    /* todo: make array forloop and clear all */
    for (i = 0; i < object_count; i++)
      object_clear(con, objects[i]);

    // if handle_keys returns 1, exit
    // otherwise, move character
    if ((handle_keys(con, map, &player)))
      break;
  }

  endwin();
  return 0;
}
