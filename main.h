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
#define DAY_LEN (DAY_END - DAY_START + HOUR) /* + HOUR because inclusive */
#define DAY_HEIGHT 55
#define DAY_WIDTH 40

/* type definitions */
typedef int Minute;
typedef int Line;

typedef struct {
    int y;
    int x;
} Cursor;

typedef struct {
    Minute start_time;
    char* msg;
} Slot;

typedef struct {
    WINDOW* pad;
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
void slot_draw(WINDOW* pad, Slot slot, int offset);
void day_destroy(Day day);
void day_draw(Day day, int offset);
Day day_create(int slot_count, Slot* slots);
Line scroll_offs(int curr, int delta, int mod);
Line min_to_lin(Minute m);

