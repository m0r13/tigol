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

#include "game.h"

#include <stdlib.h>
#include <string.h>

void _field_update_neighbors(field_t* field, int width, int height, int x,
        int y, int value) {
    // try to increase/decrease neighbor cells, if they exist
    bool left = x > 0;
    bool right = x < width - 1;
    bool top = y > 0;
    bool bottom = y < height - 1;

    if(left)
        _field_incr(field, width, x - 1, y, value);
    if(right)
        _field_incr(field, width, x + 1, y, value);

    if(top)
        _field_incr(field, width, x, y - 1, value);
    if(bottom)
        _field_incr(field, width, x, y + 1, value);

    if(left && top)
        _field_incr(field, width, x - 1, y - 1, value);
    if(left && bottom)
        _field_incr(field, width, x - 1, y + 1, value);
    if(right && top)
        _field_incr(field, width, x + 1, y - 1, value);
    if(right && bottom)
        _field_incr(field, width, x + 1, y + 1, value);
}

game_t* game_init(int width, int height) {
    game_t* game = (game_t*) malloc (sizeof(game_t));
    game->width = width;
    game->height = height;

    game->current_field = (field_t*) calloc(game->width * game->height,
            sizeof(field_t));
    game->next_field = (field_t*) calloc(game->width * game->height,
            sizeof(field_t));
    return game;
}

void game_free(game_t* game) {
    free (game->current_field);
    free (game->next_field);
    free (game);
}

void game_set(game_t* game, int x, int y, bool alive) {
    // check bounds
    if(x < 0 || x >= game->width || y < 0 || y >= game->height)
        return;
    field_t cell = _field_get(game->current_field, game->width, x, y);
    // only set something when the state of the cell changed
    if(cell >= CELL_ALIVE && !alive) {
        _field_incr(game->current_field, game->width, x, y, -CELL_ALIVE);
        _field_update_neighbors(game->current_field, game->width, game->height,
                x, y, -1);
    } else if(cell < CELL_ALIVE && alive) {
        _field_incr(game->current_field, game->width, x, y, CELL_ALIVE);
        _field_update_neighbors(game->current_field, game->width, game->height,
                x, y, 1);
    }
}

bool game_get(game_t* game, int x, int y) {
    // check for bounds
    if(x < 0 || x >= game->width || y < 0 || y >= game->height)
        return false;
    // the field value must be >= CELL_ALIVE for a living cell
    return _field_get(game->current_field, game->width, x, y) >= CELL_ALIVE;
}

void game_fill_random(game_t* game, double epsilon) {
    for(int y = 0; y < game->height; y++)
        for(int x = 0; x < game->width; x++)
            game_set(game, x, y, ((double) rand() / RAND_MAX) < epsilon);
}

void game_step(game_t* game) {
    for(int y = 0; y < game->height; y++)
        for(int x = 0; x < game->width; x++) {
            field_t cell = _field_get(game->current_field, game->width, x, y);
            // check if cell is alive
            bool alive = cell >= CELL_ALIVE;
            // get count of neighbors
            int neighbors = cell - (alive ? CELL_ALIVE : 0);
            // depending on the ruleset
            if((alive && (neighbors == 2 || neighbors == 3))
                    || (!alive && neighbors == 3)) {
                // set cell alive in the next generation
                _field_incr(game->next_field, game->width, x, y, CELL_ALIVE);
                _field_update_neighbors(game->next_field, game->width,
                        game->height, x, y, 1);
            }
        }

    // copy temporary field
    memcpy (game->current_field, game->next_field,
            sizeof(field_t) * game->width * game->height);
    memset (game->next_field, 0, sizeof(field_t) * game->width * game->height);
}
