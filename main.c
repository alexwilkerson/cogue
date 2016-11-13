#include <stdlib.h>
#include <ncurses.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 40

#define MAP_WIDTH  48
#define MAP_HEIGHT 25

struct object {
  int x;
  int y;
  char ch;
  int color;
};

struct tile {
  char ch;
  short blocked;
  short block_sight;
};
/* global map */
struct tile map[MAP_HEIGHT][MAP_WIDTH];

struct rect {
  int x1;
  int y1;
  int x2;
  int y2;
};

struct rect create_rect(int x, int y, int w, int h)
{
  struct rect r = { x, y, x + w, y + h };
  return r;
}

void create_room(struct rect room)
{
  int i, j;
  for (i = room.y1; i < room.y2; i++) {
    for (j = room.x1; j < room.x2; j++) {
      map[i][j].blocked = 0;
      map[i][j].block_sight = 0;
    }
  }
}

void create_h_tunnel(int x1, int x2, int y)
{
  int i;
  for (i = min(x1, x2); i < max(x1, x2); i++) {
    map[y][i].blocked = 0;
    map[y][i].block_sight = 0;
  }
}

void create_v_tunnel(int y1, int y2, int x)
{
  int i;
  for (i = min(y1, y2); i < max(y1, y2); i++) {
    map[i][x].blocked = 0;
    map[i][x].block_sight = 0;
  }
}

void map_make()
{
  int i, j;

  struct tile base_tile = { '#', 1, 1 };

  for (i = 0; i < MAP_HEIGHT; i++) {
    for (j = 0; j < MAP_WIDTH; j++) {
      map[i][j] = base_tile;
    }
  }

  struct rect room1 = create_rect(4, 5, 15, 5);
  struct rect room2 = create_rect(1, 1, 5, 5);
  struct rect room3 = create_rect(30, 12, 10, 10);
  create_room(room1);
  create_room(room2);
  create_room(room3);

  create_h_tunnel(6, 35, 6);
  create_v_tunnel(6, 18, 35);
}

void map_draw(WINDOW *win)
{
  int i, j;
  int wall;

  for (i = 0; i < MAP_HEIGHT; i++) {
    for (j = 0; j < MAP_WIDTH; j++) {
      wall = map[i][j].blocked;
      if (wall)
        mvwaddch(win, i, j, '#');
      else
        mvwaddch(win, i, j, '.');
    }
  }
}

void object_move(struct object *o, int x, int y) {
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
  wattron(win, COLOR_PAIR(o->color));
  mvwaddch(win, o->y, o->x, o->ch);
  wattroff(win, COLOR_PAIR(o->color));
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
  start_color();

  /* color definitions */
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  refresh();
}

int handle_keys(WINDOW *con, struct object *p)
{
  switch (getch()) {
    case 'q':
    case 'Q':
      return 1;

    case KEY_UP:
    case 'k':
    case 'K':
      object_move(p, 0, -1);
      break;

    case KEY_DOWN:
    case 'j':
    case 'J':
      object_move(p, 0, 1);
      break;

    case KEY_LEFT:
    case 'h':
    case 'H':
      object_move(p, -1, 0);
      break;

    case KEY_RIGHT:
    case 'l':
    case 'L':
      object_move(p, 1, 0);
      break;

    case 'y':
    case 'Y':
      object_move(p, -1, -1);
      break;

    case 'u':
    case 'U':
      object_move(p, 1, -1);
      break;

    case 'b':
    case 'B':
      object_move(p, -1, 1);
      break;

    case 'n':
    case 'N':
      object_move(p, 1, 1);
      break;

    default:
      break;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int i;

  WINDOW *con;

  int playerx = 10;
  int playery = 8;

  struct object player = { playerx, playery, '@', 1 };
  struct object npc = { 3, 4, '@', 2 };

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
  wbkgd(con, COLOR_PAIR(3));
  wrefresh(con);

  /* create the map */
  map_make();

  /* main game loop */
  while (1) {
    map_draw(con);

    // display player
    for (i = 0; i < object_count; i++) {
      object_draw(con, objects[i]);
    }

    /* redraw the screen */
    wrefresh(con);

    /* todo: make array forloop and clear all */
    /* for (i = 0; i < object_count; i++) */
    /*   object_clear(con, objects[i]); */

    // if handle_keys returns 1, exit
    // otherwise, move character
    if ((handle_keys(con, &player)))
      break;
  }

  endwin();
  return 0;
}
