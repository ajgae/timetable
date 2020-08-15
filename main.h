#pragma once

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
#define DAY_VIRT_HEIGHT ((DAY_END - DAY_START + HOUR) / QUARTER) /* + HOUR because inclusive */
#define DAY_PHYS_HEIGHT 35 /* physical height of day ScrollWin */
#define DAY_PHYS_WIDTH 40 /* physical width of day ScrollWin */

/* type definitions */
typedef int Minute;
typedef int Hour;

typedef struct {
    int y;
    int x;
} Cursor;

/* the container window is merely a border to the pad */
typedef struct {
    WINDOW* container;
    WINDOW* pad;
} ScrollWin;

typedef struct {
    Minute start_time;
    char* msg;
} Slot;

typedef struct {
    ScrollWin win;
    int slot_count;
    Slot* slots;
} Day;

typedef struct {
    int day_count;
    Day* days;
} Week;

/* function declarations */
void ncurses_init(void);
void loop(void);
Slot slot_create(int start_time, char const * const msg);
void slot_destroy(Slot slot);
void slot_draw(WINDOW* pad, Slot slot);
Day day_create(int slot_count, Slot* slots,
               int virt_height, /* virt_width is always phys_width-2 (for borders) */
               int phys_height, int phys_width,
               int begin_y, int begin_x);
void day_destroy(Day day);
void day_draw(Day day, int offset);
ScrollWin scrollwin_create(int virt_height, /* virt_width is always phys_width-2 (for borders) */
                           int phys_height, int phys_width,
                           int begin_y, int begin_x);
void scrollwin_destroy(ScrollWin win);
int scroll_offs(int curr, int delta, int virt_height, int phys_height);
int min_to_line(Minute m);
Hour min_to_hour(Minute m);

