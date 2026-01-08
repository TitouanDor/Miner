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
    /* Enable reporting of mouse motion events with no click timeout */
    mouseinterval(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    /* Some terminals (e.g. xterm) require enabling "Any event" reporting */
    printf("\033[?1003h"); fflush(stdout);
    write_log(LOG_INFO, "Has %d Mouse Capabilities", has_mouse());
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
        "Quit",
        NULL
    };

    int n = 0;
    while (menu[n]) n++;

    if (app->cursor_y < 2) app->cursor_y = 2;
    if (app->cursor_y >= n) app->cursor_y = n-1;

    werase(win);
    mvprintw(0, 0, "cy=%d cx=%d bstate=%08x", app->cursor_y, app->cursor_x, app->mouse_event.bstate);

    for(int i = 0; menu[i] != NULL; i++) {
        if (i == app->cursor_y) {
            wattron(win, A_REVERSE);
        }
        middle_x(win, i, menu[i]);
        if (i == app->cursor_y) {
            wattroff(win, A_REVERSE);
        }
    }

    if (app->cursor_y == 2 && (app->mouse_event.bstate & BUTTON1_PRESSED)) {
        app->current_window = GAME;
        write_log(LOG_INFO, "New game started from menu.");
    } else if (app->cursor_y == 3 && (app->mouse_event.bstate & BUTTON1_PRESSED)) {
        write_log(LOG_INFO, "Quit selected from menu.");
        app->running = 0;
    }
    wrefresh(win);
    return 0;
}

void middle_x(WINDOW *win, int y_pos, const char *text) {
    int x, y;
    getmaxyx(win, y, x);
    mvwprintw(win, y_pos, (x - strlen(text)) / 2, "%s", text);
}

void cleanup_ncurses() {
    /* Disable "Any event" reporting */
    printf("\033[?1003l"); 
    fflush(stdout);
    endwin();
}