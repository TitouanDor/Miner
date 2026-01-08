#include <string.h>
#include "ui.h"
#include "../log/log.h"
#include "./../GameState.h"

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

int draw_menu(APPstate *app) {
    WINDOW *win = app->window;
    getmaxyx(win, max_y, max_x);
    if (win == NULL) {
        write_log(LOG_ERROR, "Window is NULL in draw_menu.");
        return 1;
    }
    char *menu[] = {
        "=== Main Menu ===",
        "",
        "Start a new game",
        "Load a game",
        "Options",
        "Quit",
        NULL
    };

    if (app->cursor_y < 2) app->cursor_y = 2;
    if (app->cursor_y > 5) app->cursor_y = 5;

    werase(win);
    for(int i = 0; menu[i] != NULL; i++) {
        if (i == app->cursor_y) {
            wattron(win, A_REVERSE);
        }
        middle_x(win, i, menu[i]);
        if (i == app->cursor_y) {
            wattroff(win, A_REVERSE);
        }
    }
    wrefresh(win);
    return 0;
}

void middle_x(WINDOW *win, int y_pos, const char *text) {
    int x, y;
    getmaxyx(win, y, x);
    mvwprintw(win, y_pos, (x - strlen(text)) / 2, "%s", text);
}