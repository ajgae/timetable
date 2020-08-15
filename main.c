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
    Slot* slots = calloc(5, sizeof (Slot));
    slots[0] = slot_create(0*HOUR, "WAKEUP");
    slots[1] = slot_create(6*HOUR + QUARTER, "BREAKFAST");
    slots[2] = slot_create(12*HOUR + 2*QUARTER, "LUNCH");
    slots[3] = slot_create(18*HOUR, "SLEEP");
    slots[4] = slot_create(23*HOUR + 3*QUARTER, "NIGHT");
    Day day = day_create(5, slots, DAY_VIRT_HEIGHT, DAY_PHYS_HEIGHT, DAY_PHYS_WIDTH, 0, 0);

    int offset = 0;

    char c = 0;
    do {
        day_draw(day, offset);
        DISP_ERR(refresh(),);

        c = getch();

        switch (c) {
            case 'J':
                offset = scroll_offs(offset, +1, DAY_VIRT_HEIGHT, DAY_PHYS_HEIGHT);
                break;
            case 'K':
                offset = scroll_offs(offset, -1, DAY_VIRT_HEIGHT, DAY_PHYS_HEIGHT);
                break;
        }
    } while(c != 'q');

    day_destroy(day);
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
    if (slot.msg != NULL) free(slot.msg);
}

void slot_draw(WINDOW* pad, Slot slot)
{
    int start = min_to_line(slot.start_time);
    DISP_ERR(wmove(pad, start, 0),);
    DISP_ERR(wprintw(pad, " %02dh%02d\n %s\n",
             min_to_hour(slot.start_time), slot.start_time % HOUR, slot.msg),);
}

Day day_create(int slot_count, Slot* slots,
               int virt_height,
               int phys_height, int phys_width,
               int begin_y, int begin_x)
{
    ScrollWin win =
        scrollwin_create(virt_height,
                         phys_height, phys_width,
                         begin_y, begin_x);
    Day day = { .win = win, .slot_count = slot_count, .slots = slots };
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

/* TODO we have -2 for borders again in this function, annoying and bad, pls fix
 * and also the +1s */
void day_draw(Day day, int offset)
{
    wclear(day.win.pad);
    int cy = 0;
    int cx = 0;
    getbegyx(day.win.container, cy, cx); /* store container coords */
    int cheight = 0;
    int cwidth = 0;
    getmaxyx(day.win.container, cheight, cwidth); /* store container size */

    for (int i = 0; i < day.slot_count; ++i) {
        int start_line = min_to_line(day.slots[i].start_time);
        if (start_line >= offset && start_line <= offset + cheight-2) {
            slot_draw(day.win.pad, day.slots[i]);
        }
    }
    DISP_ERR(box(day.win.container, 0, 0),);
    DISP_ERR(wrefresh(day.win.container),);
    DISP_ERR(prefresh(day.win.pad, offset, 0, cy+1, cx+1, cheight-2, cwidth-2),);
}

ScrollWin scrollwin_create(int virt_height,
                           int phys_height, int phys_width,
                           int begin_y, int begin_x)
{
    WINDOW* container = newwin(phys_height, phys_width, begin_y, begin_x);
    WINDOW* pad = newpad(virt_height, phys_width - 2);
    ScrollWin win = { .container = container, .pad = pad };
    return win;
}

void scrollwin_destroy(ScrollWin win) {
    if (win.container != NULL) delwin(win.container);
    if (win.pad != NULL) delwin(win.pad);
}

int scroll_offs(int curr, int delta, int virt_height, int phys_height)
{
    int result = curr + delta;
    if (result < 0) {
        return 0;
    }
    else if (result >= virt_height - phys_height) {
        return virt_height - phys_height;
    }
    return result;
}

int min_to_line(Minute m)
{
    return m / QUARTER;
}

Hour min_to_hour(Minute m)
{
    return m / HOUR;
}
