#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include "./../GameState.h"

enum MouseButtons {
    NONE,
    LEFT_BUTTON,
    MIDDLE_BUTTON,
    RIGHT_BUTTON
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
int draw_menu(APPstate *app);

/**
 * Draws the options screen on the given ncurses window.
 *
 * Displays options and handles basic layout.
 *
 * \param win Pointer to the ncurses window where the options will be drawn.
 * \return 0 on success, non-zero on error.
 */
int draw_options(APPstate *app);

/**
 * Draws the game screen on the given ncurses window.
 *
 * Displays the game grid and related information.
 *
 * \param win Pointer to the ncurses window where the game will be drawn.
 * \return 0 on success, non-zero on error.
 */
int draw_game(APPstate *app);

/**
 * Centers and prints text horizontally in the given window at the specified y position.
 *
 * \param win Pointer to the ncurses window.
 * \param y_pos Y position where the text should be printed.
 * \param text The text string to be printed.
 */
void middle_x(WINDOW *win, int y_pos, const char *text);

/**
 * Cleans up ncurses settings and exits ncurses mode.
 */
void cleanup_ncurses();

#endif // UI_H