//
// Created by screemer on 19.04.2020.
//
#include <stdio.h>
#include "abl.h"

struct ControllerInfo * padinfo[MAXPADS];

int lastTL = 0;
int lastTR = 0;

int lastX = 0;
int lastY = 0;

#define DEADZONE 30000

int AB_DISABLE_ANALOGUE=1;

int AB_NumJoysticks()
{
    int joys=0;
    for (int i=0;i<MAXPADS;i++)
    {
        if (padinfo[i]!=NULL) joys++;
    }
    return joys;
}
int AB_JoyInPort(int portnum)
{
    int foundjoy = -1;
    for (int i=0;i<MAXPADS;i++)
    {
        if (padinfo[i]!=NULL)
        {
            foundjoy++;
            if (foundjoy==portnum) return i;
        }
    }
    return -1;
}

void AB_ProbePads()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            AB_RegisterPad(i);
        }
    }
}
void AB_RemovePad(int joyid)
{
    if (padinfo[joyid]!=NULL)
    {
        struct ControllerInfo * info = padinfo[joyid];
        printf("Gamepad Disconnected: %s\n",info->name);
        SDL_GameControllerClose(info->pad);
        free(padinfo[joyid]);
        padinfo[joyid]=NULL;
    }
}
void AB_RegisterPad(int joyid)
{
    SDL_Joystick* js = SDL_JoystickOpen(joyid);
    SDL_JoystickGUID guid = SDL_JoystickGetGUID(js);
    char guid_str[1024];
    SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));

    int isgamecontroller = SDL_IsGameController(joyid);
    if (!isgamecontroller)
    {
        SDL_JoystickClose(js);
        printf("Joystick not recognized: %s\n", guid_str);
    }


    if (padinfo[joyid]!=NULL)
    {
        AB_RemovePad(joyid);
    }
    SDL_GameController *controller = NULL;
    controller = SDL_GameControllerOpen(joyid);
    const char * name = SDL_GameControllerName(controller);
    printf("New GameController GUID: %s    name:  %s\n",guid_str,name);
    if (controller) {
       printf("New Game Controller ID: %d\n", joyid);
    } else {
        printf("Can not open Game Controller ID: %d\n", joyid);
        return;
    }
    char * mappingString = SDL_GameControllerMapping(controller);
    printf("MAP: %s",mappingString);
    struct ControllerInfo * info;
    info = malloc(sizeof(struct ControllerInfo));
    info->pad = controller;
    info->joy = SDL_GameControllerGetJoystick(info->pad);
    strcpy(info->guid, guid_str);
    strcpy( info->name, name);
    padinfo[joyid]=info;

}
int AB_Init(Uint32 flags, const char *gamecontrollerdb)
{  int res = SDL_Init(flags);
   SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
   SDL_InitSubSystem(SDL_INIT_JOYSTICK);
   for (int i=0;i<MAXPADS;i++)
   {
       padinfo[i]=NULL;
   }

    int loadedMappings = SDL_GameControllerAddMappingsFromFile(gamecontrollerdb);
    printf("Loaded pad mappings %d\n", loadedMappings);


    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            AB_RegisterPad(i);
        }
    }

   return res;
}

void AB_Quit()
{
    for (int i=0;i<MAXPADS;i++)
    {
        if (padinfo[i]!=NULL)
        {
            free(padinfo[i]);
        }
    }
}

void AB_PopulateDpadEvent(SDL_Event * ev, SDL_Event * originalEvent, int type, int direction)
{
    memcpy(ev,originalEvent, sizeof(SDL_Event));
    ev->type = type;
    ev->cbutton.button = direction;
}




int AB_ProcessEvent(SDL_Event * originalEvent, SDL_Event * ev)
{
    if (AB_DISABLE_ANALOGUE==0)
    {
        if (originalEvent->type == SDL_CONTROLLERAXISMOTION)
        {
            if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
            {
                int val = originalEvent->caxis.value;

                if (val>DEADZONE && lastX < DEADZONE)
                {

                    lastX = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DRIGHT);
                    return 1;
                }
                if (val<DEADZONE && lastX > DEADZONE)
                {

                    lastX = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DRIGHT);
                    return 1;
                }
                if (val<-DEADZONE && lastX > -DEADZONE)
                {

                    lastX = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DLEFT);
                    return 1;
                }
                if (val>-DEADZONE && lastX < -DEADZONE)
                {

                    lastX = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DLEFT);
                    return 1;
                }
                return 0;

            }
            if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
            {
                int val = originalEvent->caxis.value;

                if (val>DEADZONE && lastY < DEADZONE)
                {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DDOWN);
                    return 1;
                }
                if (val<DEADZONE && lastY > DEADZONE)
                {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DDOWN);
                    return 1;
                }
                if (val<-DEADZONE && lastY > -DEADZONE)
                {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DUP);
                    return 1;
                }
                if (val>-DEADZONE && lastY < -DEADZONE)
                {

                    lastY = originalEvent->caxis.value;
                    AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DUP);
                    return 1;
                }
                return 0;

            }
        }
    }


    if (originalEvent->type == SDL_CONTROLLERBUTTONUP)
    {
        if (originalEvent->cbutton.button== AB_BTN_DUP)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DUP);
            return 1;
        }
        if (originalEvent->cbutton.button== AB_BTN_DDOWN)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DDOWN);
            return 1;
        }
        if (originalEvent->cbutton.button== AB_BTN_DLEFT)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DLEFT);
            return 1;
        }
        if (originalEvent->cbutton.button== AB_BTN_DRIGHT)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONUP,AB_BTN_DRIGHT);
            return 1;
        }

    }
    if (originalEvent->type == SDL_CONTROLLERBUTTONDOWN)
    {
        if (originalEvent->cbutton.button== AB_BTN_DUP)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DUP);
            return 1;
        }
        if (originalEvent->cbutton.button== AB_BTN_DDOWN)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DDOWN);
            return 1;
        }
        if (originalEvent->cbutton.button== AB_BTN_DLEFT)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DLEFT);
            return 1;
        }
        if (originalEvent->cbutton.button== AB_BTN_DRIGHT)
        {
            AB_PopulateDpadEvent(ev,originalEvent,AB_HATMOTIONDOWN,AB_BTN_DRIGHT);
            return 1;
        }
    }
    // simulate triggers for L2/R2
    if (originalEvent->type == SDL_CONTROLLERAXISMOTION)
    {
        if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
        {
            int val = originalEvent->caxis.value;
            if (val>DEADZONE && lastTL < DEADZONE)
            {
                // L2 pressed
                memcpy(ev,originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONDOWN;
                ev->cbutton.button = AB_BTN_L2;
                lastTL = originalEvent->caxis.value;
                return 1;
            }
            if (val<DEADZONE && lastTL > DEADZONE)
            {
                // L2 released
                memcpy(ev,originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONUP;
                ev->cbutton.button = AB_BTN_L2;
                lastTL = originalEvent->caxis.value;
                return 1;
            }
            return 0;

        }
        if (originalEvent->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
        {
            int val = originalEvent->caxis.value;
            if (val>DEADZONE && lastTR < DEADZONE)
            {
                memcpy(ev,originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONDOWN;
                ev->cbutton.button = AB_BTN_R2;
                lastTR= originalEvent->caxis.value;
                return 1;
            }
            if (val<DEADZONE && lastTR > DEADZONE)
            {
                // L2 released
                memcpy(ev,originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONUP;
                ev->cbutton.button = AB_BTN_R2;
                lastTR = originalEvent->caxis.value;
                return 1;
            }
            return 0;

        }
    }
    return 1;
}

// SDL_Event Routines
int AB_PeepEvents(SDL_Event*       events,
                   int             numevents,
                   SDL_eventaction action,
                   Uint32          minType,
                   Uint32          maxType)
{
    if (action==SDL_ADDEVENT)
    {
        return SDL_PeepEvents(events, numevents, action, minType, maxType);
    } else
    {
        SDL_Event * newEvents;
        newEvents = malloc(numevents* sizeof(SDL_Event));
        int stored = SDL_PeepEvents(newEvents,numevents,action,minType,maxType);
        if (stored<0) return stored;
        int processed=0;
        for (int i=0;i<stored;i++)
        {

            int result = AB_ProcessEvent(newEvents+ sizeof(SDL_Event)*i, events+ sizeof(SDL_Event)*processed);
            if (result == 0)
            {
                continue;
            } else
            {
                processed++;
            }
        }
        free(newEvents);
        return processed;
    }

}

int AB_PollEvent(SDL_Event * ev)
{
    SDL_Event originalEvent;
    memcpy(ev,&originalEvent, sizeof(SDL_Event));
    int res = SDL_PollEvent(&originalEvent);
    int processed = AB_ProcessEvent(&originalEvent,ev);
    if (processed == 0)
    {
        return AB_PollEvent(ev);
    } else
    {

        return res;
    }


}