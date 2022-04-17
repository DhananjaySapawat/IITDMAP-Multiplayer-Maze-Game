#include <iostream> // IO
#include <SDL2/SDL.h> // basic game mechanics
#include <SDL2/SDL_image.h> // image rendering
#include "constants.h"

int check_collisions(SDL_Rect *rect) {
    if (map[rect->y/TILE_SIZE][rect->x/TILE_SIZE] ||
            map[(rect->y + rect->h)/TILE_SIZE][rect->x/TILE_SIZE] ||
            map[(rect->y)/TILE_SIZE][(rect->x + rect->w)/TILE_SIZE] ||
            map[(rect->y + rect->h)/TILE_SIZE][(rect->x + rect->w)/TILE_SIZE] ||
            rect->x <= 0 || (rect->x + rect->w >= SCREEN_WIDTH)) {
        return true;
    } else {
        return false;
    }
}

