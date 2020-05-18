#include "gui_screen.h"
#include "../lang.h"
#include <iostream>

using namespace std;

//*******************************
// GuiScreen::loop
//*******************************
void GuiScreen::loop()
{
    menuVisible = true;
    while (menuVisible) {

        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            gui->mapper.handleHotPlug(&e);
            gui->mapper.handlePowerBtn(&e);
            if (handlePowerShutdownAndQuit(e))
                continue;

            switch (e.type) {
                case SDL_CONTROLLERHATMOTIONDOWN:
                case SDL_CONTROLLERHATMOTIONUP:

                    if (gui->mapper.isUp(&e))
                        doJoyUp();
                    else if (gui->mapper.isDown(&e))
                        doJoyDown();
                    else if (gui->mapper.isRight(&e))
                        doJoyRight();
                    else if (gui->mapper.isLeft(&e))
                        doJoyLeft();
                    else if (gui->mapper.isCenter(&e))
                        doJoyCenter();
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                    if (e.cbutton.button == SDL_BTN_CROSS)
                        doCross_Pressed();
                    else if (e.cbutton.button == SDL_BTN_CIRCLE)
                        doCircle_Pressed();
                    else if (e.cbutton.button == SDL_BTN_TRIANGLE)
                        doTriangle_Pressed();
                    else if (e.cbutton.button == SDL_BTN_SQUARE)
                        doSquare_Pressed();
                    else if (e.cbutton.button == SDL_BTN_START)
                        doStart_Pressed();
                    else if (e.cbutton.button == SDL_BTN_SELECT)
                        doSelect_Pressed();
                    else if (e.cbutton.button == SDL_BTN_L1)
                        doL1_Pressed();
                    else if (e.cbutton.button == SDL_BTN_R1)
                        doR1_Pressed();
                    else if (e.cbutton.button == SDL_BTN_L2)
                        doL2_Pressed();
                    else if (e.cbutton.button == SDL_BTN_R2)
                        doR2_Pressed();
                    break;

                case SDL_CONTROLLERBUTTONUP:
                    if (e.cbutton.button == SDL_BTN_CROSS)
                        doCross_Released();
                    else if (e.cbutton.button == SDL_BTN_CIRCLE)
                        doCircle_Released();
                    else if (e.cbutton.button == SDL_BTN_TRIANGLE)
                        doTriangle_Released();
                    else if (e.cbutton.button == SDL_BTN_SQUARE)
                        doSquare_Released();
                    else if (e.cbutton.button == SDL_BTN_START)
                        doStart_Released();
                    else if (e.cbutton.button == SDL_BTN_SELECT)
                        doSelect_Released();
                    else if (e.cbutton.button == SDL_BTN_L1)
                        doL1_Released();
                    else if (e.cbutton.button == SDL_BTN_R1)
                        doR1_Released();
                    else if (e.cbutton.button == SDL_BTN_L2)
                        doL2_Released();
                    else if (e.cbutton.button == SDL_BTN_R2)
                        doR2_Released();
                    break;

                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_UP)
                        doKeyUp();
                    else if (e.key.keysym.sym == SDLK_DOWN)
                        doKeyDown();
                    else if (e.key.keysym.sym == SDLK_RIGHT)
                        doKeyRight();
                    else if (e.key.keysym.sym == SDLK_LEFT)
                        doKeyLeft();
                    else if (e.key.keysym.sym == SDLK_PAGEDOWN)
                        doPageDown();
                    else if (e.key.keysym.sym == SDLK_PAGEUP)
                        doPageUp();
                    else if (e.key.keysym.sym == SDLK_HOME)
                        doHome();
                    else if (e.key.keysym.sym == SDLK_END)
                        doEnd();
                    else if (e.key.keysym.sym == SDLK_RETURN)
                        doEnter();
                    else if (e.key.keysym.sym == SDLK_DELETE)
                        doDelete();
                    else if (e.key.keysym.sym == SDLK_BACKSPACE)
                        doBackspace();
                    else if (e.key.keysym.sym == SDLK_TAB)
                        doTab();
                    else if (e.key.keysym.sym == SDLK_ESCAPE)
                        doEscape();
                    break;
            }
        }
        render();
    }
}

//*******************************
// GuiScreen::handlePowerShutdownAndQuit
//*******************************
// returns true if applicable event type and it was handled
bool GuiScreen::handlePowerShutdownAndQuit(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP || e.key.keysym.sym == SDLK_ESCAPE) {
            gui->drawText(_("POWERING OFF... PLEASE WAIT"));
            Util::powerOff();
            return true;    // but it will never get here
        }
    } else if (e.type == SDL_QUIT) {     // this is for pc Only
        menuVisible = false;
        return true;
    }
    return false;
}

//*******************************
// GuiScreen::fastForwardUntilAnotherEvent
//*******************************
// usage example:
// void doSomeJoyEvent() {
//      do {
//          whatever you want to do on the event
//          render();
//      } while (fastForwardUntilJoyCenter(300);  // repeat every 300 milliseconds
//
bool GuiScreen::fastForwardUntilAnotherEvent(Uint32 ticksPerFastForwardRepeat) {
    SDL_Event e;
    Uint32 startTicks = SDL_GetTicks();
    while (true) {
        Uint32 time = SDL_GetTicks() - startTicks;
        if (time >= ticksPerFastForwardRepeat) {
            return true;    // fast forward - repeat key
        } else {
            SDL_PumpEvents();
            int ret = SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_CONTROLLERAXISMOTION, SDL_CONTROLLERDEVICEREMAPPED);
            ret += SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_CONTROLLERHATMOTIONUP, SDL_CONTROLLERHATMOTIONDOWN);
            if (ret > 0) {          // if there is an event in the queue
                return false;   // exit fast forward mode
            }
        }
    }
}



