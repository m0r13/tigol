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

#define SAVE_SCREEN

#define USE_TI89
#define USE_TI92PLUS
#define USE_V200

#include <tigcclib.h>
#include <kbd.h>
#include <args.h>

#include <stdio.h>
#include <stdlib.h>

// http://tigcc.ticalc.org/tut/drawline.html
#define PIXOFFSET(x,y) ((y<<5)-(y<<1)+(x>>3))
#define PIXADDR(p,x,y) (((unsigned char*)(p))+PIXOFFSET(x,y))
#define PIXMASK(x) ((unsigned char)(0x80 >> ((x)&7)))
#define GETPIX(p,x,y) (!(!(*PIXADDR(p,x,y) & PIXMASK(x))))
#define SETPIX(p,x,y) (*PIXADDR(p,x,y) |=  PIXMASK(x))
#define CLRPIX(p,x,y) (*PIXADDR(p,x,y) &= ~PIXMASK(x))
#define XORPIX(p,x,y) (*PIXADDR(p,x,y) ^=  PIXMASK(x))

// an array of the status of the cells on the screen
// so we only need to draw changed pixels
bool* cells;

/**
 * Draws the game.
 */
void game_draw(game_t* game, int size) {
    for(int y = 0; y < game->height; y++)
        for(int x = 0; x < game->width; x++) {
            bool status = game_get(game, x, y);
            if(status != cells[y * game->width + x]) {
                cells[y * game->width + x] = status;

                int sx = x * size;
                int sy = y * size;
                for(int dx = 0; dx < size; dx++)
                    for(int dy = 0; dy < size; dy++)
                        XORPIX(LCD_MEM, sx+dx, sy+dy);
            }
        }
}

void _main() {
    ClrScr ();
    randomize();

    // check if a cell size was given as arg
    int size = 5;
    if(ArgCount() > 0) {
        ESI argptr = top_estack;
        size = GetIntArg(argptr);
    }

    // create game, extra cells array
    game_t* game = game_init(LCD_WIDTH / size, LCD_HEIGHT / size);
    cells = (bool*) calloc(game->width * game->height, sizeof(bool));

    // fill game for the first time
    game_fill_random(game, 0.5);

    // is the game paused?
    bool paused = true;

    // keyboard stuff
    void* kdq = kbd_queue();
    unsigned short key;
    while(1) {
        // check pressed keys
        if(!OSdequeue (&key, kdq)) {
            if(key == KEY_ESC)
                break;
            if(key == 'p')
                paused = paused == 0 ? 1 : 0;
            if(key == 'r')
                game_fill_random(game, 0.5);
        }

        // draw the game
        game_draw(game, size);

        // calculate next generation when not paused or next clicked
        if(!paused || (paused && key == KEY_ENTER)) {
            game_step(game);
        }

        // when paused wait for a keypress
        if(paused) {
            OSenqueue (ngetchx(), kdq);
        }
    }

    // delete game resources
    game_free(game);
}
