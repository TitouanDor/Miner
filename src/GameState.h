#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <ncurses.h>
#include <time.h>
#include "rule/grid.h"

/**
 * Represents the current window/screen displayed by the program.
 */
enum ActualWindow {
    MENU,    /**< Main menu screen. */
    GAME,    /**< Game screen. */
    OPTIONS, /**< Options screen. */
    END      /**< End screen. */
};


/**
 * Represents mouse button states.
 */
enum MouseButtons {
    NONE, /**< No button pressed. */
    LEFT_BUTTON, /**< Left mouse button pressed. */
    MIDDLE_BUTTON, /**< Middle mouse button pressed. */
    RIGHT_BUTTON /**< Right mouse button pressed. */
};

/**
 *  Represents a mouse event.
 */
typedef struct mouse {
    int x;  /**< X coordinate of the mouse event. */
    int y;  /**< Y coordinate of the mouse event. */
    int click; /**< Button state of the mouse event. */
} MouseEvent;

/**
 * Represents the current state of the game.
 */
struct APPstate{
    WINDOW *window;               /**< Pointer to the main ncurses window. */
    enum ActualWindow current_window; /**< Current active window/screen. */
    int running;                  /**< Flag indicating if the application is running. */
    MouseEvent mouse;          /**< Mouse event structure. */
    int cursor_x;                /**< Current x position of the cursor. */
    int cursor_y;                /**< Current y position of the cursor. */
    int grid_columns;            /**< Number of columns in the game grid. */
    int grid_rows;               /**< Number of rows in the game grid. */
    int mine_pourcentage;        /**< Percentage of mines in the game grid. */
    Grid game_grid;              /**< The game grid structure. */
    time_t start_time;          /**< Game start time. */
    time_t end_time;            /**< Game end time. */
};

#endif // GAMESTATE_H