//
// Created by screemer on 19.04.2020.
//

#ifndef AUTOBLEEM_GUI_ABL_H
#define AUTOBLEEM_GUI_ABL_H
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif
int AB_PollEvent(SDL_Event *event);
int AB_Init(Uint32 flags, const char * gamecontrollerdb);
void AB_Quit();
void AB_RegisterPad(int joy_idx);
void AB_RemovePad(int id);
void AB_ProbePads(const char * gamecontrollerdb);
void AB_FlushPadInfo();
int AB_NumJoysticks();
int AB_JoyInPort();
int AB_PeepEvents(SDL_Event*       events,
                  int             numevents,
                  SDL_eventaction action,
                  Uint32          minType,
                  Uint32          maxType);

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

#define MAXPADS 100


#define AB_BTN_TRIANGLE SDL_CONTROLLER_BUTTON_Y
#define AB_BTN_SQUARE   SDL_CONTROLLER_BUTTON_X
#define AB_BTN_CIRCLE   SDL_CONTROLLER_BUTTON_B
#define AB_BTN_CROSS    SDL_CONTROLLER_BUTTON_A
#define AB_BTN_START    SDL_CONTROLLER_BUTTON_START
#define AB_BTN_SELECT   SDL_CONTROLLER_BUTTON_BACK
#define AB_BTN_L1       SDL_CONTROLLER_BUTTON_LEFTSHOULDER
#define AB_BTN_R1       SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
#define AB_BTN_L2       SDL_CONTROLLER_BUTTON_MAX+201
#define AB_BTN_R2       SDL_CONTROLLER_BUTTON_MAX+202
#define AB_BTN_DUP      SDL_CONTROLLER_BUTTON_DPAD_UP
#define AB_BTN_DDOWN    SDL_CONTROLLER_BUTTON_DPAD_DOWN
#define AB_BTN_DLEFT    SDL_CONTROLLER_BUTTON_DPAD_LEFT
#define AB_BTN_DRIGHT   SDL_CONTROLLER_BUTTON_DPAD_RIGHT



#define AB_CONTROLLERDEVICEADDED SDL_CONTROLLERDEVICEADDED
#define AB_CONTROLLERDEVICEREMOVED SDL_CONTROLLERDEVICEREMOVED
#define AB_CONTROLLERBUTTONUP SDL_CONTROLLERBUTTONUP
#define AB_CONTROLLERBUTTONDOWN SDL_CONTROLLERBUTTONDOWN
#define AB_HATMOTIONDOWN SDL_LASTEVENT-1
#define AB_HATMOTIONUP   SDL_LASTEVENT-2


struct ControllerInfo
{
    SDL_GameController * pad;
    SDL_Joystick * joy;
    char name[3000];
    char guid[1024];
};

extern struct ControllerInfo * padinfo[MAXPADS];


#endif //AUTOBLEEM_GUI_ABL_H
