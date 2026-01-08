#include <stdio.h>
#include <stdlib.h>
#include "log/log.h"
#include "ui/ui.h"
#include "GameState.h"

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
    app->mouse_event = (MEVENT){0};
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

    write_log(LOG_INFO, "Application started.");
    while (app->running) {

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
                draw_menu(app);
                break;

            case GAME:
                break;

            case END:
                break;

            default:
                write_log(LOG_WARNING, "Unknown window state.");
                app->running = 0;
                break;
        }
    }
    
    cleanup_ncurses();
    write_log(LOG_INFO, "Application exited.");
    return 0;
}