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
#include<chrono>
#include<iomanip>
#include<sstream>

#include"SDL_Helper.h"
using namespace std;

int x,y,x2,y2,sx,sy,sh,sw,client,server,si;
double MatchTime = GameTime;
bool getname = true;
bool gamestart = true;
bool gamepart[4] = {false};
bool Player2 = false;
bool GameOver = false;
double CountTime = GameTime-1;

//Player Name
string name = "";
string name2;

string IP_Address;

string TimeStr = to_string(GameTime);

// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
// LTexture gFooTexture;
// LTexture gFooTexture_2;
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
Ltext Screen_Wait1;
Ltext Screen_Wait2;
Ltext Screen_GetName;
Ltext Screen_Time;
Ltext Screen_Space;
Ltext Screen_Winner;
Ltext Screen_WinnerName;


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
	std::chrono::time_point<std::chrono::system_clock> TimeStart,TimeEnd;
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

			//While application is running
			while( !quit ){
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 ){
					//User requests quit
					if( e.type == SDL_QUIT ){
						quit = true;
					}
					if( e.type == SDL_KEYDOWN || e.type == SDL_KEYUP ){
						Server_Keyboard_Handle(e);
					}
					if( e.type == SDL_JOYAXISMOTION ){
						Server_Gamepad_Handle(e);
					}
					if( e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION ){
						Mouse_Handle(e,client);
					}
				}

				// move the players
				server_player.move();
				client_player.move();
	
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
				SDL_RenderClear( gRenderer );

				if(GameOver){
					gstartscreen.render(0,0);
					Screen_Space.mid = true;
					Screen_WinnerName.mid = true;
					Screen_Winner.mid = true;
					Screen_Space.render(180,360);
					Screen_WinnerName.render(460,200);
					Screen_Winner.render(100,50);
				}
				else if(getname == true){
					gstartscreen.render(0,0);
					Screen_GetName.render(100,270);
				}
				else if(gamestart == true ){
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
                	TimeStart = std::chrono::system_clock::now();

				}
				else if(gamepart[0] == true){
					if(!Player2){
						gstartscreen.render(0,0);
						Screen_Wait1.render(180,160);
						Screen_Wait2.render(425,260);
						char msg[] = "start";
						send(client, msg , sizeof(msg) , 0 );
						TimeStart = std::chrono::system_clock::now();
					}
					else{
						// always render maze prior to everything
						int i, j;
						for (i = 0; i < SCREEN_HEIGHT / TILE_SIZE; i++){
							for (j = 0; j < SCREEN_WIDTH / TILE_SIZE; j++){
								if (map[i][j]) {
									mapTexture.setWidth(TILE_SIZE);
									mapTexture.setHeight(TILE_SIZE);
									mapTexture.render(TILE_SIZE * j, TILE_SIZE * i);
								}
							}
						}

						// render the players
						server_playerTexture.setWidth(server_player.PLAYER_WIDTH);
						server_playerTexture.setHeight(server_player.PLAYER_HEIGHT);
						server_playerTexture.render(server_player.mPosX, server_player.mPosY);

						client_playerTexture.setWidth(client_player.PLAYER_WIDTH);
						client_playerTexture.setHeight(client_player.PLAYER_HEIGHT);
						client_playerTexture.render(client_player.mPosX, client_player.mPosY);

						// render timer
						TimeEnd = std::chrono::system_clock::now();
        				std::chrono::duration<double> elapsed_seconds = TimeEnd - TimeStart;
        				MatchTime = GameTime - elapsed_seconds.count();
        				//cout<<MatchTime<<" "<<CountTime<<endl;
        				if(MatchTime <= CountTime){
						if(CountTime == GameTime-1){
        						Mix_PlayMusic( gMusic, -1 );
        					}
        					std::stringstream stream;
							stream << std::fixed << std::setprecision(1) << CountTime;
							TimeStr = stream.str();
							cout<<TimeStr<<endl;
        					Screen_Time.Text_init(myfont,TimeStr,{255,255,255},25);
							Screen_Time.render(SCREEN_WIDTH - 6 * TILE_SIZE, SCREEN_HEIGHT - 2 * TILE_SIZE);
        					CountTime = CountTime - 1;
        					if(0>=CountTime){
        						GameOver = true;
        						Screen_Winner.Text_init(myfont,name,{255,255,255},85);
							Mix_PauseMusic();
        					}
        				}
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
	IP_Address = "127.0.0.1";
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
