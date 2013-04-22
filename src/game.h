/*
 * Copyright 2013 Moritz Hilscher
 *
 * This file is part of tigol.
 *
 * mapcrafter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mapcrafter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mapcrafter.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GAME_H
#define GAME_H

#define bool char
#define true 1
#define false 0

#define CELL_ALIVE 42

// the type of a single cell on the field
typedef char field_t;

/**
 * A game struct.
 */
typedef struct game_s {
    // width, height
    int width, height;

    // array with current field and a temporary array
    // for the calculation of the next generation
    field_t *current_field, *next_field;
} game_t;

/**
 * Returns a cell at a specific position.
 */
inline int _field_get(field_t* field, int width, int x, int y) {
    return field[y * width + x];
}

/**
 * Sets a cell at a specific position.
 */
inline void _field_set(field_t* field, int width, int x, int y, int value) {
    field[y * width + x] = value;
}

/**
 * Increases the value of a cell at a specific position.
 */
inline void _field_incr(field_t* field, int width, int x, int y, int value) {
    field[y * width + x] += value;
}

/**
 * Updates the neighbors of a cell.
 */
void _field_update_neighbors(field_t* field, int width, int height, int x,
        int y, int value);

/**
 * Creates a game instance.
 */
game_t* game_init(int width, int height);

/**
 * Deletes a game instance.
 */
void game_free(game_t* game);

/**
 * Sets the state of a cell from a game.
 */
void game_set(game_t* game, int x, int y, bool alive);

/**
 * Returns the state of a cell from a game.
 */bool game_get(game_t* game, int x, int y);

/**
 * Fills the game with random cells.
 */
void game_fill_random(game_t* game, double epsilon);

/**
 * Calculates the next generation of a game.
 */
void game_step(game_t* game);

#endif /* end of include guard: GAME_H */

