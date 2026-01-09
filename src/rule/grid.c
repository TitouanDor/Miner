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
            app->game_grid.cells[r][c].x = r;
            app->game_grid.cells[r][c].y = c;
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
            Cell *cell = &app->game_grid.cells[r][c];
            cell->adjacent_mines = count_adjacent_mines(cell);
        }
    }

    app->game_grid.mine_placed = TRUE;
    // Reveal the first clicked cell
    Cell *first_cell = &app->game_grid.cells[first_row][first_col];
    reveal_cell(app, first_cell);
    return TRUE;
}

int count_adjacent_mines(Cell *cell) {
    int count = 0;
    for (int n = 0; n < 8; n++) {
        Cell *nb = cell->neighbors[n];
        if (nb && nb->is_mine) {
            count++;
        }
    }
    return count;
}

int reveal_cell(APPstate *app, Cell *cell) {
    if (cell->x < 0 || cell->x >= app->game_grid.rows || cell->y < 0 || cell->y >= app->game_grid.columns) {
        write_log(LOG_WARNING, "Attempted to reveal cell out of bounds (%d, %d).", cell->x, cell->y);
        return 1;
    }

    if (cell->is_revealed) {
        reveal_all_flagged(app, cell);
        return 0; // Cell already revealed
    }

    if (cell->is_mine) {
        write_log(LOG_INFO, "Revealed a mine at (%d, %d). Game over.", cell->x, cell->y);
        return -1; // Indicate that a mine was revealed
    }

    cell->is_revealed = TRUE;
    app->game_grid.revealed_cells++;
    write_log(LOG_INFO, "Revealed cell (%d, %d) and has %d adjacent mines.", cell->x, cell->y, count_adjacent_mines(cell));

    if (cell->adjacent_mines == 0) {
        // Reveal adjacent cells recursively
        for (int n = 0; n < 8; n++) {
            Cell *nb = cell->neighbors[n];
            if (nb != NULL && !nb->is_revealed && !nb->is_mine) {
                reveal_chaine(app, nb);
            }
        }
    }

    return 0;
}

int reveal_chaine(APPstate *app, Cell *cell) {
    if (cell->is_revealed || cell->is_flagged) {
        return 0;
    } else if (cell->is_mine){
        return -1;
    }

    cell->is_revealed = TRUE;
    app->game_grid.revealed_cells++;
    write_log(LOG_INFO, "Revealed cell in chain with %d adjacent mines.", cell->adjacent_mines);

    if (cell->adjacent_mines == 0) {
        for (int n = 0; n < 8; n++) {
            Cell *nb = cell->neighbors[n];
            if (nb != NULL && !nb->is_revealed && !nb->is_mine) {
                reveal_chaine(app, nb);
            }

            if (nb != NULL && nb->is_mine) {
                return -1; // A mine was revealed in the chain
            }
        }
    }

    return 0;
}

int reveal_all_flagged(APPstate *app, Cell *cell) {
    if (cell->is_revealed) {
        int flagged_neighbors = 0;
        for (int n = 0; n < 8; n++) {
            Cell *nb = cell->neighbors[n];
            if (nb != NULL && nb->is_flagged) {
                flagged_neighbors++;
            }
        }

        if (flagged_neighbors == cell->adjacent_mines) {
            write_log(LOG_INFO, "Revealing adjacent cells for (%d, %d) as flagged neighbors match adjacent mines.", cell->x, cell->y);
            for (int n = 0; n < 8; n++) {
                Cell *nb = cell->neighbors[n];
                if (nb != NULL && !nb->is_flagged && !nb->is_revealed) {
                    int result = reveal_chaine(app, nb);
                    if (result == -1) {
                        return -1; // A mine was revealed
                    }
                }
            }
        } else {
            write_log(LOG_INFO, "Not enough flagged neighbors to reveal adjacent cells for (%d, %d).", cell->x, cell->y);
        }

    }
    
    return 0;
}

int flagged_cell(APPstate *app, Cell *cell) {
    if (cell->x < 0 || cell->x >= app->game_grid.rows || cell->y < 0 || cell->y >= app->game_grid.columns) {
        write_log(LOG_WARNING, "Attempted to flag cell out of bounds (%d, %d).", cell->x, cell->y);
        return 1;
    }

    if (cell->is_revealed) {
        write_log(LOG_INFO, "Cannot flag revealed cell (%d, %d).", cell->x, cell->y);
        return 1; // Cannot flag a revealed cell
    }

    cell->is_flagged = !cell->is_flagged;
    write_log(LOG_INFO, "%s cell (%d, %d).", cell->is_flagged ? "Flagged" : "Unflagged", cell->x, cell->y);

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