#include <stdlib.h>
#include "grid.h"
#include "../log/log.h"
#include "../GameState.h"

int initialize_grid(APPstate *app) {
    if (app->grid_rows <= 0 || app->grid_columns <= 0) {
        write_log(LOG_ERROR, "Invalid grid size: %d x %d",
                app->grid_rows, app->grid_columns);
        return 1;
    }

    int total_cells = app->grid_columns * app->grid_rows;
    app->game_grid.columns = app->grid_columns;
    app->game_grid.rows = app->grid_rows;
    app->game_grid.total_mines = (total_cells * app->mine_pourcentage) / 100;
    app->game_grid.revealed_cells = 0;

    app->game_grid.cells = malloc(app->grid_rows * sizeof(Cell *));
    if (app->game_grid.cells == NULL) {
        write_log(LOG_ERROR, "Failed to allocate memory for grid rows.");
        return 1;
    }

    for (int r = 0; r < app->grid_rows; r++) {
        app->game_grid.cells[r] = malloc(app->grid_columns * sizeof(Cell));
        if (app->game_grid.cells[r] == NULL) {
            write_log(LOG_ERROR, "Failed to allocate memory for grid columns.");
            // Free previously allocated rows
            for (int i = 0; i < r; i++) {
                free(app->game_grid.cells[i]);
            }
            free(app->game_grid.cells);
            return 1;
        }
    }

    // Initialize cells
    for (int r = 0; r < app->grid_rows; r++) {
        for (int c = 0; c < app->grid_columns; c++) {
            app->game_grid.cells[r][c].is_mine = FALSE;
            app->game_grid.cells[r][c].is_revealed = FALSE;
            app->game_grid.cells[r][c].is_flagged = FALSE;
            app->game_grid.cells[r][c].adjacent_mines = 0;
            for (int n = 0; n < 8; n++) {
                app->game_grid.cells[r][c].neighbors[n] = NULL;
            }
        }
    }

    return 0;
}

int count_adjacent_mines(APPstate *app, int row, int col) {
    int mine_count = 0;
    for (int n = 0; n < 8; n++) {
        Cell *neighbor = app->game_grid.cells[row][col].neighbors[n];
        if (neighbor != NULL && neighbor->is_mine) {
            mine_count++;
        }
    }
    return mine_count;
}

int reveal_cell(APPstate *app, int row, int col) {
    if (row < 0 || row >= app->game_grid.rows || col < 0 || col >= app->game_grid.columns) {
        write_log(LOG_WARNING, "Attempted to reveal cell out of bounds (%d, %d).", row, col);
        return 1;
    }

    Cell *cell = &app->game_grid.cells[row][col];
    if (cell->is_revealed) {
        write_log(LOG_INFO, "Cell (%d, %d) already revealed.", row, col);
        return 0; // Cell already revealed
    }

    cell->is_revealed = TRUE;
    write_log(LOG_INFO, "Revealed cell (%d, %d).", row, col);
    app->game_grid.revealed_cells++;

    // Additional logic for revealing adjacent cells if no adjacent mines would go here

    return 0;
}

int flagged_cell(APPstate *app, int row, int col) {
    if (row < 0 || row >= app->game_grid.rows || col < 0 || col >= app->game_grid.columns) {
        write_log(LOG_WARNING, "Attempted to flag cell out of bounds (%d, %d).", row, col);
        return 1;
    }

    if (app->game_grid.cells[row][col].is_revealed) {
        write_log(LOG_INFO, "Cannot flag revealed cell (%d, %d).", row, col);
        return 1; // Cannot flag a revealed cell
    }

    Cell *cell = &app->game_grid.cells[row][col];
    cell->is_flagged = !cell->is_flagged;
    write_log(LOG_INFO, "%s cell (%d, %d).", cell->is_flagged ? "Flagged" : "Unflagged", row, col);

    return 0;
}

void cleanup_grid(APPstate *app){
    if (app->game_grid.cells != NULL) {
        for (int i = 0; i < app->game_grid.rows; i++) {
            free(app->game_grid.cells[i]);
        }
        free(app->game_grid.cells);
        app->game_grid.cells = NULL;
    }
}