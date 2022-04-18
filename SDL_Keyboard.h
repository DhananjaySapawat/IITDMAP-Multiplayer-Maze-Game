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
double GameTime = 120.00;
extern double MatchTime,CountTime;
int startfontsize = 40;
int array_sx[4][4] = {{490,155,235,75},{430,235,360,80},{380,315,500,80},{500,395,190,75}};
extern string IP_Address,TimeStr,name,name2;
string myfont = "PublicPixel-0W5Kv.ttf";


const int JOYSTICK_DEAD_ZONE = 8000;
// const int SCREEN_WIDTH = 1200;
// const int SCREEN_HEIGHT = 650;

int buffer_size = 1024;
int Server_Port = 5476;

