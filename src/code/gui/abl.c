//
// Created by screemer on 19.04.2020.
//
#include <stdio.h>
#include "abl.h"

struct ControllerInfo *padinfo[MAXPADS];
#define DEADZONE 30000
//#define ANALOG_TO_DPAD


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
    int res = EVENT_ORIGINAL;
    static int lastAxisPos[4];
    int axis = -1;
    int final_button=-1;
    int val = originalEvent->caxis.value;
    switch (originalEvent->caxis.axis) {
        case SDL_CONTROLLER_AXIS_LEFTX: axis=0;  break;
        case SDL_CONTROLLER_AXIS_LEFTY: axis=1;  break;
        case SDL_CONTROLLER_AXIS_RIGHTX: axis=2; break;
        case SDL_CONTROLLER_AXIS_RIGHTY: axis=3; break;
    }

    if (axis != -1) {

        int switch_down_positive = originalEvent->caxis.value > DEADZONE && lastAxisPos[axis] < DEADZONE;
        int switch_up_positive = originalEvent->caxis.value < DEADZONE && lastAxisPos[axis] > DEADZONE;
        int switch_down_negative = originalEvent->caxis.value < -DEADZONE && lastAxisPos[axis] > -DEADZONE;
        int switch_up_negative = originalEvent->caxis.value > -DEADZONE && lastAxisPos[axis] < -DEADZONE;
        if (switch_down_positive || switch_up_positive) {
            ev->type = switch_down_positive ? SDL_CONTROLLERBUTTONDOWN : SDL_CONTROLLERBUTTONUP;
            ev->cbutton.button = final_button;
            lastAxisPos[axis] = originalEvent->caxis.value;
            res = EVENT_FILTERED;
            goto out;
        } else if (switch_down_negative || switch_up_negative)
        {
            ev->type = switch_down_negative ? SDL_CONTROLLERBUTTONDOWN : SDL_CONTROLLERBUTTONUP;
            ev->cbutton.button = final_button;
            lastAxisPos[axis] = originalEvent->caxis.value;
            res = EVENT_FILTERED;
            goto out;
        }
        res = EVENT_NONE;
    }
    out:
    return res;
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
            ev->cbutton.button = (trigger == TRIGGER_LEFT) ? SDL_BTN_L2 : SDL_BTN_R2;
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
    ev->cbutton.state = (type==SDL_CONTROLLERHATMOTIONDOWN)?SDL_PRESSED:SDL_RELEASED;
}


int playstation_event_filter(void *data, SDL_Event *originalEvent) {
    static SDL_Event instance;
    SDL_Event *ev = &instance;
    int eventType;
    int res = EVENT_ORIGINAL;
    memcpy(ev, originalEvent, sizeof(SDL_Event));
    // AutoInit GamecontrollerAPI
    if (originalEvent->type == SDL_WINDOWEVENT) {
        switch (originalEvent->window.event) {
            case SDL_WINDOWEVENT_SHOWN:
                SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
                printf ("First event %d\n");
                break;
            case SDL_WINDOWEVENT_CLOSE:
                SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
                break;
        }
    }
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
                if (res==EVENT_FILTERED)last_event_joyid = originalEvent->caxis.which;
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
                eventType = (originalEvent->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_CONTROLLERHATMOTIONDOWN : SDL_CONTROLLERHATMOTIONUP;
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




