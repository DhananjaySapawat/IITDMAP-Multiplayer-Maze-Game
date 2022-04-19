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
#include"constants.h"
#include"Player.cpp"
using namespace std;

extern int x,y,x2,y2,sx,sy,sh,sw,client,server,si,nm;
extern bool gamepart[4];
extern bool Player2,gamestart,quit,getname,GameOver;
bool backhover = false;
double g = 0.1;
double t = g;
double GameTime = 30.00;
extern double MatchTime,CountTime;
int startfontsize = 40;
int array_sx[4][4] = {{490,155,235,75},{430,235,360,80},{380,315,500,80},{500,395,190,75}};
extern string IP_Address,TimeStr,name,name2;
string myfont = "PublicPixel-0W5Kv.ttf";

//The window we'll be rendering to
extern SDL_Window* gWindow ;

//The window renderer
extern SDL_Renderer* gRenderer;

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

//Players
Player server_player(1,1);
Player client_player(1,4);

/*-----CLASS-----*/

void Keyboard_Start_Screen(SDL_Event e , int c);
void Keyboard_Get_Name(SDL_Event e , int c);

bool checkback(int a ,int b){
	if(a >= 990 and a<=1145 and b>=25 and b<=90){
			return true;
	}
	return false;
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

		//Set image dimensions
		void setWidth(int width);
		void setHeight(int height);

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

void LTexture::setWidth(int width){
	mWidth = width;
}

void LTexture::setHeight(int height){
	mHeight = height;
}

/*-----CLASS-----*/

class Ltext
{	
	public:
		bool Text_init(string f , string t , SDL_Color color , int s);
		void render( int x , int y);
		void free();
		bool mid = false;
	private:
		SDL_Surface* text;
		SDL_Texture* text_texture;
		SDL_Renderer* renderer;
		TTF_Font* font;
};

bool Ltext::Text_init(string f, string t , SDL_Color color , int s){

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

void Ltext::render( int x , int y){
	if(mid){
		x = (1200 - text->w)/2;
	}
	text_texture = SDL_CreateTextureFromSurface( gRenderer, text );

	SDL_Rect dest = { x, y, text->w, text->h };


	SDL_RenderCopy( gRenderer, text_texture , NULL , &dest );
}

void Ltext::free(){
	SDL_DestroyTexture( text_texture );
	SDL_FreeSurface( text );
}

//Scene textures
extern LTexture gstartscreen,gstartcontrol,gstartinstruction;
//Scene Text
extern Ltext Screen_Start,Screen_Controls,Screen_Instructions,Screen_Quit,Screen_Wait1,Screen_Wait2;
extern Ltext Screen_GetName,Screen_Time,Screen_Space,Screen_Winner,Screen_WinnerName;
extern Ltext Screen_Back,Screen_Keyboard,Screen_Gamepad;

/*-----GLOBAL-TEXTURES-----*/

// map texture
// SDL_Texture* mapTexture = NULL;
LTexture mapTexture;

// server-player texture
// SDL_Texture* server_playerTexture = NULL;
LTexture server_playerTexture;

// client-player texture
// SDL_Texture* client_playerTexture = NULL;
LTexture client_playerTexture;

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

void* Server_Check(void* arg){
	std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    int count = 0;
    string s;
	while(1){
		if(quit){
			 exit(0);
		}
		if(gamestart){
			string magname = "n"+name;
			char* msg = &magname[0];
			send(client,msg,sizeof(msg),0);
			continue;
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
		if(gamestart){
			string magname = "n"+name;
			char* msg = &magname[0];
			send(server,msg,sizeof(msg),0);
			continue;
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
    	
		if(buffer[0] == 'l' || buffer[0] == 'r' || buffer[0] == 'u' || buffer[0] == 'd'){
    		client_player.move(buffer);
    	}
    	if(buffer[0] == 'n'){
    		name2 = buffer;
    		name2 = name2.substr(1);
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
    	if(buffer[0] == 's'){
    		Player2 = true;
    		if(gamepart[0]){
    			char msg[] = "start";
				send(client, msg , sizeof(msg) , 0 );
    		}
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
		if(buffer[0] == 'r' || buffer[0] == 'l' || buffer[0] == 'u' || buffer[0] == 'd'){
    		server_player.move(buffer);
    	}
    	if(buffer[0] == 's'){
    		Player2 = true;
    		if(gamepart[0]){
    			char msg[] = "start";
				send(server, msg , sizeof(msg) , 0 );
    		}
    	}
    	if(buffer[0] == 'n'){
    		name2 = buffer;
    		name2 = name2.substr(1);
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

	//Load background texture and player texture
	if (!mapTexture.loadFromFile("Image/tile.png", true) || !server_playerTexture.loadFromFile("Image/undertale.png", true) || !client_playerTexture.loadFromFile("Image/undertale.png", true)){
	// if( !gBackgroundTexture.loadFromFile("background.png",true ) )
		printf( "Failed to load background texture image!\n" );
		success = false;
	}
	if ( !gstartscreen.loadFromFile("Image/back.png",true ) )
	{
		printf( "Failed to load background start image!\n" );
		success = false;
	}
	
	if ( !gstartcontrol.loadFromFile("Image/con.png",true ))
	{
		printf( "Failed to load background start image!\n" );
		success = false;
	}

	if ( !gstartinstruction.loadFromFile("Image/ins.png",true ))
	{
		printf( "Failed to load background start image!\n" );
		success = false;
	}

	//Load Text
	if ( !Screen_Start.Text_init(myfont,"Start",{255,255,255},startfontsize ) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}

	if ( !Screen_Controls.Text_init(myfont,"Controls",{255,255,255},startfontsize ) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}

	if ( !Screen_Instructions.Text_init(myfont,"Instructions",{255,255,255},startfontsize ) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}

	if ( !Screen_Quit.Text_init(myfont,"Quit",{255,255,255},startfontsize ) )
	{
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_Wait1.Text_init(myfont,"Waiting for other player ",{255,255,255},40) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_Wait2.Text_init(myfont,"to join......",{255,255,255},40) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_GetName.Text_init(myfont,"Enter Your Name: ",{255,255,255},25) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_Time.Text_init(myfont,TimeStr,{255,255,255},25) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_Space.Text_init(myfont,"press Space to Continue...........",{255,255,255},25) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_WinnerName.Text_init(myfont,"Win the Game",{255,255,255},25) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_Back.Text_init(myfont,"Back",{255,255,255},30) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_Keyboard.Text_init(myfont,"Keyboard",{255,255,255},40) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	if( !Screen_Gamepad.Text_init(myfont,"Gamepad",{255,255,255},40) ){
		printf( "Failed to load text!\n" );
		success = false;
	}
	//Load music
	gMusic = Mix_LoadMUS( "Audio/scratch.wav" );
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	//Load sound effects
	gScratch = Mix_LoadWAV( "Audio/scratch.wav" );
	if( gScratch == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	gHigh = Mix_LoadWAV( "Audio/high.wav" );
	if( gHigh == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gMedium = Mix_LoadWAV( "Audio/medium.wav" );
	if( gMedium == NULL )
	{
		printf( "Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gLow = Mix_LoadWAV( "Audio/low.wav" );
	if( gLow == NULL )
	{
		printf( "Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	return success;
}

void close(){
	gstartscreen;
	gstartcontrol;
	gstartinstruction;

	//Free loaded images
	// gBackgroundTexture.free();
	gstartscreen.free();

    mapTexture.free();
	server_playerTexture.free();
	client_playerTexture.free();

    //free text
    Screen_Start.free();
   	Screen_Controls.free();
	Screen_Instructions.free();
	Screen_Quit.free();

	//timers
	Screen_Wait1.free();
	Screen_Wait2.free();
	Screen_GetName.free();
	Screen_Time.free();
	Screen_Space.free();

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

void Server_Connect(){
	x = 240,y = 190,x2 = 360,y2 = 190;
	sx = array_sx[0][0] ,sy = array_sx[0][1], sw = array_sx[0][2],array_sx[0][3], si = 0;
	struct sockaddr_in Server_Address;
	char buffer[buffer_size] = {0};
	server = socket ( AF_INET, SOCK_STREAM, 0);
	if (server < 0){
		cout<<"Opening of Socket Failed !"<<endl;
		exit ( EXIT_FAILURE);
	}
	cout<<"Server Socket connection created"<<endl;

	char*IP_Address_Array = &IP_Address[0];

	Server_Address.sin_family = AF_INET;
	Server_Address.sin_port = htons(Server_Port);
    Server_Address.sin_addr.s_addr = inet_addr(IP_Address_Array);

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
	sx = array_sx[0][0] ,sy = array_sx[0][1], sw = array_sx[0][2],array_sx[0][3], si = 0;
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

	char*IP_Address_Array = &IP_Address[0];

	if(inet_pton(AF_INET, IP_Address_Array, &Server_Address.sin_addr)<=0) 
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
void Mouse_Handle(SDL_Event e , int c){
	//Get mouse position
	int mx, my;
	SDL_GetMouseState(&mx,&my);
	if(e.type == SDL_MOUSEBUTTONDOWN and gamepart[1] == true){
		gamepart[1] = false;
		return;
	}
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
	return;
}

void Server_Keyboard_Handle(SDL_Event e){
	if(GameOver){
		if(e.key.keysym.sym == SDLK_SPACE){
			gamepart[0] = false;
			gamestart = true;
			Player2 = false;
			GameOver = false;
			MatchTime = GameTime;
			CountTime = GameTime-1;
			cout<<name<<endl;
			cout<<name2<<endl;
		}
	}
	else if(getname){
		Keyboard_Get_Name(e,client);
		return;
	}
	else if(gamestart){
		Keyboard_Start_Screen(e, client);
		return ;
	}
	switch(e.key.keysym.sym){
                     
		case SDLK_RIGHT:{
			char msg[] = "right";
			send(client, msg , sizeof(msg) , 0);
			server_player.move(msg);
			break;
		}	

		case SDLK_LEFT:{
			char msg[] = "left";
			send(client, msg , sizeof(msg) , 0);
			server_player.move(msg);
			break;
		}

		case SDLK_UP:{
			char msg[] = "up";
			send(client, msg , sizeof(msg) , 0);
			server_player.move(msg);
			break;
		}

		case SDLK_DOWN:{
			char msg[] = "down";
			send(client, msg , sizeof(msg) , 0 );
			server_player.move(msg);
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


void Client_Keyboard_Handle(SDL_Event e){
	if(GameOver){
		if(e.key.keysym.sym == SDLK_SPACE){
			gamepart[0] = false;
			gamestart = true;
			Player2 = false;
			GameOver = false;
			MatchTime = GameTime;
			CountTime = GameTime-1;
			cout<<name<<endl;
			cout<<name2<<endl;
		}
	}
	else if(getname){
		Keyboard_Get_Name(e,server);
		return;
	}
	else if(gamestart){
		Keyboard_Start_Screen(e,server);
		return ;
	}
	switch(e.key.keysym.sym){
                     
		case SDLK_RIGHT:{
			char msg[] = "right";
			send(server, msg , sizeof(msg) , 0 );
			client_player.move(msg);
			break;
		}	

		case SDLK_LEFT:{
			char msg[] = "left";
			send(server, msg , sizeof(msg) , 0 );
			client_player.move(msg);
			break;
		}

		case SDLK_UP:{
			char msg[] = "up";
			send(server, msg , sizeof(msg) , 0 );
			client_player.move(msg);
			break;
		}

		case SDLK_DOWN:{
			char msg[] = "down";
			send(server, msg , sizeof(msg) , 0 );
			client_player.move(msg);
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
		       	send(client, msg , sizeof(msg) , 0);
				server_player.move(msg);
            }
            //Right of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "right";
				send(client, msg , sizeof(msg) , 0);
				server_player.move(msg);
            }
        }
        //Y axis motion
        else if( e.jaxis.axis == 1 ){
            //Below of dead zone
            if( e.jaxis.value < -JOYSTICK_DEAD_ZONE ){
                char msg[] = "up";
	            send(client, msg , sizeof(msg) , 0);
				server_player.move(msg);
            }
            //Above of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "down";
			    send(client, msg , sizeof(msg) , 0);
				server_player.move(msg);
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
				client_player.move(msg);
            }
            //Right of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "right";
				send(server, msg , sizeof(msg) , 0 );
				client_player.move(msg);
            }
        }
        //Y axis motion
        else if( e.jaxis.axis == 1 ){
            //Below of dead zone
            if( e.jaxis.value < -JOYSTICK_DEAD_ZONE ){
                char msg[] = "up";
	            send(server, msg , sizeof(msg) , 0 );
				client_player.move(msg);
            }
            //Above of dead zone
            else if( e.jaxis.value > JOYSTICK_DEAD_ZONE ){
                char msg[] = "down";
			    send(server, msg , sizeof(msg) , 0 );
				client_player.move(msg);
            }
        }
    }
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
void Keyboard_Get_Name(SDL_Event e , int c){
	if(e.key.keysym.sym == 	SDLK_KP_ENTER or e.key.keysym.sym == SDLK_RETURN ){
		if(name!=""){
			getname = false;
			return;
		}	
	}
	if(e.key.keysym.sym == 	SDLK_BACKSPACE and (name.size()>0)){
		name.pop_back();
	}
	switch(e.key.keysym.sym){
                     
		case SDLK_a:
		{
			name = name + "a";
			break;
		}
		case SDLK_b:
		{
			name = name + "b";
			break;
		}
		case SDLK_c:
		{
			name = name + "c";
			break;
		}
		case SDLK_d:
		{
			name = name + "d";
			break;
		}
		case SDLK_e:
		{
			name = name + "e";
			break;
		}
		case SDLK_f:
		{
			name = name + "f";
			break;
		}
		case SDLK_g:
		{
			name = name + "g";
			break;
		}
		case SDLK_h:
		{
			name = name + "h";
			break;
		}
		case SDLK_i:
		{
			name = name + "i";
			break;
		}
		case SDLK_j:
		{
			name = name + "j";
			break;
		}
		case SDLK_k:
		{
			name = name + "k";
			break;
		}
		case SDLK_l:
		{
			name = name + "l";
			break;
		}
		case SDLK_m:
		{
			name = name + "m";
			break;
		}
		case SDLK_n:
		{
			name = name + "n";
			break;
		}
		case SDLK_o:
		{
			name = name + "o";
			break;
		}
		case SDLK_p:
		{
			name = name + "p";
			break;
		}
		case SDLK_q:
		{
			name = name + "q";
			break;
		}
		case SDLK_r:
		{
			name = name + "r";
			break;
		}
		case SDLK_s:
		{
			name = name + "s";
			break;
		}
		case SDLK_t:
		{
			name = name + "t";
			break;
		}
		case SDLK_w:
		{
			name = name + "w";
			break;
		}
		case SDLK_u:
		{
			name = name + "u";
			break;
		}
		case SDLK_v:
		{
			name = name + "v";
			break;
		}
		case SDLK_x:
		{
			name = name + "x";
			break;
		}
		case SDLK_y:
		{
			name = name + "y";
			break;
		}
		case SDLK_z:
		{
			name = name + "z";
			break;
		}

	}
	name[0] = toupper(name[0]);
	Screen_GetName.Text_init(myfont,"Enter Your Name: "+name,{255,255,255},25) ;

}
