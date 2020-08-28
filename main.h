#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

/* MACROS */
#define DISP_ERR(e, s) {\
    int err = e;\
    if (err == ERR) { fprintf(stderr, "%d: ERROR: Function returned ERR: " #e s "\n", __LINE__); }\
}

/* DEFINES */
#define HOUR 60
#define QUARTER (HOUR / 4)
#define DAY_START (0 * HOUR)
#define DAY_END (23 * HOUR)
#define DAY_VIRT_HEIGHT ((DAY_END - DAY_START + HOUR) / QUARTER) /* + HOUR because inclusive */
#define DAY_PHYS_HEIGHT 35 /* physical height of day ScrollWin */
#define DAY_PHYS_WIDTH 40 /* physical width of day ScrollWin */
#define SCROLLWIN_PADDING 1
/* ncurses color pairs */
#define PAIR_SLOT_HEADER 1

/* TYPE DEFINITIONS */
typedef int Minute;
typedef int Hour;

typedef struct {
    int y;
    int x;
} Cursor;

/* TODO (after previous): include offset directly in ScrollWin struct 
 * (or should we ? since that would make the offset not general for a week, 
 * ==> how to deal with different day lengths in a week ? just ban it ? seems easiest
 * and most sensical) */
/* the container window is merely a border to the pad (if padding >= 1) */
typedef struct {
    WINDOW* container;
    WINDOW* pad;
    int offset;
    int container_dirty;
} ScrollWin;

typedef struct {
    Minute start_time;
    char* msg;
} Slot;

typedef struct {
    ScrollWin* win;
    int slot_count;
    Slot* slots;
} Day;

typedef struct {
    int day_count;
    Day* days;
} Week;

/* FUNCTION DECLARATIONS */
/* general */
void ncurses_init(void);
void loop(void);
/* slots */
Slot slot_create(int start_time, char const * const msg);
void slot_destroy(Slot slot);
void slot_draw(ScrollWin* win, Slot slot);
/* days */
Day day_create(int slot_count, Slot* slots, int virt_height,
               int day_count, int index);
Day debug_day_create_default(int day_count, int index);
void day_destroy(Day day);
void day_draw(Day day);
/* weeks */
Week debug_week_create_default(void);
void week_destroy(Week week);
void week_draw(Week week);
/* scrollwins */
ScrollWin* scrollwin_create(int virt_height, int phys_height, int phys_width,
                           int begin_y, int begin_x);
void scrollwin_destroy(ScrollWin* win);
void scrollwin_draw(ScrollWin* win);
void scrollwin_clear_inner(ScrollWin* win);
void scrollwin_draw_slot_header(ScrollWin* win, Slot slot);
char* scrollwin_format_slot_header(ScrollWin* win, Slot slot);
void scrollwin_scroll(ScrollWin* win, int delta);
/* scrollwin gets */
int scrollwin_get_begin_y(ScrollWin* win);
int scrollwin_get_begin_x(ScrollWin* win);
int scrollwin_get_phys_height(ScrollWin* win);
int scrollwin_get_phys_width(ScrollWin* win);
int scrollwin_get_virt_height(ScrollWin* win);
int scrollwin_get_virt_width(ScrollWin* win);
/* utility */
int min_to_line(Minute m);
Hour min_to_hour(Minute m);

