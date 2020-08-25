#include "main.h"

int main(void) 
{
    ncurses_init();
    loop();
    DISP_ERR(endwin(),);

    return 0;
}

void ncurses_init(void) 
{
    initscr();
    DISP_ERR(cbreak(),);
    DISP_ERR(noecho(),);
    DISP_ERR(refresh(),);
}

void loop(void) 
{
    Week week = debug_week_create_default();

    // BREAKPOINT HERE
    char c = 0;
    do {
        week_draw(week);
        DISP_ERR(refresh(),);

        c = getch();

        switch (c) {
        case 'J':
            for (int i = 0; i < week.day_count; ++i) {
                scrollwin_scroll(week.days[i].win, +1);
            }
            break;
        case 'K':
            for (int i = 0; i < week.day_count; ++i) {
                scrollwin_scroll(week.days[i].win, -1);
            }
            break;
        }
    } while(c != 'q');

    week_destroy(week);
}

Slot slot_create(Minute start_time, char const * const msg)
{
    int len = strlen(msg);
    char* buf = calloc(len + 1, sizeof (char));
    strcpy(buf, msg);
    Slot slot = { .start_time = start_time, .msg = buf };
    return slot;
}

void slot_destroy(Slot slot) 
{
    if (slot.msg != NULL) {
        free(slot.msg);
        slot.msg = NULL;
    }
}

void slot_draw(WINDOW* pad, Slot slot)
{
    int start = min_to_line(slot.start_time);
    DISP_ERR(wmove(pad, start, 0),);
    /* TODO crop header text (hour | event name) to fit within pad */
    DISP_ERR(wprintw(pad, "%02dh%02d | %s",
             min_to_hour(slot.start_time), slot.start_time % HOUR, slot.msg),);
}

Day day_create(int slot_count, Slot* slots,
               int virt_height,
               int phys_height, int phys_width,
               int begin_y, int begin_x)
{
    ScrollWin* win =
        scrollwin_create(virt_height, SCROLLWIN_PADDING,
                         phys_height, phys_width,
                         begin_y, begin_x);
    Day day = { .win = win, .slot_count = slot_count, .slots = slots };
    return day;
}

Day debug_day_create_default(int h_index) {
    Slot* slots = calloc(5, sizeof (Slot));
    slots[0] = slot_create(0*HOUR, "WAKEUP");
    slots[1] = slot_create(6*HOUR + QUARTER, "BREAKFAST");
    slots[2] = slot_create(12*HOUR + 2*QUARTER, "LUNCH");
    slots[3] = slot_create(18*HOUR, "SLEEP");
    slots[4] = slot_create(23*HOUR + 3*QUARTER, "NIGHT");
    Day day = day_create(5, slots, DAY_VIRT_HEIGHT,
            DAY_PHYS_HEIGHT, DAY_PHYS_WIDTH,
            0, h_index*(DAY_PHYS_WIDTH+1));
    return day;
}

void day_destroy(Day day) 
{
    scrollwin_destroy(day.win);
    day.win = NULL;
    if (day.slots != NULL) {
        for (int i = 0; i < day.slot_count; ++i) {
            slot_destroy(day.slots[i]);
        }
        free(day.slots);
        day.slots = NULL;
    }
}

void day_draw(Day day)
{
    /* TODO optimize when we change info, how we refresh etc.
     * instead of doing it every time */
    /* fill the pad with the right info */
    scrollwin_clear_inner(day.win);
    for (int i = 0; i < day.slot_count; ++i) {
        slot_draw(day.win->pad, day.slots[i]);
    }

    scrollwin_draw(day.win);
}

Week debug_week_create_default(void) {
    int day_count = 1;
    //Day days[day_count]; // <-- this is on the stack
    Day* days = calloc(day_count, sizeof (Day));
    for (int i = 0; i < day_count; ++i) {
        days[i] = debug_day_create_default(i);
    }
    Week week = { .day_count = day_count, .days = days };
    return week;
}

void week_destroy(Week week) {
    if (week.days != NULL && week.day_count != 0) {
        for (int i = 0; i < week.day_count; ++i) {
            day_destroy(week.days[i]);
        }
        free(week.days);
        week.days = NULL;
    }
}

void week_draw(Week week) {
    for (int i = 0; i < week.day_count; ++i) {
        day_draw(week.days[i]); 
    }
}

ScrollWin* scrollwin_create(int virt_height, int padding,
                            int phys_height, int phys_width,
                            int begin_y, int begin_x)
{
    if (virt_height < 0 || padding < 0
    || phys_height < 0 || phys_width < 0
    || begin_y < 0 || begin_x < 0
    || phys_height - 2*padding < 0)
    {
        //ScrollWin win = { .container = NULL, .pad = NULL, .padding = 0, .offset = 0 };
        //return win;
        return NULL;
    }

    WINDOW* container = newwin(phys_height, phys_width, begin_y, begin_x);
    WINDOW* pad = newpad(virt_height, phys_width - 2*padding);
    ScrollWin* win = malloc(sizeof (ScrollWin));
    win->container = container;
    win->pad = pad;
    win->padding = padding;
    win->offset = 0;
    return win;
}

void scrollwin_destroy(ScrollWin* win) {
    if (win != NULL) {
        if (win->container != NULL) {
            delwin(win->container);
            win->container = NULL;
        }
        if (win->pad != NULL) {
            delwin(win->pad);
            win->pad = NULL;
        }
        free(win);
        win = NULL;
    }
}

void scrollwin_draw(ScrollWin* win) {
    DISP_ERR(box(win->container, 0, 0),);
    DISP_ERR(wrefresh(win->container),);
    DISP_ERR(prefresh(win->pad, win->offset, 0, 
                      scrollwin_get_begin_y(win) + win->padding,
                      scrollwin_get_begin_x(win) + win->padding,
                      scrollwin_get_phys_height(win) - 2*win->padding,
                      scrollwin_get_phys_width(win) - 2*win->padding)
            ,);
}

void scrollwin_clear_inner(ScrollWin* win) {
    wclear(win->pad);
}

void scrollwin_scroll(ScrollWin* win, int delta)
{
    int offset_new = win->offset + delta;
    int scroll_max =
        scrollwin_get_virt_height(win) - scrollwin_get_phys_height(win) + 2*win->padding;
    if (offset_new < 0) {
        win->offset = 0;
    } else if (offset_new >= scroll_max) {
        win->offset = scroll_max;
    } else {
        win->offset = offset_new;
    }
}

int scrollwin_get_begin_y(ScrollWin* win) {
    return getbegy(win->container);
}

int scrollwin_get_begin_x(ScrollWin* win) {
    return getbegx(win->container);
}

int scrollwin_get_phys_height(ScrollWin* win) {
    return getmaxy(win->container);
}

int scrollwin_get_phys_width(ScrollWin* win) {
    return getmaxx(win->container);
}

int scrollwin_get_virt_height(ScrollWin* win) {
    return getmaxy(win->pad);
}

int scrollwin_get_virt_width(ScrollWin* win) {
    return getmaxx(win->pad);
}

/* TODO make this customizable ? like 1 line can be 10, 15, or XX minutes */
int min_to_line(Minute m)
{
    return m / QUARTER;
}

Hour min_to_hour(Minute m)
{
    return m / HOUR;
}
