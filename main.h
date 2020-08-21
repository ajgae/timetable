#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

/* MACROS */
#define DISP_ERR(e, s) {\
    int err = e;\
    if (err == ERR) { fprintf(stderr, "%d: Function returned ERR: " #e s "\n", __LINE__); }\
}

/* TODO (not urgent): makes use of a GCC extension, see statement expressions
 * -> find a better way to do it or remove it later */
#define DISP_NULPTR(e, s) ({\
    void* ptr = e;\
    if (ptr == NULL) { fprintf(stderr, "%d: Function returned NULL: " #e s "\n", __LINE__); }\
    ptr;\
})

/* DEFINES */
/* time unit is minutes
 * "physical" unit is lines */
#define HOUR 60
#define DAY 24 * HOUR
#define QUARTER (HOUR / 4)
#define DAY_PHYS_HEIGHT 37 /* physical height of day ScrollWin */
#define DAY_PHYS_WIDTH 40 /* physical width of day ScrollWin */
#define SCROLLWIN_PADDING 1

/* TYPE DEFINITIONS */
typedef int Minute;
typedef int Hour;

typedef struct {
    int y;
    int x;
} Cursor;

/* the container window is merely a border to the pad (if padding >= 1) */
typedef struct {
    WINDOW* container;
    WINDOW* pad;
    int padding;
    int offset;
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
void ncurses_init(void);
void loop(void);

Slot slot_create(int start_time, char const * const msg);
void slot_destroy(Slot slot);
void slot_draw(WINDOW* pad, Slot slot);

Day* day_create(int slot_count, Slot* slots,
                int virt_height,
                int phys_height, int phys_width,
                int begin_y, int begin_x);
Day* debug_day_create_default(int i);
void day_destroy(Day* day);
void day_draw(Day* day);

Week* debug_week_create_default();
void week_destroy(Week* week);
void week_draw(Week* week);

ScrollWin* scrollwin_create(int virt_height, int padding, /* virt_width is always phys_width - 2*padding */
                           int phys_height, int phys_width,
                           int begin_y, int begin_x);
void scrollwin_destroy(ScrollWin* win);
void scrollwin_draw(ScrollWin* win);
void scrollwin_clear_inner(ScrollWin* win);
void scrollwin_scroll(ScrollWin* win, int delta);
int scrollwin_get_begin_y(ScrollWin* win);
int scrollwin_get_begin_x(ScrollWin* win);
int scrollwin_get_phys_height(ScrollWin* win);
int scrollwin_get_phys_width(ScrollWin* win);
int scrollwin_get_virt_height(ScrollWin* win);
int scrollwin_get_virt_width(ScrollWin* win);
int scrollwin_get_padding(ScrollWin* win);

int min_to_line(Minute m);
Hour min_to_hour(Minute m);
int compare_int(const void* a, const void* b);
