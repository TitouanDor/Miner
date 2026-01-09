#include <string.h>
#include <stdlib.h>
#include "ui.h"
#include "../log/log.h"
#include "./../GameState.h"

int max_x, max_y, n = 0;
const char *menu[] = {
    "=== Miner ===",
    "",
    "Start a new game (default settings)",
    "Options",
    "Quit",
    NULL
};

const char *options[] = {
    "=== Options ===",
    "",
    "Columns",
    "",
    "Rows",
    "",
    "Mines (% of grid)",
    "",
    "Start",
    "Back to Menu",
    "Quit",
    NULL
};

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
    
    werase(win);
    while (menu[n]) n++;

    for(int i = 0; menu[i] != NULL; i++) {
        if (i == app->cursor_y && i >=2 && i < n) {
            wattron(win, A_REVERSE);
        }
        middle_x(win, i, menu[i]);
        if (i == app->cursor_y && i >=2 && i < n) {
            wattroff(win, A_REVERSE);
        }
    }

    if (app->cursor_y == 2 && (app->mouse_event.bstate & BUTTON1_PRESSED)) {
        app->mouse_event.bstate = 0;
        app->start_time = time(NULL);
        app->current_window = GAME;
        write_log(LOG_INFO, "New game started from menu.");

    } else if (app->cursor_y == 3 && (app->mouse_event.bstate & BUTTON1_PRESSED)) {
        app->mouse_event.bstate = 0;
        app->current_window = OPTIONS;
        write_log(LOG_INFO, "Options selected from menu.");

    } else if (app->cursor_y == 4 && (app->mouse_event.bstate & BUTTON1_PRESSED)) {
        app->mouse_event.bstate = 0;
        write_log(LOG_INFO, "Quit selected from menu.");
        app->running = 0;
    }
    return 0;
}

int draw_options(APPstate *app) {
    int clicked_y = -1;
    int clicked_x = -1;
    int plus_x = -1;
    int minus_x = -1;
    WINDOW *win = app->window;
    getmaxyx(win, max_y, max_x);
    if (win == NULL) {
        write_log(LOG_ERROR, "Window is NULL in draw_options.");
        return 1;
    }
    
    werase(win);
    while (options[n]) n++;

    for(int i = 0; options[i] != NULL; i++) {
        if ((i == app->cursor_y && (i >=2 && i < n)) || (i+1==app->cursor_y && options[i+1] && strcmp(options[i+1], "") == 0)) {
            wattron(win, A_REVERSE);
        }
        middle_x(win, i, options[i]);
        if ((i == app->cursor_y && (i >=2 && i < n)) || (i+1==app->cursor_y && options[i+1] && strcmp(options[i+1], "") == 0)) {
            wattroff(win, A_REVERSE);
        }
    }

    char *col, *row, *mine;
    asprintf(&col, "- %d +", app->grid_columns);
    asprintf(&row, "- %d +", app->grid_rows);
    asprintf(&mine, "- %d +", app->mine_pourcentage);
    middle_x(win, 3, col);
    middle_x(win, 5, row);
    middle_x(win, 7, mine);

    if (app->mouse_event.bstate & BUTTON1_PRESSED) {
        app->mouse_event.bstate = 0;
        clicked_y = app->mouse_event.y;
        clicked_x = app->mouse_event.x;
    }

    if (clicked_y == 8){
        app->current_window = GAME;
        app->start_time = time(NULL);
        write_log(LOG_INFO, "Starting game from options.");

    } else if (clicked_y == 9){
        app->current_window = MENU;
        write_log(LOG_INFO, "Returning to menu from options.");

    } else if (clicked_y == 10) {
        write_log(LOG_INFO, "Quit selected from options.");
        app->running = 0;

    } else if (clicked_y == 3) {
        write_log(LOG_INFO, "Increasing columns from %d to %d", app->grid_columns, app->grid_columns + 1);
        plus_x = (max_x - strlen(col)) / 2 + strlen(col) - 1;
        minus_x = (max_x - strlen(col)) / 2;
        if (clicked_x == plus_x && app->grid_columns < 30) {
            app->grid_columns += 1;
        } else if (clicked_x == minus_x && app->grid_columns > 5) {
            app->grid_columns -= 1;
        }

    } else if (clicked_y == 5) {
        write_log(LOG_INFO, "Increasing rows from %d to %d", app->grid_rows, app->grid_rows + 1);
        plus_x = (max_x - strlen(row)) / 2 + strlen(row) - 1;
        minus_x = (max_x - strlen(row)) / 2;
        if (clicked_x == plus_x && app->grid_rows < 30) {
            app->grid_rows += 1;
        } else if (clicked_x == minus_x && app->grid_rows > 5) {
            app->grid_rows -= 1;
        }

    } else if (clicked_y == 7) {
        write_log(LOG_INFO, "Increasing mines from %d to %d", app->mine_pourcentage, app->mine_pourcentage + 1);
        plus_x = (max_x - strlen(mine)) / 2 + strlen(mine) - 1;
        minus_x = (max_x - strlen(mine)) / 2;
        if (clicked_x == plus_x && app->mine_pourcentage < 23) {
            app->mine_pourcentage += 1;
        } else if (clicked_x == minus_x && app->mine_pourcentage > 12) {
            app->mine_pourcentage -= 1;
        }
    }
    
    free(col);
    free(row);
    free(mine);
    return 0;
}

int draw_game(APPstate *app) {
    int clicked_y = -1;
    int clicked_x = -1;
    int mouse_clicked = 0;
    WINDOW *win = app->window;
    if (win == NULL) {
        write_log(LOG_ERROR, "Window is NULL in draw_game.");
        return 1;
    }
    getmaxyx(win, max_y, max_x);

    if (app->game_grid.cells == NULL) {
        if (initialize_grid(app) != 0) {
            write_log(LOG_ERROR, "Failed to initialize game grid.");
            return 1;
        }
        write_log(LOG_INFO, "Grid %d x %d, mines=%d", app->grid_rows, app->grid_columns, app->game_grid.total_mines);
    }

    if (app->game_grid.cells == NULL) {
        write_log(LOG_ERROR, "Failed to initialize game grid.");
    }

    
    
    werase(win);
    time_t now = time(NULL);
    int elapsed = (int)difftime(now, app->start_time);
    char time_str[20];
    snprintf(time_str, sizeof(time_str), "Time: %02d:%02d", elapsed / 60, elapsed % 60);
    middle_x(win, 0, "=== Game Screen ===");
    middle_x(win, 2, time_str);
    //middle_x(win, max_y / 2, "Game screen not yet implemented.");

    if (app->cursor_y == max_y-2){
        wattron(win, A_REVERSE);
    }
    middle_x(win, max_y-2, "Click to return to menu.");
    if (app->cursor_y == max_y-2){
        wattroff(win, A_REVERSE);
    }

    if (app->cursor_y == max_y-1){
        wattron(win, A_REVERSE);
    }
    middle_x(win, max_y-1, "Click to exit.");
    if (app->cursor_y == max_y-1){
        wattroff(win, A_REVERSE);
    }

    int x_start = (max_x - app->grid_columns) / 2;
    int y_start = (max_y - app->grid_rows) / 2;

    if (max_y < app->grid_rows || max_x < app->grid_columns) {
        //this will change later to a interdiction of starting the game with too small window and lock the window size
        write_log(LOG_ERROR, "Window too small (%d x %d) for grid %d x %d", max_y, max_x, app->grid_rows, app->grid_columns);
        return 1;
    }

    for (int r = 0; r < app->grid_rows; r++) {
        for (int c = 0; c < app->grid_columns; c++) {
            if (app->game_grid.cells[r][c].is_revealed == TRUE) {
                char adj[4];
                snprintf(adj, sizeof(adj), "%d", app->game_grid.cells[r][c].adjacent_mines);
                mvwprintw(win, y_start + r, x_start + c, "%s", adj);
            } else if (app->game_grid.cells[r][c].is_flagged == TRUE) {
                mvwprintw(win, y_start + r, x_start + c, "F");
            } else {
                mvwprintw(win, y_start + r, x_start + c, "#");
            }
        }
    }

    mouse_clicked = NONE;

    if (app->mouse_event.bstate & BUTTON1_PRESSED) {
        mouse_clicked = LEFT_BUTTON;
    } else if (app->mouse_event.bstate & BUTTON3_PRESSED) {
        mouse_clicked = RIGHT_BUTTON;
    } else {
        return 0;
    }

    clicked_y = app->mouse_event.y;
    clicked_x = app->mouse_event.x;
    app->mouse_event.bstate = 0;


    if (mouse_clicked == LEFT_BUTTON) {
        if (clicked_y == max_y-2){
            app->current_window = MENU;
            write_log(LOG_INFO, "Returning to menu from game screen.");
        } else if (clicked_y == max_y-1) {
            write_log(LOG_INFO, "Quit selected from game screen.");
            app->running = 0;
        } else {
            reveal_cell(app, clicked_y - y_start, clicked_x - x_start);
        }
    } else if (mouse_clicked == RIGHT_BUTTON) {
        flagged_cell(app, clicked_y - y_start, clicked_x - x_start);
    }
    
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