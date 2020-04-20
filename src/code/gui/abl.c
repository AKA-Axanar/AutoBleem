//
// Created by screemer on 19.04.2020.
//
#include <stdio.h>
#include "abl.h"

int lastTL = 0;
int lastTR = 0;

int lastX = 0;
int lastY = 0;

#define DEADZONE 31000

int AB_DISABLE_ANALOGUE=0;

int AB_PopulateDpadEvent(SDL_Event * ev, SDL_Event * originalEvent, int type, int direction, int resval)
{
    memcpy(ev,originalEvent, sizeof(SDL_Event));
    ev->type = type;
    ev->cbutton.button = direction;
    return resval;
}

int AB_PollEvent(SDL_Event * ev)
{
    SDL_Event originalEvent;

    int res = SDL_PollEvent(&originalEvent);

    if (AB_DISABLE_ANALOGUE==0)
    {
        if (originalEvent.type == SDL_CONTROLLERAXISMOTION)
        {
            if (originalEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
            {
                int val = originalEvent.caxis.value;

                if (val>DEADZONE && lastX < DEADZONE)
                {

                    lastX = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DRIGHT,res);;
                }
                if (val<DEADZONE && lastX > DEADZONE)
                {

                    lastX = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DRIGHT,res);;
                }
                if (val<-DEADZONE && lastX > -DEADZONE)
                {

                    lastX = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DLEFT,res);;
                }
                if (val>-DEADZONE && lastX < -DEADZONE)
                {

                    lastX = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DLEFT,res);;
                }
                return AB_PollEvent(ev);
            }
            if (originalEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
            {
                int val = originalEvent.caxis.value;

                if (val>DEADZONE && lastY < DEADZONE)
                {

                    lastY = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DDOWN,res);;
                }
                if (val<DEADZONE && lastY > DEADZONE)
                {

                    lastY = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DDOWN,res);;
                }
                if (val<-DEADZONE && lastY > -DEADZONE)
                {

                    lastY = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DUP,res);;
                }
                if (val>-DEADZONE && lastY < -DEADZONE)
                {

                    lastY = originalEvent.caxis.value;
                    return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DUP,res);;
                }
                return AB_PollEvent(ev);
            }
        }
    }


    if (originalEvent.type == SDL_CONTROLLERBUTTONUP)
    {
        if (originalEvent.cbutton.button== AB_BTN_DUP)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DUP,res);
        }
        if (originalEvent.cbutton.button== AB_BTN_DDOWN)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DDOWN,res);
        }
        if (originalEvent.cbutton.button== AB_BTN_DLEFT)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DLEFT,res);
        }
        if (originalEvent.cbutton.button== AB_BTN_DRIGHT)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONUP,AB_BTN_DRIGHT,res);
        }

    }
    if (originalEvent.type == SDL_CONTROLLERBUTTONDOWN)
    {
        if (originalEvent.cbutton.button== AB_BTN_DUP)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DUP,res);
        }
        if (originalEvent.cbutton.button== AB_BTN_DDOWN)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DDOWN,res);
        }
        if (originalEvent.cbutton.button== AB_BTN_DLEFT)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DLEFT,res);
        }
        if (originalEvent.cbutton.button== AB_BTN_DRIGHT)
        {
            return AB_PopulateDpadEvent(ev,&originalEvent,AB_HATMOTIONDOWN,AB_BTN_DRIGHT,res);
        }
    }
    // simulate triggers for L2/R2
    if (originalEvent.type == SDL_CONTROLLERAXISMOTION)
    {




        if (originalEvent.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
        {
            int val = originalEvent.caxis.value;
            if (val>DEADZONE && lastTL < DEADZONE)
            {
                // L2 pressed
                memcpy(ev,&originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONDOWN;
                ev->cbutton.button = AB_BTN_L2;
                lastTL = originalEvent.caxis.value;
                return res;
            }
            if (val<DEADZONE && lastTL > DEADZONE)
            {
                // L2 released

                memcpy(ev,&originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONUP;
                ev->cbutton.button = AB_BTN_L2;
                lastTL = originalEvent.caxis.value;
                return res;
            }

            return AB_PollEvent(ev);
        }
        if (originalEvent.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
        {
            int val = originalEvent.caxis.value;
            if (val>DEADZONE && lastTR < DEADZONE)
            {
                memcpy(ev,&originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONDOWN;
                ev->cbutton.button = AB_BTN_R2;
                lastTR= originalEvent.caxis.value;
                return res;
            }
            if (val<DEADZONE && lastTR > DEADZONE)
            {
                // L2 released
                memcpy(ev,&originalEvent, sizeof(SDL_Event));
                ev->type = SDL_CONTROLLERBUTTONUP;
                ev->cbutton.button = AB_BTN_R2;
                lastTR = originalEvent.caxis.value;
                return res;
            }

            return AB_PollEvent(ev);
        }
    }
    memcpy(ev,&originalEvent, sizeof(SDL_Event));
    return res;
}