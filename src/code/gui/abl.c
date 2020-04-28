//
// Created by screemer on 19.04.2020.
//
#include <stdio.h>
#include "abl.h"

struct ControllerInfo *padinfo[MAXPADS];

int lastX = 0;
int lastY = 0;

#define DEADZONE 30000

#define ANALOG_TO_DPAD

void AB_FlushPadInfo() {
    for (int i = 0; i < MAXPADS; i++) {
        padinfo[i] = NULL;
    }

}


void AB_ProbePads(const char *gamecontrollerdb) {
    int loadedMappings = SDL_GameControllerAddMappingsFromFile(gamecontrollerdb);
    printf("Loaded pad mappings %d\n", loadedMappings);

    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            AB_RegisterPad(i);
        }
    }
}

void AB_RemovePad(int joyid) {
    if (padinfo[joyid] != NULL) {
        struct ControllerInfo *info = padinfo[joyid];
        printf("Gamepad Disconnected: %s\n", info->name);
        SDL_GameControllerClose(info->pad);
        free(padinfo[joyid]);
        padinfo[joyid] = NULL;
    }
}

void AB_RegisterPad(int joy_idx) {
    if (padinfo[joy_idx] != NULL) {
        AB_RemovePad(joy_idx);
    }
    SDL_Joystick *js = SDL_JoystickOpen(joy_idx);
    SDL_JoystickGUID guid = SDL_JoystickGetGUID(js);
    char guid_str[1024];
    SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));

    int isgamecontroller = SDL_IsGameController(joy_idx);
    if (!isgamecontroller) {
        SDL_JoystickClose(js);
        printf("Joystick not recognized: %s\n", guid_str);
    }

    SDL_JoystickID instanceID = SDL_JoystickInstanceID(js);


    SDL_GameController *controller = NULL;
    controller = SDL_GameControllerOpen(joy_idx);
    const char *name = "";
    if (controller) {
        name = SDL_GameControllerName(controller);
        printf("New GameController GUID: %s    name:  %s\n", guid_str, name);
        printf("New Game Controller index: %d\n", joy_idx);
    } else {
        printf("Can not open Game Controller ID: %d %s\n", joy_idx, SDL_GetError());
        return;
    }
    char *mappingString = SDL_GameControllerMapping(controller);
    printf("MAP: %s", mappingString);
    struct ControllerInfo *info;
    info = malloc(sizeof(struct ControllerInfo));
    info->pad = controller;
    info->joy = SDL_GameControllerGetJoystick(info->pad);
    strcpy(info->guid, guid_str);
    strcpy(info->name, name);
    padinfo[instanceID] = info;

}

#define EVENT_ORIGINAL -1
#define EVENT_FILTERED 1
#define EVENT_NONE 0
#define TRIGGER_LEFT 0
#define TRIGGER_RIGHT 1

int process_stick_event(SDL_Event *originalEvent, SDL_Event *ev)
{

}

int process_trigger_event(SDL_Event *originalEvent, SDL_Event *ev) {
    int res = EVENT_ORIGINAL;
    static int lastTriggerPos[2];
    int trigger = -1;
    if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
        trigger = TRIGGER_LEFT;
    else if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
        trigger = TRIGGER_RIGHT;

    if (trigger != -1) {
        int trigger_down = originalEvent->caxis.value > DEADZONE && lastTriggerPos[trigger] < DEADZONE;
        int trigger_up = originalEvent->caxis.value < DEADZONE && lastTriggerPos[trigger] > DEADZONE;
        if (trigger_down || trigger_up) {
            ev->type = trigger_down ? SDL_CONTROLLERBUTTONDOWN : SDL_CONTROLLERBUTTONUP;
            ev->cbutton.button = (trigger == TRIGGER_LEFT) ? AB_BTN_L2 : AB_BTN_R2;
            lastTriggerPos[trigger] = originalEvent->caxis.value;
            res = EVENT_FILTERED;
            goto out;
        }
        res = EVENT_NONE;
    }
    out:
    return res;
}

void populate_dpad_event(SDL_Event *ev, SDL_Event *originalEvent, int type, int button) {
    memcpy(ev, originalEvent, sizeof(SDL_Event));
    ev->type = type;
    ev->cbutton.button = button;
}


int playstation_event_filter(void *data, SDL_Event *originalEvent) {
    static SDL_Event instance;
    SDL_Event *ev = &instance;
    int eventType;
    int res = EVENT_ORIGINAL;
    memcpy(ev, originalEvent, sizeof(SDL_Event));
    // L2/R2 simulation
    if (originalEvent->type == SDL_CONTROLLERAXISMOTION) {
        switch (originalEvent->caxis.axis) {
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                res = process_trigger_event(originalEvent, ev);
                break;
#ifdef ANALOG_TO_DPAD
            case SDL_CONTROLLER_AXIS_LEFTX:
            case SDL_CONTROLLER_AXIS_LEFTY:
            case SDL_CONTROLLER_AXIS_RIGHTX:
            case SDL_CONTROLLER_AXIS_RIGHTY:
                res = process_stick_event(originalEvent,ev);
                break;

#endif
            default:
                res = EVENT_ORIGINAL;
        }
    } else if ((originalEvent->type == SDL_CONTROLLERBUTTONUP) || (originalEvent->type == SDL_CONTROLLERBUTTONDOWN)) {
        switch (originalEvent->cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                eventType = (originalEvent->type == SDL_CONTROLLERBUTTONDOWN) ? AB_HATMOTIONDOWN : AB_HATMOTIONUP;
                populate_dpad_event(ev, originalEvent, eventType, originalEvent->cbutton.button);
                res = EVENT_FILTERED;
                goto out;

        }
        res = EVENT_ORIGINAL;
    }

    out:
    switch (res) {
        case EVENT_NONE:
            return 0;
        case EVENT_FILTERED:
            memcpy(originalEvent, ev, sizeof(SDL_Event));
            return 1;
        default:
            return 1;
    }
}

int AB_Init(Uint32 flags, const char *gamecontrollerdb) {
    int res = SDL_Init(flags);
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_SetEventFilter(&playstation_event_filter, NULL);
    for (int i = 0; i < MAXPADS; i++) {
        padinfo[i] = NULL;
    }

    AB_ProbePads(gamecontrollerdb);


    return res;
}

void AB_Quit() {
    for (int i = 0; i < MAXPADS; i++) {
        if (padinfo[i] != NULL) {
            free(padinfo[i]);
            padinfo[i] = 0;
        }
    }
}


int AB_ProcessEvent(SDL_Event *originalEvent, SDL_Event *ev) {
    /*
    if (AB_DISABLE_ANALOGUE == 0) {
        if (originalEvent->type == SDL_CONTROLLERAXISMOTION) {
            if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                int val = originalEvent->caxis.value;

                if (val > DEADZONE && lastX < DEADZONE) {

                    lastX = originalEvent->caxis.value;
                    process_dpad_event(ev, originalEvent, AB_HATMOTIONDOWN, AB_BTN_DRIGHT);
                    return 1;
                }
                if (val < DEADZONE && lastX > DEADZONE) {

                    lastX = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev, originalEvent, AB_HATMOTIONUP, AB_BTN_DRIGHT);
                    return 1;
                }
                if (val < -DEADZONE && lastX > -DEADZONE) {

                    lastX = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev, originalEvent, AB_HATMOTIONDOWN, AB_BTN_DLEFT);
                    return 1;
                }
                if (val > -DEADZONE && lastX < -DEADZONE) {

                    lastX = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev, originalEvent, AB_HATMOTIONUP, AB_BTN_DLEFT);
                    return 1;
                }
                return 0;

            }
            if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                int val = originalEvent->caxis.value;

                if (val > DEADZONE && lastY < DEADZONE) {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev, originalEvent, AB_HATMOTIONDOWN, AB_BTN_DDOWN);
                    return 1;
                }
                if (val < DEADZONE && lastY > DEADZONE) {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev, originalEvent, AB_HATMOTIONUP, AB_BTN_DDOWN);
                    return 1;
                }
                if (val < -DEADZONE && lastY > -DEADZONE) {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev, originalEvent, AB_HATMOTIONDOWN, AB_BTN_DUP);
                    return 1;
                }
                if (val > -DEADZONE && lastY < -DEADZONE) {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev, originalEvent, AB_HATMOTIONUP, AB_BTN_DUP);
                    return 1;
                }
                return 0;

            }
        }
    }
*/

    return 1;
}


