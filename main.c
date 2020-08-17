#include "main.h"

int main(int argc, char const * argv[]) 
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
    Day day = debug_day_create_default();

    int offset = 0;

    char c = 0;
    do {
        day_draw(day, offset);
        DISP_ERR(refresh(),);

        c = getch();

        switch (c) {
            case 'J':
                offset = scrollwin_scroll(day.win, offset, +1);
                break;
            case 'K':
                offset = scrollwin_scroll(day.win, offset, -1);
                break;
        }
    } while(c != 'q');

    day_destroy(day);
}

Slot slot_create(Minute start_time, char const * const msg)
{
    int len = strlen(msg);
    char* buf = DISP_NULPTR(calloc(len + 1, sizeof (char)),);
    strcpy(buf, msg);
    Slot slot = { .start_time = start_time, .msg = buf };
    return slot;
}

void slot_destroy(Slot slot) 
{
    if (slot.msg != NULL) free(slot.msg);
}

void slot_draw(WINDOW* pad, Slot slot)
{
    int start = min_to_line(slot.start_time);
    DISP_ERR(wmove(pad, start, 0),);
    DISP_ERR(wprintw(pad, "%02dh%02d\n%s\n",
             min_to_hour(slot.start_time), slot.start_time % HOUR, slot.msg),);
}

Day day_create(int slot_count, Slot* slots,
               int virt_height,
               int phys_height, int phys_width,
               int begin_y, int begin_x)
{
    ScrollWin win =
        scrollwin_create(virt_height, SCROLLWIN_PADDING,
                         phys_height, phys_width,
                         begin_y, begin_x);
    Day day = { .win = win, .slot_count = slot_count, .slots = slots };
    return day;
}

Day debug_day_create_default(void) {
    Slot* slots = DISP_NULPTR(calloc(5, sizeof (Slot)),);
    slots[0] = slot_create(0*HOUR, "WAKEUP");
    slots[1] = slot_create(6*HOUR + QUARTER, "BREAKFAST");
    slots[2] = slot_create(12*HOUR + 2*QUARTER, "LUNCH");
    slots[3] = slot_create(18*HOUR, "SLEEP");
    slots[4] = slot_create(23*HOUR + 3*QUARTER, "NIGHT");
    Day day = day_create(5, slots, DAY_VIRT_HEIGHT, DAY_PHYS_HEIGHT, DAY_PHYS_WIDTH, 0, 0);
    return day;
}

void day_destroy(Day day) 
{
    scrollwin_destroy(day.win);
    if (day.slots != NULL) {
        for (int i = 0; i < day.slot_count; ++i) {
            slot_destroy(day.slots[i]);
        }
        free(day.slots);
    }
}

void day_draw(Day day, int offset)
{
    /* TODO optimize when we change info, how we refresh etc.
     * instead of doing it every time */
    /* fill the pad with the right info */
    scrollwin_clear_inner(day.win);
    for (int i = 0; i < day.slot_count; ++i) {
        slot_draw(day.win.pad, day.slots[i]);
    }

    scrollwin_draw(day.win, offset);
}

ScrollWin scrollwin_create(int virt_height, int padding,
                           int phys_height, int phys_width,
                           int begin_y, int begin_x)
{
    if (virt_height < 0 || padding < 0
    || phys_height < 0 || phys_width < 0
    || begin_y < 0 || begin_x < 0)
    {
        ScrollWin win = { .container = NULL, .pad = NULL, .padding = 0 };
        return win;
    }

    WINDOW* container = DISP_NULPTR(newwin(phys_height, phys_width, begin_y, begin_x),);
    WINDOW* pad = DISP_NULPTR(newpad(virt_height, phys_width - 2*padding),);
    ScrollWin win = { .container = container, .pad = pad, .padding = padding };
    return win;
}

void scrollwin_destroy(ScrollWin win) {
    if (win.container != NULL) delwin(win.container);
    if (win.pad != NULL) delwin(win.pad);
}

void scrollwin_draw(ScrollWin win, int offset) {
    DISP_ERR(box(win.container, 0, 0),);
    DISP_ERR(wrefresh(win.container),);
    DISP_ERR(prefresh(win.pad, offset, 0, 
                      scrollwin_get_begin_y(win) + win.padding,
                      scrollwin_get_begin_x(win) + win.padding,
                      scrollwin_get_phys_height(win) - 2*win.padding,
                      scrollwin_get_phys_width(win) - 2*win.padding)
            ,);
}

void scrollwin_clear_inner(ScrollWin win) {
    wclear(win.pad);
}

int scrollwin_scroll(ScrollWin win, int curr_offs, int delta)
{
    int result = curr_offs + delta;
    int scroll_max =
        scrollwin_get_virt_height(win) - scrollwin_get_phys_height(win) + 2*win.padding;
    if (result < 0) {
        return 0;
    } else if (result >= scroll_max) {
        return scroll_max;
    }
    return result;
}

int scrollwin_get_begin_y(ScrollWin win) {
    return getbegy(win.container);
}

int scrollwin_get_begin_x(ScrollWin win) {
    return getbegx(win.container);
}

int scrollwin_get_phys_height(ScrollWin win) {
    return getmaxy(win.container);
}

int scrollwin_get_phys_width(ScrollWin win) {
    return getmaxx(win.container);
}

int scrollwin_get_virt_height(ScrollWin win) {
    return getmaxy(win.pad);
}

int scrollwin_get_virt_width(ScrollWin win) {
    return getmaxx(win.pad);
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
