#ifndef GRID_H
#define GRID_H

#define FALSE 0
#define TRUE 1

typedef struct APPstate APPstate; // Forward declaration

/** 
 * Represents a single cell in the game grid.
 */
typedef struct cell {
    int is_mine;        /**< Flag indicating if the cell contains a mine. */
    int is_revealed;    /**< Flag indicating if the cell has been revealed. */
    int is_flagged;     /**< Flag indicating if the cell is flagged by the player. */
    int adjacent_mines; /**< Number of adjacent mines. */
    struct cell *neighbors[8]; /**< Pointers to neighboring cells. */
} Cell;

/** 
 * Represents the game grid.
 */
typedef struct {
    Cell **cells;       /**< 2D array of cells representing the grid. */
    int columns;       /**< Number of columns in the grid. */
    int rows;          /**< Number of rows in the grid. */
    int total_mines;   /**< Total number of mines in the grid. */
    int revealed_cells;/**< Count of revealed cells. */
} Grid;

/**
 * Initializes the game grid based on the current application state.
 *
 * Sets up the grid dimensions and mine placements according to the
 * specified number of columns, rows, and mine percentage in the APPstate.
 *
 * \param app Pointer to the current application state.
 * \return 0 on success, non-zero on error.
 */
int initialize_grid(APPstate *app);

/**
 * Reveals a cell at the specified row and column.
 *
 * Updates the cell's state to revealed and performs any necessary
 * game logic (e.g., revealing adjacent cells if there are no adjacent mines).
 *
 * \param app Pointer to the current application state.
 * \param row Row index of the cell to reveal.
 * \param col Column index of the cell to reveal.
 * \return 0 on success, 1 on error and -1 if the cell is a mine.
 */
int reveal_cell(APPstate *app, int row, int col);

/**
 * Recursively reveals adjacent cells when a cell with zero adjacent mines is revealed.
 *
 * \param cell Pointer to the cell to reveal.
 * \param app Pointer to the current application state.
 * \return 0 on success, non-zero on error.
 */
int revel_chaine(Cell *cell, APPstate *app);

/**
 * Counts the number of adjacent mines for a given cell.
 *
 * \param app Pointer to the current application state.
 * \param row Row index of the cell.
 * \param col Column index of the cell.
 * \return Number of adjacent mines.
 */
int count_adjacent_mines(APPstate *app, int row, int col);

/**
 * Flags or unflags a cell at the specified row and column.
 *
 * Toggles the flagged state of the cell, preventing it from being revealed.
 *
 * \param app Pointer to the current application state.
 * \param row Row index of the cell to flag/unflag.
 * \param col Column index of the cell to flag/unflag.
 * \return 0 on success, non-zero on error.
 */
int flagged_cell(APPstate *app, int row, int col);

/**
 * Counts the number of flagged cells in the grid.
 *
 * \param app Pointer to the current application state.
 * \return Number of flagged cells.
 */
int count_flagged_cells(APPstate *app);

/**
 * Cleans up the game grid resources.
 *
 * Frees any allocated memory and resets grid-related data in the APPstate.
 *
 * \param app Pointer to the current application state.
 */
void cleanup_grid(APPstate *app);



#endif // GRID_H