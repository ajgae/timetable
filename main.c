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
    Day day = day_create(5, slots);

    Line offset = 0;

    char c = 0;
    do {
        day_draw(day, offset);
        DISP_ERR(refresh(),);

        c = getch();

        switch (c) {
            case 'J':
                offset = scroll_offs(offset, 1, DAY_HEIGHT);
                break;
            case 'K':
                offset = scroll_offs(offset, -1, DAY_HEIGHT);
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

void slot_draw(WINDOW* win, Slot slot, Line offset) {
    int start_phys = min_to_lin(slot.start_time) - offset;
    if (start_phys < 0 || start_phys >= DAY_HEIGHT)
        return;
    DISP_ERR(wmove(win, start_phys, 0),);
    DISP_ERR(wprintw(win, "%02dh%02d\n%s\n",
             slot.start_time / HOUR, slot.start_time % HOUR, slot.msg),);
}

Day day_create(int slot_count, Slot* slots, virt_height, virt_width) {
    WINDOW* pad = newpad(virt_height, virt_width);
    Day day = { .pad = pad, .slot_count = slot_count, .slots = slots };
    return day;
}

void day_destroy(Day day) 
{
    if (day.pad != NULL) delwin(day.pad);
    if (day.slots != NULL) {
        for (int i = 0; i < day.slot_count; ++i) {
            slot_destroy(day.slots[i]);
        }
        free(day.slots);
    }
}

void day_draw(Day day, Line offset) {
    wclear(day.pad);
    for (int i = 0; i < day.slot_count; ++i) {
        slot_draw(day.pad, day.slots[i], offset);
    }
    prefresh(day.pad);
}

Line scroll_offs(int curr, int delta, int mod) {
    int result = (curr + delta) % mod;
    if (result < 0)
        return mod - DAY_HEIGHT;
    return result;
}

Line min_to_lin(Minute m)
{
    return m / QUARTER;
}
