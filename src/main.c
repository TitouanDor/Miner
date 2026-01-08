#include <stdio.h>
#include "log/log.h"
#include "ui/ui.h"

typedef struct {
    WINDOW *window;
    enum ActualWindow current_window;
    int running;
} APPstate;

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
    wtimeout(app.window, 100); // Set input timeout
    write_log(LOG_INFO, "Application started.");
    while (app.running) {
        switch (app.current_window) {
            case MENU:
                // Draw menu and handle input
                draw_menu(app.window);
                break;
            case GAME:
                // Game logic would go here
                break;
            case OPTIONS:
                // Options logic would go here
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