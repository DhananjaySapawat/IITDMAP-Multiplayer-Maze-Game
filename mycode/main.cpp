/*
    Create a single player game.
    Place obstructions in the window.
    Make a dot move around the window while respecting collisions.
*/

// includes
#include <iostream> // IO
#include <SDL2/SDL.h> // basic game mechanics
#include <SDL2/SDL_image.h> // image rendering
#include <string>
#include "constants.h" // constants like maze structure, window size, etc.
#include "Player.cpp" // player motion

// global variables
SDL_Window* gWindow = NULL; // instantiate the window we'll be rendering to
SDL_Renderer* gRenderer = NULL; // global renderer
SDL_Texture* mapTexture = NULL; // map texture
SDL_Texture* playerTexture = NULL; // player texture

// functions
bool init(){
    /*
        Starts up SDL and creates a window.
    */
    bool success = true; // flag signifying successful execution

    // initialize window
    if (SDL_Init(SDL_INIT_VIDEO)){
        std::cout << "SDL could not initialize! SDL Error:" << SDL_GetError() << "\n";
        success = false;
    }
    else{
        // set texture filtering to linear
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
			std::cout << "Warning: Linear texture filtering not enabled!";
		}

        // assign window
        gWindow = SDL_CreateWindow(
            "Maze", // title
            SDL_WINDOWPOS_UNDEFINED, // x-coordnate
            SDL_WINDOWPOS_UNDEFINED, // y-coordinate
            SCREEN_WIDTH, // width
            SCREEN_HEIGHT, // height
            SDL_WINDOW_SHOWN // flag: always display the window when updated
        );
        if(gWindow == NULL){
			std::cout << "Window could not be created! SDL Error:" << SDL_GetError() << "\n";
			success = false;
		}
        else{
            // assign renderer
            gRenderer = SDL_CreateRenderer(
                gWindow, // SDL_Window
                -1, // index
                SDL_RENDERER_ACCELERATED // flags
            );
			
            if(gRenderer == NULL){
				std::cout << "Renderer could not be created! SDL Error:" << SDL_GetError() << "\n";
				success = false;
			}
            else{
                // set a default rendering color
                SDL_SetRenderDrawColor(
                    gRenderer, // SDL_Renderer
                    0x00, 0x00, 0x00, 0xFF // RGBA
                );

                // initialize png loading
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags)){
					std::cout << "SDL_image could not initialize! SDL_image Error:" << IMG_GetError() << "\n";
					success = false;
				}
            }
        }
    }
    return success;
}

SDL_Texture* loadTexture(std::string path){
	// declare a new texture
	SDL_Texture* newTexture = NULL;

	//Load image into surface
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if(loadedSurface == NULL){
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else{
		// Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if(newTexture == NULL){
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		// Free the surface. Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}
    // return newTexture
	return newTexture;
}

bool loadMedia(){
	// Loading success flag
	bool success = true;

	// Load PNG texture
	mapTexture = loadTexture("../../data/tile.png");
	playerTexture = loadTexture("../../data/red-block.png");

	if(mapTexture == NULL || playerTexture == NULL){
		printf("Failed to load texture image!\n");
		success = false;
	}

	return success;
}

void close(){
	//Free loaded image
	SDL_DestroyTexture(mapTexture);
	mapTexture = NULL;
	SDL_DestroyTexture(playerTexture);
	playerTexture = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

// main function
int main(int argc, char* args[]){

	//Start up SDL and create window
	if(!init()){
		printf( "Failed to initialize!\n" );
	}
	else{
		// Load media
		if(!loadMedia()){
			printf( "Failed to load media!\n" );
		}
		else{	
			// Main loop flag
			bool quit = false;

			// Event handler
			SDL_Event e;

			// Player
			Player player;

			// While application is running
			while(!quit){
				// Handle events on queue
				while(SDL_PollEvent(&e) != 0){
					// User requests quit
					if(e.type == SDL_QUIT){
						quit = true;
					}
					player.handleEvent(e);
				}

				// possible collisions
				int x, y;
                for (y = -1 + player.mPosY / TILE_SIZE; y < 1 + player.mPosY / TILE_SIZE; y++){
                    for (x = -1 + player.mPosX / TILE_SIZE; x < 1 + player.mPosX / TILE_SIZE; x++){
                        if (map[y][x]){
                            SDL_Rect wall;
                            wall.x = TILE_SIZE * x;
                            wall.y = TILE_SIZE * y;
							wall.w = TILE_SIZE;
							wall.h = TILE_SIZE;

							// move the player
							player.move(wall);
						}
					}
				}

				// // move the player
				// player.move();

				// Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(gRenderer);

                // Create viewports
                int i, j;
                for (i = 0; i < SCREEN_HEIGHT / TILE_SIZE; i++){
                    for (j = 0; j < SCREEN_WIDTH / TILE_SIZE; j++){
                        if (map[i][j]) {
                            SDL_Rect block;
                            block.x = TILE_SIZE * j;
                            block.y = TILE_SIZE * i;
							block.w = TILE_SIZE;
							block.h = TILE_SIZE;
                            SDL_RenderSetViewport(gRenderer, &block);
                            SDL_RenderCopy(gRenderer, mapTexture, NULL, NULL);
                        }
                    }
                }

				SDL_Rect renderQuad;
				renderQuad.x = player.mPosX;
				renderQuad.y = player.mPosY,
				renderQuad.w = player.PLAYER_WIDTH,
				renderQuad.h = player.PLAYER_HEIGHT;
				SDL_RenderSetViewport(gRenderer, &renderQuad);
				SDL_RenderCopy(gRenderer, playerTexture, NULL, NULL);

				// Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	// Free resources and close SDL
	close();
	return 0;
}
