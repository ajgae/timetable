#include "main.h"

int main(int argc, char const * argv[]) 
{
    printf("Hewwo world! uwunicode %d\n", 10);

    init_ncurses();
    loop();
    DISP_ERR(endwin(),);

    return 0;
}

void init_ncurses(void) 
{
    initscr();
    DISP_ERR(cbreak(), "cbreak");
    DISP_ERR(noecho(),);
    DISP_ERR(refresh(),);
}

void loop(void) 
{
    Day day = create_day_empty();

    char c = 0;
    do {
        draw_day(day, DAY_START);
        DISP_ERR(refresh(),);

        c = getch();
        /* input bullshit */
    } while(c != 'q');

    destroy_day(day);
}

void cursor_move_to(Cursor const cursor) 
{
    DISP_ERR(move(cursor.y, cursor.x),)
}

Day create_day_empty(void)
{
    Cell* cells = malloc(sizeof(Cell));
    *cells = create_cell("<empty>");
    int* junc = malloc(sizeof(int));
    *junc = DAY_START;

    WINDOW* window = newwin(DAY_HEIGHT, DAY_WIDTH, 0, 0);
    Day day = { .win = window, .count = 1, .juncs = junc, .cells = cells };
    return day;
}

void destroy_day(Day day) 
{
    if (day.win != NULL) delwin(day.win);
    if (day.juncs != NULL) free(day.juncs);
    if (day.cells != NULL) {
        for (int i = 0; i < day.count; ++i) {
            destroy_cell(day.cells[i]);
        }
        free(day.cells);
    }
}

Cell create_cell(char const * const msg)
{
    int len = strlen(msg);
    char* buf = calloc(len + 1, sizeof(char));
    strcpy(buf, msg);
    Cell cell = { .msg = buf };
    return cell;
}

void destroy_cell(Cell cell) 
{
    if (cell.msg != NULL) free(cell.msg);
}

void draw_day(Day day, int offset) {
    for (int i = 0; i < day.count; ++i) {
        draw_cell(day.win, day.cells[i], day.juncs[i], offset);
    }
    box(day.win, 0, 0);
    wrefresh(day.win);
}

void draw_cell(WINDOW* win, Cell cell, int start, int offset) {
    int line_start = ((start - offset) / QUARTER) + BORDER_WIDTH;
    DISP_ERR(wmove(win, line_start, 1),);
    DISP_ERR(wprintw(win, " %02dh%02d\n  %s", start / HOUR, start % HOUR, cell.msg),);
}

