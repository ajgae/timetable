#include "main.h"
#include "debug.h"

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

    /* initialize color stuff (apparently it is good practice
     * to do this right after initscr(), see start_color(3X)) */
    if (!has_colors()) {
        fprintf(stderr, "ERROR: the terminal does not support colors\n");
        exit(1);
    } else {
        start_color();
        use_default_colors();
        
        if (can_change_color() && COLORS >= 16) {
            init_color(COLOR_WHITE_BRIGHT, 1000, 1000, 1000);
        }
        
        if (COLORS >= 16) {
            init_pair(PAIR_SLOT_HEADER, COLOR_WHITE_BRIGHT, COLOR_BLUE);
            init_pair(PAIR_SLOT_BG, COLOR_BLACK, COLOR_WHITE_BRIGHT);
        } else {
            init_pair(PAIR_SLOT_HEADER, COLOR_WHITE, COLOR_BLUE);
            init_pair(PAIR_SLOT_BG, COLOR_BLACK, COLOR_WHITE);
        }
    }

    /* other settings */
    DISP_ERR(cbreak(),);
    DISP_ERR(noecho(),);

    /* initial refresh */
    DISP_ERR(refresh(),);
}

void loop(void) 
{
    Week week = debug_week_create_true();

    char c = 0;
    do {
        week_draw(week);

        c = getch();

        switch (c) {
        case 'j':
            for (int i = 0; i < week.day_count; ++i) {
                scrollwin_scroll(week.days[i].win, +1);
            }
            break;
        case 'k':
            for (int i = 0; i < week.day_count; ++i) {
                scrollwin_scroll(week.days[i].win, -1);
            }
            break;
        }
    } while(c != 'q');

    week_destroy(week);
}

Slot slot_create(Minute start_time, Minute duration, char const * const msg)
{
    if (duration < 0) {
        duration = 0;
    }
    int len = strlen(msg);
    char* buf = calloc(len + 1, sizeof (char));
    strcpy(buf, msg);
    Slot slot = { .start_time = start_time, .duration = duration, .msg = buf };
    return slot;
}

void slot_destroy(Slot slot) 
{
    if (slot.msg != NULL) {
        free(slot.msg);
        slot.msg = NULL;
    }
}

void slot_draw(ScrollWin* win, Slot slot)
{
    scrollwin_draw_slot_bg(win, slot);
    scrollwin_draw_slot_header(win, slot);
}

Day day_create(int slot_count, Slot* slots, int virt_height,
               int day_count, int index)
{
    /* scale day height to terminal height, or to the maximum needed by the contents of day
     * if that is less than the terminal height */
    int scr_size_y, scr_size_x;
    getmaxyx(stdscr, scr_size_y, scr_size_x);
    scr_size_y = (scr_size_y > DAY_VIRT_HEIGHT+2*SCROLLWIN_PADDING ?
                  DAY_VIRT_HEIGHT+2*SCROLLWIN_PADDING :
                  scr_size_y);
    ScrollWin* win =
        scrollwin_create(virt_height, scr_size_y, scr_size_x/day_count,
                         0, index*(scr_size_x/day_count));
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
        day.slots = NULL;
    }
}

void day_draw(Day day)
{
    /* TODO understand why we dont need to clear ? */
    //scrollwin_clear_inner(day.win);

    /* fill the pad with the right info */
    for (int i = 0; i < day.slot_count; ++i) {
        slot_draw(day.win, day.slots[i]);
    }

    scrollwin_draw(day.win);
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

ScrollWin* scrollwin_create(int virt_height, int phys_height, int phys_width,
                            int begin_y, int begin_x)
{
    if (virt_height < 0 || phys_height < 0 || phys_width < 0
    || begin_y < 0 || begin_x < 0)
    {
        return NULL;
    }

    WINDOW* container = newwin(phys_height, phys_width, begin_y, begin_x);
    WINDOW* pad = newpad(virt_height, phys_width - 2*SCROLLWIN_PADDING);
    ScrollWin* win = malloc(sizeof (ScrollWin));
    win->container = container;
    win->pad = pad;
    win->offset = 0;
    win->container_dirty = 1;
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
    if (win->container_dirty) {
        /* box with default borders */
        DISP_ERR(box(win->container, 0, 0),);
        DISP_ERR(wrefresh(win->container),);
        win->container_dirty = 0;
    }
    int phys_begin_y = scrollwin_get_begin_y(win) + SCROLLWIN_PADDING;
    int phys_begin_x = scrollwin_get_begin_x(win) + SCROLLWIN_PADDING;
    DISP_ERR(prefresh(win->pad, win->offset, 0, 
                      phys_begin_y,
                      phys_begin_x,
                      /* TODO see if there isnt a better fix than this ugly -1 */
                      phys_begin_y + scrollwin_get_phys_height(win) - 2*SCROLLWIN_PADDING - 1,
                      phys_begin_x + scrollwin_get_phys_width(win) - 2*SCROLLWIN_PADDING)
            ,);
}

void scrollwin_draw_slot_header(ScrollWin* win, Slot slot) {
    char* header_text = scrollwin_format_slot_header(win, slot);

    /* draw line with color */
    DISP_ERR(wmove(win->pad, min_to_line(slot.start_time), 0),);
    DISP_ERR(wattron(win->pad, COLOR_PAIR(PAIR_SLOT_HEADER)),);
    DISP_ERR(wprintw(win->pad, header_text),);
    DISP_ERR(wattroff(win->pad, COLOR_PAIR(PAIR_SLOT_HEADER)),);

    /* free allocated memory */
    free(header_text);
}

void scrollwin_draw_slot_bg(ScrollWin* win, Slot slot) {
    /* TODO find a better way to fill in the bg (maybe?) */
    /* wprintw newlines after it hits the pad border,
     * we can use that to fill in the necessary lines */
    int width = scrollwin_get_virt_width(win);
    int n_lines = min_to_line(slot.duration);
    char* filler = calloc(width * n_lines + 1, sizeof (char));
    for (int i = 0; i < width * n_lines; ++i) {
        filler[i] = ' ';
    }
    
    /* fill in the bg */
    DISP_ERR(wmove(win->pad, min_to_line(slot.start_time), 0),);
    DISP_ERR(wattron(win->pad, COLOR_PAIR(PAIR_SLOT_BG)),);
    DISP_ERR(wprintw(win->pad, filler),);
    DISP_ERR(wattroff(win->pad, COLOR_PAIR(PAIR_SLOT_BG)),);

    /* free allocated memory */
    free(filler);
}

/* TODO(?) end formatted header text with "..." */
char* scrollwin_format_slot_header(ScrollWin* win, Slot slot) {
    /* dont forget +1, account for terminating null byte ! */
    char* result = calloc(scrollwin_get_virt_width(win) + 1, sizeof (char));
    /* right pad so that header bg color fills window width */
    snprintf(result, scrollwin_get_virt_width(win) + 1, "%02dh%02d | %-*s",
             min_to_hour(slot.start_time), slot.start_time % HOUR,
             scrollwin_get_virt_width(win), slot.msg);
    return result;
}

void scrollwin_clear_inner(ScrollWin* win) {
    werase(win->pad);
}

void scrollwin_scroll(ScrollWin* win, int delta)
{
    int offset_new = win->offset + delta;
    int scroll_max =
        scrollwin_get_virt_height(win) - scrollwin_get_phys_height(win) + 2*SCROLLWIN_PADDING;
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

int min_to_line(Minute m)
{
    return m / LINESTEP;
}

Hour min_to_hour(Minute m)
{
    return m / HOUR;
}
