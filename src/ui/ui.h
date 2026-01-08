#ifndef UI_H
#define UI_H

#include <ncurses.h>

/**
 * Represents the current window/screen displayed by the program.
 */
enum ActualWindow {
    MENU,    /**< Main menu screen. */
    GAME,    /**< Game screen. */
    OPTIONS  /**< Options/settings screen. */
};

/**
 * Initializes the ncurses library and creates the main window.
 *
 * Sets up ncurses mode (input, colors, echo settings, etc.) and returns
 * a window ready to be used for drawing the UI.
 *
 * \return Pointer to the main ncurses window, or NULL on error.
 */
WINDOW *initialize_ncurses();

/**
 * Draws the main menu on the given ncurses window.
 *
 * Displays menu options and handles basic layout.
 *
 * \param win Pointer to the ncurses window where the menu will be drawn.
 * \return 0 on success, non-zero on error.
 */
int draw_menu(WINDOW *win);

#endif // UI_H