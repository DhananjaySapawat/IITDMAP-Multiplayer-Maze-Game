#ifndef PLAYER
#define PLAYER

#include <iostream> // for IO
#include <SDL2/SDL.h> // for basic game mechanics
#include <SDL2/SDL_image.h> // for rendering

class Player{
    public:
        //The X and Y offsets of the player
        int mPosX, mPosY;

        //The velocity of the player
        int mVelX, mVelY;

        //The dimensions of the player block
        static const int PLAYER_WIDTH = TILE_SIZE;
        static const int PLAYER_HEIGHT = TILE_SIZE;

        //Maximum axis velocity of the PLAYER
        static const int PLAYER_VEL = TILE_SIZE/5;

        // // Player's hitbox
        // SDL_Rect mCollider;

        //Initializes the variables
        Player(int xTile, int yTile);

        //Takes key presses and adjusts the player's velocity
        void handleEvent( SDL_Event& e );

        //Moves the player
        void move();

        //Moves the player
        void move(SDL_Rect& wall);

        //Shows the player on the screen
        void render();
};

Player::Player(int xTile, int yTile){
    //Initialize the offsets
    mPosX = xTile*TILE_SIZE;
    mPosY = yTile*TILE_SIZE;

    // mCollider.w = TILE_SIZE;
    // mCollider.h = TILE_SIZE;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Player::handleEvent(SDL_Event& e){
    //If a key was pressed
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
        //Adjust the velocity
        switch(e.key.keysym.sym){
            case SDLK_UP: mVelY -= PLAYER_VEL; break;
            case SDLK_DOWN: mVelY += PLAYER_VEL; break;
            case SDLK_LEFT: mVelX -= PLAYER_VEL; break;
            case SDLK_RIGHT: mVelX += PLAYER_VEL; break;
        }
    }
    //If a key was released
    else if(e.type == SDL_KEYUP && e.key.repeat == 0){
        //Adjust the velocity
        switch(e.key.keysym.sym){
            case SDLK_UP: mVelY += PLAYER_VEL; break;
            case SDLK_DOWN: mVelY -= PLAYER_VEL; break;
            case SDLK_LEFT: mVelX += PLAYER_VEL; break;
            case SDLK_RIGHT: mVelX -= PLAYER_VEL; break;
        }
    }
}

bool checkCollision(SDL_Rect a, SDL_Rect b){
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if(bottomA <= topB){
        return false;
    }
    if(topA >= bottomB){
        return false;
    }
    if(rightA <= leftB){
        return false;
    }
    if(leftA >= rightB){
        return false;
    }
    //If none of the sides from A are outside B
    return true;
}


void Player::move(){

    // std::cout << "Current Positon: " << mPosX << ", " << mPosY << "\n\n";

    //Move the player left or right
    mPosX += mVelX;

    //If the player went too far to the left or right
    if((mPosX < 0 ) || (mPosX + PLAYER_WIDTH > SCREEN_WIDTH)){
        //Move back
        mPosX -= mVelX;
    }
    //Move the player up or down
    mPosY += mVelY;

    //If the player went too far up or down
    if( (mPosY < 0) || (mPosY + PLAYER_HEIGHT > SCREEN_HEIGHT)){
        //Move back
        mPosY -= mVelY;
    }
}
/*
void Player::move(SDL_Rect& wall){
    //Move the player left or right
    mPosX += mVelX;
	mCollider.x = mPosX;

    //If the player collided or went too far to the left or right
    if( (mPosX < 0) || (mPosX + TILE_SIZE > SCREEN_WIDTH) || checkCollision(mCollider, wall)){
        //Move back
        std::cout << "x-Collision: " << mPosX << ", " << mPosY << "\n";
        mPosX -= mVelX;
		mCollider.x = mPosX;
    }
    //Move the player up or down
    mPosY += mVelY;
	mCollider.y = mPosY;

    //If the player collided or went too far up or down
    if((mPosY < 0) || (mPosY + TILE_SIZE > SCREEN_HEIGHT) || checkCollision(mCollider, wall)){
        //Move back
        std::cout << "y-Collision: " << mPosX << ", " << mPosY << "\n";
        mPosY -= mVelY;
		mCollider.y = mPosY;
    }
}
*/
#endif
