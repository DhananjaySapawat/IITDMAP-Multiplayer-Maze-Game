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
#include<climits>

#include "constants.h"
#include "Player.cpp"
using namespace std;

/*-----CONSTANTS & VARIABLES-----*/

extern int x, y, x2, y2, sx, sy, sh, sw, client, server, si;
extern bool gamestart;
extern bool gamepart[4];
extern bool Player2;
extern bool quit;
double g = 0.1;
double t = g;
int array_sx[4][4] = {{490,205,200,80}, {430,285,320,80}, {370,365,440,80}, {500,445,190,80}};

//The window we'll be rendering to
extern SDL_Window* gWindow ;

//The window renderer
extern SDL_Renderer* gRenderer;

// map texture
SDL_Texture* mapTexture = NULL;

// player texture
SDL_Texture* playerTexture = NULL;

//Game Controller 1 handler
extern SDL_Joystick* gGameController;

//The music that will be played
extern Mix_Music *gMusic ;

//The sound effects that will be used
extern Mix_Chunk *gScratch,*gHigh,*gMedium,*gLow;

const int JOYSTICK_DEAD_ZONE = 8000;
// const int SCREEN_WIDTH = 1200;
// const int SCREEN_HEIGHT = 650;

int buffer_size = 1024;
int Server_Port = 5476;

enum LButtonSprite{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3,
	BUTTON_SPRITE_TOTAL = 4
};

/*-----CLASS-----*/

class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile(std::string path,bool backscreen);

		//Deallocates texture
		void free();

		//Renders texture at given point
		void render( int x, int y );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path,bool backscreen)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			if(backscreen){	
				mWidth = SCREEN_WIDTH;
				mHeight = SCREEN_HEIGHT ;
			}
			else{
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render(int x, int y)
{
	//Set rendering space and render to screen
	// std::cout << "Rendered at :" << x << ", " << y << "\n";
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

/*-----CLASS-----*/

class Ltext
{	
	public:
		bool Text_init(string f , string t , SDL_Color color , int s);
		void render( int x , int y);
		void free();
	private:
		SDL_Surface* text;
		SDL_Texture* text_texture;
		SDL_Renderer* renderer;
		TTF_Font* font;
};

bool Ltext::Text_init(string f, string t , SDL_Color color , int s){

	// string fon = "/usr/share/fonts/truetype/"+f;
	const char *fc = f.c_str();
	font = TTF_OpenFont(fc, s);
	if (font == NULL) {
		cout << "Error loading font: " << TTF_GetError() << endl;
		return false;
	}
	const char *tc = t.c_str();
	text = TTF_RenderText_Solid( font, tc, {color });
	if ( !text ) {
		cout << "Failed to render text: " << TTF_GetError() << endl;
		return false;
	}

	return true;
}

void Ltext::render( int x , int y ){

	text_texture = SDL_CreateTextureFromSurface( gRenderer, text );

	SDL_Rect dest = { x, y, text->w, text->h };

	SDL_RenderCopy( gRenderer, text_texture , NULL , &dest );
}

void Ltext::free(){
	SDL_DestroyTexture( text_texture );
	SDL_FreeSurface( text );
}

//Scene textures
extern LTexture gFooTexture,gFooTexture_2,gBackgroundTexture,gstartscreen,gwaitscreen,gstartcontrol,gstartinstruction;

//Scene Text
extern Ltext Screen_Start,Screen_Controls,Screen_Instructions,Screen_Quit;

/*-----FUNCTIONS-----*/

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

int check(int a ,int b){
	for(int i = 0;i<4;i++){
		if(a >= array_sx[i][0] and a<=array_sx[i][0]+array_sx[i][2] and b>=array_sx[i][1] and b<=array_sx[i][1]+array_sx[i][3]){
			return i;
		}
	}
	return -1;
}

int Give_Value(string s){
	int a = stoi(s.substr(1));
	return a;
}

void* Server_Check(void* arg){
	std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    int count = 0;
    string s;
	while(1){
		if(quit){
			 exit(0);
		}
		end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if(elapsed_seconds.count() > t){
        	t = t + g;
        	count++;
        	if(count%2 ==0 ){
        		s = "x" + to_string(x2);
        	}
        	else{
        		s = "y" + to_string(y2);
        	}
    		char* msg = &s[0];
    		send(client,msg,sizeof(msg),0);
        }
    }
    pthread_exit(NULL);
}

void* Client_Check(void* arg){
	std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    int count = 0;
    string s;
	while(1){
		if(quit){
			 exit(0);
		}
		end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if(elapsed_seconds.count() > t){
        	t = t + g;
        	count++;
        	if(count%2 ==0 ){
        		s = "x" + to_string(x);
        	}
        	else{
        		s = "y" + to_string(y);
        	}
    		char* msg = &s[0];
    		send(server,msg,sizeof(msg),0);
        }
    }
    pthread_exit(NULL);
}

void* Server_Recieve(void* arg){
	int buffer_size = 1024;
	char buffer[buffer_size] = {0};
	while(1){
		int Client_Read;
		Client_Read = read(client , buffer, buffer_size );
    	if(buffer[0] == 'r'){
    		x = x + 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'l'){
    		x = x - 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'u'){
    		y = y - 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'd'){
    		y = y + 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 's'){
    		Player2 = true;
    		if(gamepart[0]){
    			char msg[] = "start";
				send(client, msg , sizeof(msg) , 0 );
    		}
    	}
    	if(buffer[0] == 'x'){
    		string s = buffer;
    		x = Give_Value(s);
    		// cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'y'){
    		string s = buffer;
    		y = Give_Value(s);
    		// cout<<buffer<<endl;
    	}
    	
    }
    pthread_exit(NULL);
}

void* Client_Recieve(void* arg){
	int buffer_size = 1024;
	char buffer[buffer_size] = {0};
	while(1){
		int Server_Read;
		Server_Read = read(server , buffer, buffer_size );
		if(buffer[0] == 'r'){
    		x2 = x2 + 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'l'){
    		x2 = x2 - 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'u'){
    		y2 = y2 - 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'd'){
    		y2 = y2 + 10;
    		//cout<<buffer<<endl;
    	}
    	if(buffer[0] == 's'){
    		Player2 = true;
    		if(gamepart[0]){
    			char msg[] = "start";
				send(server, msg , sizeof(msg) , 0 );
    		}
    	}
    	if(buffer[0] == 'x'){
    		string s = buffer;
    		x2 = Give_Value(s);
    		// cout<<buffer<<endl;
    	}
    	if(buffer[0] == 'y'){
    		string s = buffer;
    		y2 = Give_Value(s);
    		// cout<<buffer<<endl;
    	}
    }
    pthread_exit(NULL);
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				// Initialize SDL_ttf
				if ( TTF_Init() < 0 ) {
					cout << "Error intializing SDL_ttf: " << TTF_GetError() << endl;
					return false;
				}
				//Initialize SDL_mixer
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
			}
			//Check for joysticks
        	if( SDL_NumJoysticks() < 1 ){
            	printf( "Warning: No joysticks connected!\n" );
        	}
        	else{
            	//Load joystick
            	gGameController = SDL_JoystickOpen( 0 );
            	if( gGameController == NULL ){
                	printf( "Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
            	}
        	}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load Foo' texture
	if( !gFooTexture.loadFromFile("./data/undertale.png",false ) )
	{
		printf( "Failed to load Foo' texture image!\n" );
		success = false;
	}
	
	if( !gFooTexture_2.loadFromFile("./data/undertale.png",false ) )
	{
		printf( "Failed to load Foo' texture image!\n" );
		success = false;
	}
	
	//Load background texture
	mapTexture = loadTexture("./data/tile.png");
	playerTexture = loadTexture("./data/undertale.png");
	if (mapTexture == NULL || mapTexture == NULL){
	// if( !gBackgroundTexture.loadFromFile("background.png",true ) )
		printf( "Failed to load background texture image!\n" );
		success = false;
	}
	
	if ( !gstartscreen.loadFromFile("back.png",true ) )
	{
		printf( "Failed to load background start image!\n" );
		success = false;
	}
	
	if ( !gwaitscreen.loadFromFile("wait.png",true ) )
	{
		printf( "Failed to load background start image!\n" );
		success = false;
	}
	
	if ( !gstartcontrol.loadFromFile("control.png",true ))
	{
		printf( "Failed to load background start image!\n" );
		success = false;
	}

	if ( !gstartinstruction.loadFromFile("ins.png",true ))
	{
		printf( "Failed to load background start image!\n" );
		success = false;
	}

	//Load Text
	if ( !Screen_Start.Text_init("PublicPixel-0W5Kv.ttf","Start",{255,255,255},80) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}

	if ( !Screen_Controls.Text_init("PublicPixel-0W5Kv.ttf","Controls",{255,255,255},80) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}

	if ( !Screen_Instructions.Text_init("PublicPixel-0W5Kv.ttf","Instructions",{255,255,255},80) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}

	if ( !Screen_Quit.Text_init("PublicPixel-0W5Kv.ttf","Quit",{255,255,255},80) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}
	//Load music
	gMusic = Mix_LoadMUS( "beat.wav" );
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	//Load sound effects
	gScratch = Mix_LoadWAV( "scratch.wav" );
	if( gScratch == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	gHigh = Mix_LoadWAV( "high.wav" );
	if( gHigh == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gMedium = Mix_LoadWAV( "medium.wav" );
	if( gMedium == NULL )
	{
		printf( "Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gLow = Mix_LoadWAV( "low.wav" );
	if( gLow == NULL )
	{
		printf( "Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	return success;
}

void close()
{
	//Free loaded images
	gFooTexture.free();
	// gBackgroundTexture.free();
	gstartscreen.free();
    gwaitscreen.free();

	SDL_DestroyTexture(mapTexture);
	mapTexture = NULL;
	SDL_DestroyTexture(playerTexture);
	playerTexture = NULL;

    //free text
    Screen_Start.free();
   	Screen_Controls.free();
	Screen_Instructions.free();
	Screen_Quit.free();


	//Close game controller
    SDL_JoystickClose( gGameController );
    gGameController = NULL;

    //Free the sound effects
    Mix_FreeChunk( gScratch );
    Mix_FreeChunk( gHigh );
    Mix_FreeChunk( gMedium );
    Mix_FreeChunk( gLow );
    gScratch = NULL;
    gHigh = NULL;
    gMedium = NULL;
    gLow = NULL;
    
    //Free the music
    Mix_FreeMusic( gMusic );
    gMusic = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Keyboard_Start_Screen(SDL_Event e , int c){
	if(gamepart[1]){
		if(e.key.keysym.sym == 	SDLK_BACKSPACE){
			gamepart[1] = false;
		}
		return;
	}
	if(gamepart[2]){
		if(e.key.keysym.sym == 	SDLK_BACKSPACE){
			gamepart[2] = false;
		}
		return;
	}
	if(e.key.keysym.sym == 	SDLK_KP_ENTER or e.key.keysym.sym == SDLK_RETURN){
		if(si == 0){
			gamestart = false;
			char msg[] = "start";
			send(c, msg , sizeof(msg) , 0 );
		}
		gamepart[si] = true;
		return;
	}                  
	else if(e.key.keysym.sym == SDLK_UP){
		if(si != 0){
			si--;
		}
	}
	else if(e.key.keysym.sym == SDLK_DOWN){
		if(si != 3){
			si++;
		}
	}
	sx = array_sx[si][0];
	sy = array_sx[si][1];
	sw = array_sx[si][2];
	sh = array_sx[si][3];
	return ;
}

void Server_Keyboard_Handle(SDL_Event e){
	if(gamestart){
		Keyboard_Start_Screen(e, client);
		return ;
	}
	switch(e.key.keysym.sym){
                     
		case SDLK_RIGHT:{
			char msg[] = "right";
			send(client, msg , sizeof(msg) , 0);
			x2 = x2 + 10;
			break;
		}	

		case SDLK_LEFT:{
			char msg[] = "left";
			send(client, msg , sizeof(msg) , 0);
			x2 = x2 - 10;
			break;
		}

		case SDLK_UP:{
			char msg[] = "up";
			send(client, msg , sizeof(msg) , 0);
			y2 = y2 - 10;
			break;
		}

		case SDLK_DOWN:{
			char msg[] = "down";
			send(client, msg , sizeof(msg) , 0 );
			y2 = y2 + 10;
			break;
		}

		//Play high sound effect
    	case SDLK_1:{
			Mix_PlayChannel( -1, gHigh, 0 );
			break;
		}
                            
    	//Play medium sound effect
    	case SDLK_2:{
			Mix_PlayChannel( -1, gMedium, 0 );
			break;
		}
                            
    	//Play low sound effect
    	case SDLK_3:{
			Mix_PlayChannel( -1, gLow, 0 );
			break;
		}
                            
    	//Play scratch sound effect
    	case SDLK_4:{
			Mix_PlayChannel( -1, gScratch, 0 );
			break;
		}

    	case SDLK_9:{
			//If there is no music playing
			if( Mix_PlayingMusic() == 0 ){
				//Play the music
				Mix_PlayMusic( gMusic, -1 );
			}
			//If music is being played
			else{
				//If the music is paused
				if( Mix_PausedMusic() == 1 ){
					//Resume the music
					Mix_ResumeMusic();
				}
				//If the music is playing
				else{
					//Pause the music
					Mix_PauseMusic();
				}
			}
			break;
		}
    
		case SDLK_0:{
			//Stop the music
			Mix_HaltMusic();
			break;
		}
    }
    return;
}
/*
void Server_Keyboard_Handle(SDL_Event e, Player player){
	if(gamestart){
		Keyboard_Start_Screen(e, client);
		return ;
	}
	switch(e.key.keysym.sym){
                     
		case SDLK_RIGHT:{
			char msg[] = "right";
			send(client, msg , sizeof(msg) , 0);
			x2 = x2 + 10;
			break;
		}	

		case SDLK_LEFT:{
			char msg[] = "left";
			send(client, msg , sizeof(msg) , 0);
			x2 = x2 - 10;
			break;
		}

		case SDLK_UP:{
			char msg[] = "up";
			send(client, msg , sizeof(msg) , 0);
			y2 = y2 - 10;
			break;
		}

		case SDLK_DOWN:{
			char msg[] = "down";
			send(client, msg , sizeof(msg) , 0 );
			y2 = y2 + 10;
			break;
		}

		//Play high sound effect
    	case SDLK_1:{
			Mix_PlayChannel( -1, gHigh, 0 );
			break;
		}
                            
    	//Play medium sound effect
    	case SDLK_2:{
			Mix_PlayChannel( -1, gMedium, 0 );
			break;
		}
                            
    	//Play low sound effect
    	case SDLK_3:{
			Mix_PlayChannel( -1, gLow, 0 );
			break;
		}
                            
    	//Play scratch sound effect
    	case SDLK_4:{
			Mix_PlayChannel( -1, gScratch, 0 );
			break;
		}

    	case SDLK_9:{
			//If there is no music playing
			if( Mix_PlayingMusic() == 0 ){
				//Play the music
				Mix_PlayMusic( gMusic, -1 );
			}
			//If music is being played
			else{
				//If the music is paused
				if( Mix_PausedMusic() == 1 ){
					//Resume the music
					Mix_ResumeMusic();
				}
				//If the music is playing
				else{
					//Pause the music
					Mix_PauseMusic();
				}
			}
			break;
		}
    
		case SDLK_0:{
			//Stop the music
			Mix_HaltMusic();
			break;
		}
    }
    return;
}
*/
void Client_Keyboard_Handle(SDL_Event e){
	if(gamestart){
		Keyboard_Start_Screen(e,server);
		return ;
	}
	switch(e.key.keysym.sym){
                     
		case SDLK_RIGHT:
		{
		char msg[] = "right";
		send(server, msg , sizeof(msg) , 0 );
		x = x + 10;
		break;
		}	

		case SDLK_LEFT:
		{
		char msg[] = "left";
		send(server, msg , sizeof(msg) , 0 );
		x = x - 10;
		break;
		}

		case SDLK_UP:
		{
		char msg[] = "up";
		send(server, msg , sizeof(msg) , 0 );
		y = y - 10;
		break;
		}

		case SDLK_DOWN:
		{
		char msg[] = "down";
		send(server, msg , sizeof(msg) , 0 );
		y = y + 10;
		break;
		}

		//Play high sound effect
    	case SDLK_1:
    	{
    	Mix_PlayChannel( -1, gHigh, 0 );
    	break;
		}
                            
    	//Play medium sound effect
    	case SDLK_2:
    	{
    	Mix_PlayChannel( -1, gMedium, 0 );
    	break;
		}
                            
    	//Play low sound effect
    	case SDLK_3:
    	{
    	Mix_PlayChannel( -1, gLow, 0 );
    	break;
		}
                            
    	//Play scratch sound effect
    	case SDLK_4:
    	{
    	Mix_PlayChannel( -1, gScratch, 0 );
    	break;
		}

    	case SDLK_9:{
    	//If there is no music playing
		if( Mix_PlayingMusic() == 0 ){
			//Play the music
			Mix_PlayMusic( gMusic, -1 );
		}
		//If music is being played
		else{
			//If the music is paused
			if( Mix_PausedMusic() == 1 ){
				//Resume the music
				Mix_ResumeMusic();
       	    }
			//If the music is playing
			else{
				//Pause the music
				Mix_PauseMusic();
			}
		}
		break;
		}
                            
		case SDLK_0:
		{
		//Stop the music
		Mix_HaltMusic();
		break;
		}
    }
    return ;
}

void Server_Gamepad_Handle(SDL_Event e){
	//Motion on controller 0
    if( e.jaxis.which == 0 ){                        
        //X axis motion
        if( e.jaxis.axis == 0 ){
            //Left of dead zone
            if( e.jaxis.value < -JOYSTICK_DEAD_ZONE ){
                char msg[] = "left";
		        send(client, msg , sizeof(msg) , 0 );
		        x2 = x2 - 10;
            }
            //Right of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "right";
				send(client, msg , sizeof(msg) , 0 );
				x2 = x2 + 10;
            }
        }
        //Y axis motion
        else if( e.jaxis.axis == 1 ){
            //Below of dead zone
            if( e.jaxis.value < -JOYSTICK_DEAD_ZONE ){
                char msg[] = "up";
	            send(client, msg , sizeof(msg) , 0 );
	            y2 = y2 - 10;
            }
            //Above of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "down";
			    send(client, msg , sizeof(msg) , 0 );
				y2 = y2 + 10;
            }
        }
    }
}

void Client_Gamepad_Handle(SDL_Event e){
	//Motion on controller 0
    if( e.jaxis.which == 0 ){                        
        //X axis motion
        if( e.jaxis.axis == 0 ){
            //Left of dead zone
            if( e.jaxis.value < -JOYSTICK_DEAD_ZONE ){
                char msg[] = "left";
		        send(server, msg , sizeof(msg) , 0 );
		        x = x - 10;
            }
            //Right of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "right";
				send(server, msg , sizeof(msg) , 0 );
				x = x + 10;
            }
        }
        //Y axis motion
        else if( e.jaxis.axis == 1 ){
            //Below of dead zone
            if( e.jaxis.value < -JOYSTICK_DEAD_ZONE ){
                char msg[] = "up";
	            send(server, msg , sizeof(msg) , 0 );
	            y = y - 10;
            }
            //Above of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "down";
			    send(server, msg , sizeof(msg) , 0 );
				y = y + 10;
            }
        }
    }
}

void Mouse_Handle(SDL_Event e , int c){
	//Get mouse position
	int mx, my;
	SDL_GetMouseState(&mx,&my);
	int p = check(mx,my);
	if(e.type == SDL_MOUSEBUTTONDOWN ){
		if(p == 0){
			gamestart = false;
			gamepart[0] = true;
		}
		else if (p!=-1){
			gamepart[p] = true;
		}
	}
	else{
		if(p!=-1){
			sx = array_sx[p][0];
			sy = array_sx[p][1];
			sw = array_sx[p][2];
			sh = array_sx[p][3];
		}
	}
}

void Server_Connect(){
	x = 240,y = 190,x2 = 360,y2 = 190;
	sx = 500 ,sy = 205 , sw = 180 , sh = 80 , si = 0;
	struct sockaddr_in Server_Address;
	char buffer[buffer_size] = {0};
	server = socket ( AF_INET, SOCK_STREAM, 0);
	if (server < 0){
		cout<<"Opening of Socket Failed !"<<endl;
		exit ( EXIT_FAILURE);
	}
	cout<<"Server Socket connection created"<<endl;

	Server_Address.sin_family = AF_INET;
	Server_Address.sin_port = htons(Server_Port);
    Server_Address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int Server_Address_Size = sizeof(Server_Address);
    int Server_Bind = bind(server,(struct sockaddr*)&Server_Address,Server_Address_Size);
    if(Server_Bind < 0){
		cout<<"Socket Binding Failed !"<<endl;
		exit ( EXIT_FAILURE);
	}
	cout<<"Server Socket Bind"<<endl;

	listen(server,3);
	
	client = accept(server, (struct sockaddr *)&Server_Address,(socklen_t*)&Server_Address_Size);

	if(client < 0){
		cout<<"Client not connect !"<<endl;
		exit ( EXIT_FAILURE);
	}
	cout<<"Client connected !"<<endl;
}

void Client_Connect(){
	x = 240,y = 190,x2 = 360,y2 = 190;
	sx = 500 ,sy = 205 , sw = 180 , sh = 80 ,si =0 ;
 	struct sockaddr_in Server_Address;
	char buffer[buffer_size] = {0};

	server = socket ( AF_INET, SOCK_STREAM, 0);
	if (server < 0){
		cout<<"Opening of Socket Failed !"<<endl;
		exit ( EXIT_FAILURE);
	}
	cout<<"Client Socket created"<<endl;

	Server_Address.sin_family = AF_INET;
	Server_Address.sin_port = htons(Server_Port);

	if(inet_pton(AF_INET, "127.0.0.1", &Server_Address.sin_addr)<=0) 
    {
        printf("Invalid address/ Address not supported \n");
        exit ( EXIT_FAILURE);
    }
    
    int Server_Address_Size = sizeof(Server_Address);
	int Server_Connector = connect(server, (struct sockaddr *)&Server_Address, Server_Address_Size);

	if (Server_Connector < 0){
		cout<<"Client connection Failed !"<<endl;
		exit ( EXIT_FAILURE);
	}
	cout<<"Client Server connected"<<endl;
}
