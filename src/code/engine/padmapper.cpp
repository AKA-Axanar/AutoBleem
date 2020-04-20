//
// Created by screemer on 2019-03-21.
//

#include "padmapper.h"
#include "../util.h"
#include <iostream>
#include "../DirEntry.h"
#include "../environment.h"
#include "../gui/abl.h"



//*******************************
// PadMapper::isDirection
//*******************************
bool PadMapper::isDirection(SDL_Event *e, int dir) {
    if (e->type==AB_HATMOTIONDOWN)
    {
        if (e->cbutton.button == AB_BTN_DUP) status[DUP]=true;
        if (e->cbutton.button == AB_BTN_DDOWN) status[DDOWN]=true;
        if (e->cbutton.button == AB_BTN_DLEFT) status[DLEFT]=true;
        if (e->cbutton.button == AB_BTN_DRIGHT) status[DRIGHT]=true;
    }

    if (e->type==AB_HATMOTIONUP)
    {
        if (e->cbutton.button == AB_BTN_DUP) status[DUP]=false;
        if (e->cbutton.button == AB_BTN_DDOWN) status[DDOWN]=false;
        if (e->cbutton.button == AB_BTN_DLEFT) status[DLEFT]=false;
        if (e->cbutton.button == AB_BTN_DRIGHT) status[DRIGHT]=false;
    }

    switch (dir) {
        case DIR_UP:
            return status[DUP];
        case DIR_DOWN:
            return status[DDOWN];
        case DIR_LEFT:
            return status[DLEFT];
        case DIR_RIGHT:
            return status[DRIGHT];
        case DIR_NONE:
            return !status[DUP] && !status[DDOWN] && !status[DLEFT] && !status[DRIGHT];

    }

return false;
/*
    SDL_JoystickID id;

    if (e->type == AB_HATMOTIONUP) {
        id = e->jhat.which;
        cout << "HAT:" << endl;
        cout << to_string(e->jhat.hat) << endl;
    } else {
        id = e->jaxis.which;
        cout << "AXIS:" << endl;
        cout << to_string(e->jaxis.axis) << endl;
    }
    SDL_Joystick *joy = SDL_JoystickFromInstanceID(id);
    const char *joyname = SDL_JoystickName(joy);

    Inifile *config = configs[joyname];
    if (config == nullptr) {
        config = defaultConfig;
    }

    int deadZone = atoi(config->values["dpaddeadzone"].c_str());

    int axis;
    if ((dir == DIR_DOWN) || (dir == DIR_UP)) axis = 1;
    else axis = 0;

    if (config->values["dpad"] == "analogue") {

        if (dir != DIR_NONE) {
            if (e->type == AB_HATMOTIONDOWN) {
                if (e->jaxis.axis == axis) {
                    if ((dir == DIR_UP) || (dir == DIR_LEFT)) {
                        return (e->jaxis.value < -deadZone);
                    } else {
                        return (e->jaxis.value > deadZone);
                    }
                }
            }
        } else {
            return (e->jaxis.value >= -deadZone) && (e->jaxis.value <= deadZone);
        }
    } else {
        if (e->type == AB_HATMOTIONUP) {
            switch (dir) {
                case DIR_UP:
                    return (e->jhat.value == SDL_HAT_UP);
                case DIR_DOWN:
                    return (e->jhat.value == SDL_HAT_DOWN);
                case DIR_LEFT:
                    return (e->jhat.value == SDL_HAT_LEFT);
                case DIR_RIGHT:
                    return (e->jhat.value == SDL_HAT_RIGHT);
                case DIR_NONE:
                    return (e->jhat.value == SDL_HAT_CENTERED);
            }
        }
    }
    return false;
    */
}

//*******************************
// PadMapper::isCenter
//*******************************
bool PadMapper::isCenter(SDL_Event *event) {
    return isDirection(event, DIR_NONE);
}

//*******************************
// PadMapper::isUp
//*******************************
bool PadMapper::isUp(SDL_Event *event) {
    return isDirection(event, DIR_UP);
}

//*******************************
// PadMapper::isDown
//*******************************
bool PadMapper::isDown(SDL_Event *event) {
    return isDirection(event, DIR_DOWN);
}

//*******************************
// PadMapper::isLeft
//*******************************
bool PadMapper::isLeft(SDL_Event *event) {
    return isDirection(event, DIR_LEFT);
}

//*******************************
// PadMapper::isRight
//*******************************
bool PadMapper::isRight(SDL_Event *event) {
    return isDirection(event, DIR_RIGHT);
}

