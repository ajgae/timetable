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
    Day day = day_create_empty();

    char c = 0;
    do {
        day_draw(day, DAY_START);
        DISP_ERR(refresh(),);

        c = getch();
        /* input bullshit */
        /* ... */

    } while(c != 'q');

    day_destroy(day);
}

void cursor_move_to(Cursor const cursor) 
{
    DISP_ERR(move(cursor.y, cursor.x),)
}

Day day_create_empty(void)
{
    Cell* cells = malloc(sizeof (Cell));
    *cells = cell_create("<empty>");
    int* slot = malloc(sizeof (int));
    *slot = DAY_START;

    WINDOW* window = newwin(DAY_HEIGHT, DAY_WIDTH, 0, 0);
    Day day = { .win = window, .slot_count = 1, .slots = slot, .cells = cells };
    return day;
}

void day_destroy(Day day) 
{
    if (day.win != NULL) delwin(day.win);
    if (day.slots != NULL) free(day.slots);
    if (day.cells != NULL) {
        for (int i = 0; i < day.slot_count; ++i) {
            cell_destroy(day.cells[i]);
        }
        free(day.cells);
    }
}

void day_draw(Day day, int offset) {
    for (int i = 0; i < day.slot_count; ++i) {
        cell_draw(day.win, day.cells[i], day.slots[i], offset);
    }
    box(day.win, 0, 0);
    wrefresh(day.win);
}

//void day_add_time_slot(Day day, int start_time,

Cell cell_create(char const * const msg)
{
    int len = strlen(msg);
    char* buf = calloc(len + 1, sizeof (char));
    strcpy(buf, msg);
    Cell cell = { .msg = buf };
    return cell;
}

void cell_destroy(Cell cell) 
{
    if (cell.msg != NULL) free(cell.msg);
}

void cell_draw(WINDOW* win, Cell cell, int start, int offset) {
    int line_start = ((start - offset) / QUARTER) + BORDER_WIDTH;
    DISP_ERR(wmove(win, line_start, 1),);
    DISP_ERR(wprintw(win, " %02dh%02d\n  %s", start / HOUR, start % HOUR, cell.msg),);
}

