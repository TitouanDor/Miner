#include <stdio.h>
#include "log/log.h"
#include "ui/ui.h"
#include "GameState.h"

int main() {
    if (init_log() != 0) {
        printf("[ERROR] : Log initialization failed.\n");
        return 1;
    }

    APPstate app;
    app.window = initialize_ncurses();
    if (app.window == NULL) {
        write_log(LOG_ERROR, "Failed to initialize ncurses.");
        return 1;
    }
    app.current_window = MENU;
    app.running = 1;
    app.cursor_x = 0;
    app.cursor_y = 0;
    wtimeout(app.window, 100); // Set input timeout
    write_log(LOG_INFO, "Application started.");
    while (app.running) {

        switch (app.current_window) {
            case MENU:
                draw_menu(&app);
                break;

            case GAME:
                break;

            case END:
                break;

            default:
                write_log(LOG_WARNING, "Unknown window state.");
                app.running = 0;
                break;
        }
    }
    endwin(); // End ncurses mode
    write_log(LOG_INFO, "Application exited.");
    return 0;
}