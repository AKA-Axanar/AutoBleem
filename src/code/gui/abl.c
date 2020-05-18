//
// Created by screemer on 19.04.2020.
//
#include <stdio.h>
#include "abl.h"

#define TRIGGER_DEADZONE 5000
#define STICK_DEADZONE 25000
#define ANALOG_TO_DPAD

#define EVENT_ORIGINAL -1
#define EVENT_FILTERED 1
#define EVENT_NONE 0
#define TRIGGER_LEFT 0
#define TRIGGER_RIGHT 1

int sdl_filter_analog_on = 1;

int process_stick_event(SDL_Event *originalEvent) {
    int res = EVENT_ORIGINAL;
    static int lastAxisPos[4];
    int axis = -1;
    switch (originalEvent->caxis.axis) {
        case SDL_CONTROLLER_AXIS_LEFTX:
            axis = 0;
            break;
        case SDL_CONTROLLER_AXIS_LEFTY:
            axis = 1;
            break;
        case SDL_CONTROLLER_AXIS_RIGHTX:
            axis = 2;
            break;
        case SDL_CONTROLLER_AXIS_RIGHTY:
            axis = 3;
            break;
    }

    if (axis != -1) {
        int switch_down_positive = originalEvent->caxis.value > STICK_DEADZONE && lastAxisPos[axis] < STICK_DEADZONE;
        int switch_up_positive = originalEvent->caxis.value < STICK_DEADZONE && lastAxisPos[axis] > STICK_DEADZONE;
        int switch_down_negative = originalEvent->caxis.value < -STICK_DEADZONE && lastAxisPos[axis] > -STICK_DEADZONE;
        int switch_up_negative = originalEvent->caxis.value > -STICK_DEADZONE && lastAxisPos[axis] < -STICK_DEADZONE;

        int newEventType;
        if (switch_down_positive || switch_down_negative)
            newEventType = SDL_CONTROLLERBUTTONDOWN;
        else newEventType = SDL_CONTROLLERBUTTONUP;
        int newButton = -1;
        if ((switch_down_positive || switch_up_positive)
            && (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY ||
                originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY))
            newButton = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        else if ((switch_down_negative || switch_up_negative)
                 && (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY ||
                     originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY))
            newButton = SDL_CONTROLLER_BUTTON_DPAD_UP;
        else if ((switch_down_positive || switch_up_positive)
                 && (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX ||
                     originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX))
            newButton = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        else if ((switch_down_negative || switch_up_negative)
                 && (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX ||
                     originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX))
            newButton = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        if (newButton != -1) {
            lastAxisPos[axis] = originalEvent->caxis.value;
            originalEvent->type = newEventType;
            originalEvent->cbutton.button = newButton;
            originalEvent->cbutton.state = (newEventType == SDL_CONTROLLERBUTTONDOWN) ? SDL_PRESSED : SDL_RELEASED;
            res = EVENT_ORIGINAL;
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
        int trigger_down =
                (originalEvent->caxis.value > TRIGGER_DEADZONE) && (lastTriggerPos[trigger] < TRIGGER_DEADZONE);
        int trigger_up =
                (originalEvent->caxis.value < TRIGGER_DEADZONE) && (lastTriggerPos[trigger] > TRIGGER_DEADZONE);
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
    ev->cbutton.state = (type == SDL_CONTROLLERHATMOTIONDOWN) ? SDL_PRESSED : SDL_RELEASED;
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
                printf("First event %d\n");
                break;
            case SDL_WINDOWEVENT_CLOSE:
                SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
                break;
        }
    } else
        // L2/R2 simulation
    if (originalEvent->type == SDL_CONTROLLERAXISMOTION) {
        switch (originalEvent->caxis.axis) {
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                res = process_trigger_event(originalEvent, ev);
                goto out;

                // simulation of dpad on analogue - we do not need this
            case SDL_CONTROLLER_AXIS_LEFTX:
            case SDL_CONTROLLER_AXIS_LEFTY:
            case SDL_CONTROLLER_AXIS_RIGHTX:
            case SDL_CONTROLLER_AXIS_RIGHTY:
                if (sdl_filter_analog_on == 1) {
                    res = process_stick_event(originalEvent);
                }
                break;

            default:
                res = EVENT_ORIGINAL;
                goto out;
        }
    }
    if ((originalEvent->type == SDL_CONTROLLERBUTTONUP) || (originalEvent->type == SDL_CONTROLLERBUTTONDOWN)
        || (ev->type == SDL_CONTROLLERBUTTONDOWN) || (ev->type == SDL_CONTROLLERBUTTONDOWN)) {
        switch (originalEvent->cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                eventType = (originalEvent->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_CONTROLLERHATMOTIONDOWN :
                            SDL_CONTROLLERHATMOTIONUP;
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





