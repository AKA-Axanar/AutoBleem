#pragma once
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif
int AB_Init(Uint32 flags, const char * gamecontrollerdb);
void AB_Quit();
void AB_RegisterPad(int joy_idx);
void AB_RemovePad(int id);
void AB_ProbePads(const char * gamecontrollerdb);
void AB_FlushPadInfo();
// all of your legacy C code here

#ifdef __cplusplus
}
#endif

#define MAXPADS 100

// Nice names for Playstation developers
#define SDL_BTN_TRIANGLE SDL_CONTROLLER_BUTTON_Y
#define SDL_BTN_SQUARE   SDL_CONTROLLER_BUTTON_X
#define SDL_BTN_CIRCLE   SDL_CONTROLLER_BUTTON_B
#define SDL_BTN_CROSS    SDL_CONTROLLER_BUTTON_A
#define SDL_BTN_START    SDL_CONTROLLER_BUTTON_START
#define SDL_BTN_SELECT   SDL_CONTROLLER_BUTTON_BACK
#define SDL_BTN_L1       SDL_CONTROLLER_BUTTON_LEFTSHOULDER
#define SDL_BTN_R1       SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
#define SDL_BTN_L2       SDL_CONTROLLER_BUTTON_MAX+201
#define SDL_BTN_R2       SDL_CONTROLLER_BUTTON_MAX+202
#define SDL_BTN_DUP      SDL_CONTROLLER_BUTTON_DPAD_UP
#define SDL_BTN_DDOWN    SDL_CONTROLLER_BUTTON_DPAD_DOWN
#define SDL_BTN_DLEFT    SDL_CONTROLLER_BUTTON_DPAD_LEFT
#define SDL_BTN_DRIGHT   SDL_CONTROLLER_BUTTON_DPAD_RIGHT

// Custom event types
#define SDL_CONTROLLERHATMOTIONDOWN SDL_LASTEVENT-1
#define SDL_CONTROLLERHATMOTIONUP   SDL_LASTEVENT-2


struct ControllerInfo
{
    SDL_GameController * pad;
    SDL_Joystick * joy;
    char name[3000];
    char guid[1024];
};

extern struct ControllerInfo * padinfo[MAXPADS];


