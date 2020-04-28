//
// Created by screemer on 2019-03-21.
//
#pragma once

#include <SDL2/SDL_events.h>
#include <vector>
#include "../main.h"
#include "inifile.h"


using namespace std;

#define DUP         0
#define DDOWN       1
#define DLEFT       2
#define DRIGHT      3






#define DIR_UP    1
#define DIR_DOWN  2
#define DIR_LEFT  3
#define DIR_RIGHT 4
#define DIR_NONE  5

class ControllerInfo
{
public:
    SDL_GameController * pad;
    SDL_Joystick * joy;
    string name;
    string guid;
    int index;
};
using namespace std;
//******************
// PadMapper
//******************
class PadMapper {
public:
    vector<ControllerInfo*> connectedPads;
    bool status[4];
    // loads all mapping files
    PadMapper()
    {
        status[0]=false;
        status[1]=false;
        status[2]=false;
        status[3]=false;
    };
    bool isUp(SDL_Event* event);
    bool isDown(SDL_Event* event);
    bool isLeft(SDL_Event* event);
    bool isRight(SDL_Event* event);
    bool isCenter(SDL_Event* event);
    bool isDirection(SDL_Event* e,  int dir);
    void init();
    void probePads(vector<string> gamecontrollerdb);
    void registerPad(int joy_idx);
    void removePad(int joy_idx);
    void flushPads();
    void handleHotPlug(SDL_Event * event);
};
