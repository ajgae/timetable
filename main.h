#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define DISP_ERR(e, s) {\
    int err = e;\
    if (err == ERR) { fprintf(stderr, "Function returned ERR: " #e s "\n"); }\
}

#define HOUR 60
#define QUARTER (HOUR / 4)
#define DAY_START (0 * HOUR)
#define DAY_END (23 * HOUR)
#define DAY_LEN (DAY_END - DAY_START + HOUR) /* + HOUR because inclusive */
#define DAY_HEIGHT (2 + DAY_LEN / QUARTER)
#define DAY_WIDTH 20

#define BORDER_WIDTH 1

typedef struct Cursor_s {
    int y;
    int x;
} Cursor;

typedef struct Cell_s {
    char* msg;
} Cell;

typedef struct Day_s {
    WINDOW* win;
    int count;
    int* juncs;
    Cell* cells;
} Day;

void init_ncurses(void);
void loop(void);
void cursor_move_to(Cursor const cursor);
Day create_day_empty(void);
void destroy_day(Day day);
Cell create_cell(char const * const msg);
void destroy_cell(Cell cell);
void draw_day(Day day, int offset);
void draw_cell(WINDOW* win, Cell cell, int start, int offset);
