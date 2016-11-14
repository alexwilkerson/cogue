#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define true  1
#define false 0
#define boolean char

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 40

#define MAP_WIDTH  80
#define MAP_HEIGHT 25

#define ROOM_MAX_SIZE 10
#define ROOM_MIN_SIZE 6
#define MAX_ROOMS     30

typedef struct object {
  int x;
  int y;
  char ch;
  int color;
} object;

object *new_object()
{
  return (object *)malloc(sizeof(object));
}

typedef struct tile {
  char ch;
  short blocked;
  short block_sight;
} tile;
/* global map */
tile map[MAP_WIDTH][MAP_HEIGHT];

typedef struct rect {
  int x1;
  int y1;
  int x2;
  int y2;
  int centerx;
  int centery;
} rect;

rect create_rect(int x, int y, int w, int h)
{
  rect r = { x, y, x + w, y + h, (x + x + w) / 2, (y + y + h) / 2 };
  return r;
}

rect *new_rect()
{
  return (rect *)malloc(sizeof(rect));
}

void create_room(rect room)
{
  int x, y;
  for (x = room.x1; x < room.x2; x++) {
    for (y = room.y1; y < room.y2; y++) {
      map[x][y].blocked = 0;
      map[x][y].block_sight = 0;
    }
  }
}

boolean intersect(rect room1, rect room2) {
  return (room1.x1 <= room2.x2 && \
          room1.x2 >= room2.x1 && \
          room1.y1 <= room2.y2 && \
          room1.y2 >= room2.y1);
}

void create_h_tunnel(int x1, int x2, int y)
{
  int x;
  for (x = min(x1, x2); x < max(x1, x2) + 1; x++) {
    map[x][y].blocked = false;
    map[x][y].block_sight = false;
  }
}

void create_v_tunnel(int y1, int y2, int x)
{
  int y;
  for (y = min(y1, y2); y < max(y1, y2) + 1; y++) {
    map[x][y].blocked = false;
    map[x][y].block_sight = false;
  }
}

void map_make(object *player)
{
  int x, y;

  tile base_tile = { '#', 1, 1 };

  for (x = 0; x < MAP_WIDTH; x++) {
    for (y = 0; y < MAP_HEIGHT; y++) {
      map[x][y] = base_tile;
    }
  }

  rect rooms[MAX_ROOMS];
  int i, j;
  int roomx, roomy, roomw, roomh;
  int prev_centerx, prev_centery;
  int num_rooms = 0;
  rect room;

  for (i = 0; i < MAX_ROOMS; i++) {
    roomx = rand() % (MAP_WIDTH - ROOM_MAX_SIZE) + 1;
    roomy = rand() % (MAP_HEIGHT - ROOM_MAX_SIZE) + 1;
    roomw = (rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE)) + ROOM_MIN_SIZE;
    roomh = (rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE)) + ROOM_MIN_SIZE;
    room = create_rect(roomx, roomy, roomw, roomh);

    boolean failed = false;
    for (j = 0; j < num_rooms; j++) {
      if (intersect(room, rooms[j])) {
        failed = true;
        break;
      }
    }

    if (failed == false) {
      // no intersections, so this room is valid

      create_room(room);

      if (num_rooms == 0) {
        player->x = room.centerx;
        player->y = room.centery;
      } else {
        // all rooms after the first
        // connect it to the previous room with a tunnel

        // center coordinates of previous room
        prev_centerx = rooms[num_rooms - 1].centerx;
        prev_centery = rooms[num_rooms - 1].centery;

        if ((rand() % 2) == 1) {
          create_h_tunnel(prev_centerx, room.centerx, prev_centery);
          create_v_tunnel(prev_centery, room.centery, room.centerx);
        } else {
          create_v_tunnel(prev_centery, room.centery, prev_centerx);
          create_h_tunnel(prev_centerx, room.centerx, room.centery);
        }
      }
      rooms[num_rooms] = room;
      num_rooms++;
    }
  }
}

void map_draw(WINDOW *win)
{
  int x, y;
  int wall;

  for (x = 0; x < MAP_WIDTH; x++) {
    for (y = 0; y < MAP_HEIGHT; y++) {
      wall = map[x][y].blocked;
      if (wall)
        mvwaddch(win, y, x, '#');
      else
        mvwaddch(win, y, x, '.');
    }
  }
}

void object_move(object *o, int x, int y) {
  if (((o->x + x) >= 0)           && \
      ((o->x + x) < MAP_WIDTH)    && \
      ((o->y + y) >= 0)           && \
      ((o->y + y) < MAP_HEIGHT)   && \
      (!map[o->x + x][o->y + y].blocked)) {
    o->x += x;
    o->y += y;
  }
}

void object_draw(WINDOW *win, object *o) {
  wattron(win, COLOR_PAIR(o->color));
  mvwaddch(win, o->y, o->x, o->ch);
  wattroff(win, COLOR_PAIR(o->color));
}

void object_clear(WINDOW *win, object *o) {
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

int handle_keys(WINDOW *con, object *p)
{
  switch (getch()) {
    case 'q':
    case 'Q':
      return 1;

    case 'r':
    case 'R':
      map_make(p);
      break;

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
  srand(time(NULL));
  int i;

  WINDOW *con;

  int playerx = 10;
  int playery = 8;

  object player = { playerx, playery, '@', 1 };
  /* object npc = { 3, 4, '@', 2 }; */

  object *objects[] = {/* &npc, */&player};
  int object_count = sizeof(objects) / sizeof(object *);

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
  map_make(&player);

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
