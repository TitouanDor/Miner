#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <ncurses.h>

/**
 * Represents the current window/screen displayed by the program.
 */
enum ActualWindow {
    MENU,    /**< Main menu screen. */
    GAME,    /**< Game screen. */
    END  /**< End screen. */
};

/**
 * Represents the current state of the game.
 */
typedef struct {
    WINDOW *window;               /**< Pointer to the main ncurses window. */
    enum ActualWindow current_window; /**< Current active window/screen. */
    int running;                  /**< Flag indicating if the application is running. */
    int cursor_x;                /**< X position of the cursor in the game grid. */
    int cursor_y;                /**< Y position of the cursor in the game grid. */
} APPstate;

#endif // GAMESTATE_H