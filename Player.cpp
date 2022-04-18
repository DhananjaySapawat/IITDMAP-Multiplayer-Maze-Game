#ifndef PLAYER
#define PLAYER

#include <iostream> // for IO
#include <SDL2/SDL.h> // for basic game mechanics
#include <SDL2/SDL_image.h> // for rendering
#include "constants.h"

class Player{
    public:
        //The X and Y offsets of the player
        int mPosX, mPosY;

        const int PLAYER_VEL = TILE_SIZE/5;

        //The dimensions of the player block
        static const int PLAYER_WIDTH = TILE_SIZE;
        static const int PLAYER_HEIGHT = TILE_SIZE;

        //Initializes the variables
        Player(int xTile, int yTile);

        void move(char msg[]);
};

Player::Player(int xTile, int yTile){
    //Initialize the offsets
    mPosX = xTile*TILE_SIZE;
    mPosY = yTile*TILE_SIZE;
}

void Player::move(char msg[]){
    std::string direction = &msg[0];

    struct block{
        int x, y;
    };

    struct block leftblock;
    leftblock.x = mPosX/TILE_SIZE - 1;
    leftblock.y = mPosY/TILE_SIZE;

    struct block rightblock;
    rightblock.x = mPosX/TILE_SIZE + 1;
    rightblock.y = mPosY/TILE_SIZE;

    struct block upblock;
    upblock.x = mPosX/TILE_SIZE;
    upblock.y = mPosY/TILE_SIZE - 1;

    struct block downblock;
    downblock.x = mPosX/TILE_SIZE;
    downblock.y = mPosY/TILE_SIZE + 1;

    if (direction == "left"){
        mPosX -= PLAYER_VEL;
        //If the player went too far to the left or right
        if(mPosX < 0  || map[leftblock.y][leftblock.x] == 1){
        // if(mPosX < 0){
            mPosX += PLAYER_VEL; //Move back
        }
    }

    else if (direction == "right"){
        mPosX += PLAYER_VEL;
        //If the player went too far to the left or right
        if(mPosX + PLAYER_WIDTH > SCREEN_WIDTH || map[rightblock.y][rightblock.x] == 1){
        // if(mPosX + PLAYER_WIDTH > SCREEN_WIDTH){
            mPosX -= PLAYER_VEL; //Move back
        }
    }

    else if (direction == "up"){
        mPosY -= PLAYER_VEL;
        //If the player went too far to the left or right
        if(mPosY < 0  || map[upblock.y][upblock.x] == 1){
        // if(mPosY < 0){
            mPosY += PLAYER_VEL; //Move back
        }
    }

    else if (direction == "down"){
        mPosY += PLAYER_VEL;
        //If the player went too far to the left or right
        if(mPosY + PLAYER_WIDTH > SCREEN_HEIGHT || map[downblock.y][downblock.x] == 1){
        // if(mPosY + PLAYER_WIDTH > SCREEN_HEIGHT){
            mPosY -= PLAYER_VEL; //Move back
        }
    }
}

#endif
