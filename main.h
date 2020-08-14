#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

/* macros */
#define DISP_ERR(e, s) {\
    int err = e;\
    if (err == ERR) { fprintf(stderr, "Function returned ERR: " #e s "\n"); }\
}

/* defines */
#define HOUR 60
#define QUARTER (HOUR / 4)
#define DAY_START (0 * HOUR)
#define DAY_END (23 * HOUR)
#define DAY_LEN (DAY_END - DAY_START + HOUR) /* + HOUR because inclusive */
#define DAY_HEIGHT (2 + DAY_LEN / QUARTER)
#define DAY_WIDTH 40

#define BORDER_WIDTH 1

/* type definitions */
typedef struct Cursor_s {
    int y;
    int x;
} Cursor;

typedef struct Cell_s {
    char* msg;
} Cell;

typedef struct Day_s {
    WINDOW* win;
    int slot_count;
    int* slots;
    Cell* cells;
} Day;

/* function declarations */
void ncurses_init(void);
void loop(void);
void cursor_move_to(Cursor const cursor);
Day day_create_empty(void);
void day_destroy(Day day);
Cell cell_create(char const * const msg);
void cell_destroy(Cell cell);
void day_draw(Day day, int offset);
void cell_draw(WINDOW* win, Cell cell, int start, int offset);
