#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "rule/grid.h"
#include "log/log.h"
#include "ui/ui.h"
#include "GameState.h"

volatile sig_atomic_t sigint_triggered = 0;

void sigint_handler(int signal){
    if (signal == SIGINT) {
        write_log(LOG_INFO, "SIGINT received, preparing to exit...");
        sigint_triggered = 1;
    }
}

APPstate *init_APPstate() {
    APPstate *app = malloc(sizeof(APPstate));
    if (app == NULL) {
        write_log(LOG_ERROR, "Failed to allocate memory for APPstate.");
        return NULL;
    }
    app->window = initialize_ncurses();
    if (app->window == NULL) {
        write_log(LOG_ERROR, "Failed to initialize ncurses.");
        return NULL;
    }
    app->current_window = MENU;
    app->running = 1;
    app->cursor_x = 0;
    app->cursor_y = 0;
    app->grid_columns = 10;
    app->grid_rows = 10;
    app->mine_pourcentage = 15;
    app->mouse_event = (MEVENT){0};
    app->game_grid.cells = NULL;
    wtimeout(app->window, 100); // Set input timeout
    return app;
}

int main() {
    if (init_log() != 0) {
        printf("[ERROR] : Log initialization failed.\n");
        return 1;
    }

    APPstate *app = init_APPstate();
    if (app == NULL) {
        write_log(LOG_ERROR, "Application state initialization failed.");
        return 1;
    }
    srand(time(NULL));

    write_log(LOG_INFO, "Application started.");
    while (app->running && !sigint_triggered) {
        signal(SIGINT, sigint_handler);

        int ch = getch();

        if (ch == KEY_MOUSE) {
            MEVENT event;
            if (getmouse(&event) == OK) {
                app->mouse_event = event;

                if (event.bstate & REPORT_MOUSE_POSITION) {
                    app->cursor_x = event.x;
                    app->cursor_y = event.y;
                }
            }
        } else {
            /* handle keyboard here: change app->cursor_y on KEY_UP/KEY_DOWN, etc. */
        }


        switch (app->current_window) {
            case MENU:
                if (draw_menu(app) == 1){
                    write_log(LOG_ERROR, "Error drawing menu screen.");
                    app->running = 0;
                }
                break;

            case GAME:
                if (draw_game(app) == 1){
                    write_log(LOG_ERROR, "Error drawing game screen.");
                    app->running = 0;
                }
                break;
            
            case OPTIONS:
                if (draw_options(app) == 1){
                    write_log(LOG_ERROR, "Error drawing options screen.");
                    app->running = 0;
                }
                break;

            case END:
                app->running = 0; // Placeholder to exit the loop
                break;

            default:
                write_log(LOG_WARNING, "Unknown window state.");
                app->running = 0;
                break;
        }
        wrefresh(app->window);
    }
    
    cleanup_ncurses();
    write_log(LOG_INFO, "Application exited.\n\n\n");
    return 0;
}