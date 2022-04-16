#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
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

int x,y,x2,y2,client,server;

bool gamestart = true;
bool gamewait = true;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gFooTexture;
LTexture gFooTexture_2;
LTexture gBackgroundTexture;
LTexture gstartscreen;
LTexture gwaitscreen;

//Game Controller 1 handler
SDL_Joystick* gGameController = NULL;

//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;

void* start(void* arg){
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT ){
						quit = true;
					}
					if( e.type == SDL_KEYDOWN ){
						Client_Keyboard_Handle(e);
					}
					if( e.type == SDL_JOYAXISMOTION ){
						Client_Gamepad_Handle(e);
					}
					if( e.type == SDL_MOUSEBUTTONDOWN ){
						Mouse_Handle(e);
					}
				}
	
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				if(gamestart == true and gamewait == true){
					gstartscreen.render( 0, 0 );
				}
				else if(gamewait == true and gamestart == false){
					gwaitscreen.render( 0, 0 );
				}
				else{
					//Render background texture to screen
					gBackgroundTexture.render( 0, 0 );

					//Render Foo' to the screen
					gFooTexture.render(x,y);

					gFooTexture_2.render(x2,y2);
					//Update screen
				}
				SDL_RenderPresent( gRenderer );
			}
		}

		//Free resources and close SDL
		close();
		pthread_exit(NULL);
	}
}
int main( int argc, char* args[] )
{   
	Client_Connect();
    pthread_t Reciever,Client_Start,checker;
    pthread_create(&Reciever, NULL, &Client_Recieve, NULL);
	pthread_create(&Client_Start, NULL, &start, NULL);
	pthread_create(&checker, NULL, &Client_Check, NULL);

	pthread_join(Reciever, NULL);
	pthread_join(Client_Start, NULL);
	pthread_join(checker, NULL);

	return 0;
}