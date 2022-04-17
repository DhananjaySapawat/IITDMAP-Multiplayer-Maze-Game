#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_mixer.h>
#include<SDL2/SDL_ttf.h>
#include<iostream>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<pthread.h>

#include"SDL_Helper.h"

using namespace std;

/*-----CONSTANTS & VARIABLES-----*/

int x, y, x2, y2, sx, sy, sh, sw, client, server, si;
bool gamestart = true;
bool gamepart[4] = {false};
bool Player2 = false;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gFooTexture;
LTexture gFooTexture_2;
// LTexture gBackgroundTexture;
LTexture gstartscreen;
LTexture gwaitscreen;
LTexture gstartcontrol;
LTexture gstartinstruction;

//scene text
Ltext Screen_Start;
Ltext Screen_Controls;
Ltext Screen_Instructions;
Ltext Screen_Quit;

//Game Controller 1 handler
SDL_Joystick* gGameController = NULL;

//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;

//Main loop flag
bool quit = false;

/*-----FUNCTIONS-----*/

void* start(void* arg){
	if( !init() ){
		printf( "Failed to initialize!\n" );
	}
	else{
		//Load media
		if( !loadMedia() ){
			printf( "Failed to load media!\n" );
		}
		else{	
			//Event handler
			SDL_Event e;

			// Player
			Player server_player;

			//While application is running
			while( !quit ){
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 ){
					//User requests quit
					if( e.type == SDL_QUIT ){
						quit = true;
					}
					if( e.type == SDL_KEYDOWN ){
						Server_Keyboard_Handle(e);
					}
					if( e.type == SDL_JOYAXISMOTION ){
						Server_Gamepad_Handle(e);
					}
					if( e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION ){
						Mouse_Handle(e,client);
					}
				}
	
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
				SDL_RenderClear( gRenderer );

				if(gamestart == true){
					if(gamepart[1] == true){
						gstartcontrol.render(0,0);
					}
					else if (gamepart[2] == true){
						gstartinstruction.render(0,0);
					}
					else if(gamepart[3] == true){
						quit = true;
					}
					else{
						gstartscreen.render(0,0);
						Screen_Start.render(510,170);
						Screen_Controls.render(450,250);
						Screen_Instructions.render(390,330);
						Screen_Quit.render(520,410);
						for(int i = 0;i<5;i++){
							SDL_Rect outlineRect = {sx-i,sy-i,sw+2*i,sh+2*i};
                			SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );        
                			SDL_RenderDrawRect( gRenderer, &outlineRect );
                		}
                	}
				}
				else if(gamepart[0] == true){
					if(!Player2){
						gwaitscreen.render( 0, 0 );
						char msg[] = "start";
						send(client, msg , sizeof(msg) , 0 );
					}
					else{
						//Render background texture to screen
						// gBackgroundTexture.render( 0, 0 );

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

						// render the player
						SDL_Rect renderQuad;
						renderQuad.x = server_player.mPosX;
						renderQuad.y = server_player.mPosY,
						renderQuad.w = server_player.PLAYER_WIDTH,
						renderQuad.h = server_player.PLAYER_HEIGHT;
						SDL_RenderSetViewport(gRenderer, &renderQuad);
						SDL_RenderCopy(gRenderer, server_playerTexture, NULL, NULL);

						//Render Foos to the screen
						// gFooTexture.render(x,y);
						// gFooTexture_2.render(x2,y2);
					}
				}
				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}

		//Free resources and close SDL
		close();
		pthread_exit(NULL);
	}
}

int main( int argc, char* args[] ){
	Server_Connect();
    pthread_t Reciever,Server_Start,checker;
    pthread_create(&Reciever, NULL, &Server_Recieve, NULL);
    pthread_create(&Server_Start, NULL, &start, NULL);
    pthread_create(&checker, NULL, &Server_Check, NULL);	
	pthread_join(Server_Start, NULL);
    pthread_join(Reciever, NULL);
    pthread_join(checker, NULL);
	return 0;
}
