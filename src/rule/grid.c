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
    if (app->mine_pourcentage < 0 || app->mine_pourcentage > 100) {
        write_log(LOG_ERROR, "Invalid mine percentage: %d", app->mine_pourcentage);
        return 1;
    }
    app->game_grid.total_mines = (total_cells * app->mine_pourcentage) / 100;
    if (app->game_grid.total_mines <= 0) {
        write_log(LOG_ERROR, "Calculated total mines is non-positive: %d",
                app->game_grid.total_mines);
        return 1;
    }

    app->game_grid.cells = malloc(app->grid_rows * sizeof(Cell *));
    if (app->game_grid.cells == NULL) {
        write_log(LOG_ERROR, "Failed to allocate memory for grid rows.");
        return 1;
    }
    app->game_grid.revealed_cells = 0;
    app->game_grid.mine_placed = FALSE;

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

int place_mines(APPstate *app, int first_row, int first_col) {
    int placed_mines = 0;
    while (placed_mines < app->game_grid.total_mines) {
        int r = rand() % app->grid_rows;
        int c = rand() % app->grid_columns;
        if (!app->game_grid.cells[r][c].is_mine && !(r == first_row && c == first_col)) {
            app->game_grid.cells[r][c].is_mine = TRUE;
            placed_mines++;
        }
    }

    // Set neighbors for each cell
    for (int r = 0; r < app->grid_rows; r++) {
        for (int c = 0; c < app->grid_columns; c++) {
            int idx = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) {
                        continue;
                    }
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < app->grid_rows &&
                        nc >= 0 && nc < app->grid_columns) {
                        app->game_grid.cells[r][c].neighbors[idx] = &app->game_grid.cells[nr][nc];
                    } else {
                        app->game_grid.cells[r][c].neighbors[idx] = NULL;
                    }
                    idx++;
                }
            }
        }
    }

    // Calculate adjacent mines for each cell
    for (int r = 0; r < app->grid_rows; r++) {
        for (int c = 0; c < app->grid_columns; c++) {
            app->game_grid.cells[r][c].adjacent_mines = count_adjacent_mines(app, r, c);
        }
    }

    reveal_cell(app, first_row, first_col);
    return TRUE;
}

int count_adjacent_mines(APPstate *app, int r, int c) {
    int count = 0;
    for (int n = 0; n < 8; n++) {
        Cell *nb = app->game_grid.cells[r][c].neighbors[n];
        if (nb && nb->is_mine) {
            count++;
        }
    }
    return count;
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

    if (cell->is_mine) {
        write_log(LOG_INFO, "Revealed a mine at (%d, %d). Game over.", row, col);
        return -1; // Indicate that a mine was revealed
    }

    cell->is_revealed = TRUE;
    app->game_grid.revealed_cells++;
    write_log(LOG_INFO, "Revealed cell (%d, %d) and has %d adjacent mines.", row, col, count_adjacent_mines(app, row, col));

    if (cell->adjacent_mines == 0) {
        // Reveal adjacent cells recursively
        for (int n = 0; n < 8; n++) {
            Cell *nb = cell->neighbors[n];
            if (nb != NULL && !nb->is_revealed && !nb->is_mine) {
                revel_chaine(nb, app);
            }
        }
    }

    return 0;
}

int revel_chaine(Cell *cell, APPstate *app) {
    if (cell->is_revealed || cell->is_mine) {
        return 0;
    }

    cell->is_revealed = TRUE;
    app->game_grid.revealed_cells++;
    write_log(LOG_INFO, "Revealed cell in chain with %d adjacent mines.", cell->adjacent_mines);

    if (cell->adjacent_mines == 0) {
        for (int n = 0; n < 8; n++) {
            Cell *nb = cell->neighbors[n];
            if (nb != NULL && !nb->is_revealed && !nb->is_mine) {
                revel_chaine(nb, app);
            }
        }
    }

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

int count_flagged_cells(APPstate *app) {
    if (app->game_grid.cells == NULL) {
        write_log(LOG_WARNING, "Game grid cells are NULL when counting flagged cells.");
        return 0;
    }
    int flagged_count = 0;
    for (int r = 0; r < app->game_grid.rows; r++) {
        for (int c = 0; c < app->game_grid.columns; c++) {
            if (app->game_grid.cells[r][c].is_flagged) {
                flagged_count++;
            }
        }
    }
    return flagged_count;
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