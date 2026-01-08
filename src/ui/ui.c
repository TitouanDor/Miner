#include "ui.h"
#include "../log/log.h"

int max_x, max_y;

WINDOW *initialize_ncurses() {
    WINDOW *win = initscr();
    if (win == NULL) {
        write_log(LOG_ERROR, "Failed to initialize ncurses.");
        return NULL;
    }
    cbreak();
    noecho();
    keypad(win, TRUE);
    nodelay(win, TRUE);
    return win;
}

int draw_menu(WINDOW *win) {
    getmaxyx(win, max_y, max_x);
    if (win == NULL) {
        write_log(LOG_ERROR, "Window is NULL in draw_menu.");
        return 1;
    }
    werase(win);
    mvwprintw(win, 1, max_x/2 - 10, "=== Main Menu ===");
    mvwprintw(win, 3, max_x/2 - 15, "1. Start a new game");
    mvwprintw(win, 4, max_x/2 - 15, "2. Load a game");
    mvwprintw(win, 5, max_x/2 - 15, "3. Options");
    mvwprintw(win, 6, max_x/2 - 15, "4. Quit");
    wrefresh(win);
    return 0;
}