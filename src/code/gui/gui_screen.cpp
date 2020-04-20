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
        gui->watchJoystickPort();
        SDL_Event e;

        while (AB_PollEvent(&e)) {
            if (handlePowerShutdownAndQuit(e))
                continue;

            switch (e.type) {
                case AB_HATMOTIONDOWN:
                case AB_HATMOTIONUP:

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

                case AB_CONTROLLERBUTTONDOWN:
                    if (e.cbutton.button == AB_BTN_CROSS)
                        doCross_Pressed();
                    else if (e.cbutton.button == AB_BTN_CIRCLE)
                        doCircle_Pressed();
                    else if (e.cbutton.button == AB_BTN_TRIANGLE)
                        doTriangle_Pressed();
                    else if (e.cbutton.button == AB_BTN_SQUARE)
                        doSquare_Pressed();
                    else if (e.cbutton.button == AB_BTN_START)
                        doStart_Pressed();
                    else if (e.cbutton.button == AB_BTN_SELECT)
                        doSelect_Pressed();
                    else if (e.cbutton.button == AB_BTN_L1)
                        doL1_Pressed();
                    else if (e.cbutton.button == AB_BTN_R1)
                        doR1_Pressed();
                    else if (e.cbutton.button == AB_BTN_L2)
                        doL2_Pressed();
                    else if (e.cbutton.button == AB_BTN_R2)
                        doR2_Pressed();
                    break;

                case AB_CONTROLLERBUTTONUP:
                    if (e.cbutton.button == AB_BTN_CROSS)
                        doCross_Released();
                    else if (e.cbutton.button == AB_BTN_CIRCLE)
                        doCircle_Released();
                    else if (e.cbutton.button == AB_BTN_TRIANGLE)
                        doTriangle_Released();
                    else if (e.cbutton.button == AB_BTN_SQUARE)
                        doSquare_Released();
                    else if (e.cbutton.button == AB_BTN_START)
                        doStart_Released();
                    else if (e.cbutton.button == AB_BTN_SELECT)
                        doSelect_Released();
                    else if (e.cbutton.button == AB_BTN_L1)
                        doL1_Released();
                    else if (e.cbutton.button == AB_BTN_R1)
                        doR1_Released();
                    else if (e.cbutton.button == AB_BTN_L2)
                        doL2_Released();
                    else if (e.cbutton.button == AB_BTN_R2)
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
        if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
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
            int ret = SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
            if (ret > 0) {          // if there is an event in the queue
                return false;   // exit fast forward mode
            }
        }
    }
}

#if 0
//*******************************
// GuiScreen::fastForwardUntilJoyCenter
//*******************************
// usage example:
// void doSomeJoyEvent() {
//      do {
//          whatever you want to do on the event
//          render();
//      } while (fastForwardUntilJoyCenter(300);  // repeat every 300 milliseconds
//
bool GuiScreen::fastForwardUntilJoyCenter(Uint32 ticksPerFastForwardRepeat) {
    SDL_Event e;
    Uint32 startTicks = SDL_GetTicks();
    cout << "start " << startTicks << endl;
    while (true) {
        Uint32 time = SDL_GetTicks() - startTicks;
        cout << "time " << time << endl;
        if (time >= ticksPerFastForwardRepeat) {
            cout << "return true" << endl;
            return true;    // fast forward - repeat key
        } else {
            SDL_PumpEvents();
            int ret = SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
            if (ret > 0) {          // if there is an event in the queue
                AB_PollEvent(&e);  // eat the event
                cout << "event type " << hex << e.type << dec << endl;
                if (e.type == AB_HATMOTIONDOWN || e.type == AB_HATMOTIONUP) {
                    if (gui->mapper.isCenter(&e)) {
                        cout << "return false" << endl;
                        return false;   // exit fast forward mode
                    }
                    else
                        cout << "not center. is = " << e.jaxis.value << endl;
                }
            }
        }
    }
}

//*******************************
// GuiScreen::fastForwardUntilButtonReleased
//*******************************
// usage example:
// void doSomeJoyEvent() {
//      do {
//          whatever you want to do on the event
//          render();
//      } while (fastForwardUntilButtonReleased(button, 300);  // repeat every 300 milliseconds
//
bool GuiScreen::fastForwardUntilButtonReleased(int button, Uint32 ticksPerFastForwardRepeat) {
    SDL_Event e;
    Uint32 startTicks = SDL_GetTicks();
    cout << "start " << startTicks << endl;
    while (true) {
        Uint32 time = SDL_GetTicks() - startTicks;
        cout << "time " << time << endl;
        if (time >= ticksPerFastForwardRepeat) {
            cout << "return true" << endl;
            return true;    // fast forward - repeat key
        } else {
            SDL_PumpEvents();
            int ret = SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
            if (ret > 0) {          // if there is an event in the queue
                AB_PollEvent(&e);  // eat the event
                cout << "event type " << hex << e.type << dec << endl;
                if ((e.type == AB_CONTROLLERBUTTONUP) && (e.cbutton.button == gui->_cb(button, &e))) {
                    cout << "return false" << endl;
                    return false;   // exit fast forward mode
                }
                else
                    cout << "wrong event" << endl;
            }
        }
    }
}
#endif

//*******************************
// GuiScreen::countMoreJoyPressesInQueue
//*******************************
// by the time the render() finishes the user may have already pushed the joy button one or more times
// direction = DIR_UP, DIR_DOWN, etc from padmapper.h
int GuiScreen::countMoreJoyPressesInQueue(int direction) {
    int count = 0;
    while (true) {
      SDL_Event e;
      SDL_PumpEvents();     // needed before peek
      int ret = SDL_PeepEvents(&e, 1, SDL_PEEKEVENT , SDL_FIRSTEVENT, SDL_LASTEVENT);   // look in the queue
      if (ret > 0 && (e.type == AB_HATMOTIONDOWN || e.type == AB_HATMOTIONUP)) {
          if (gui->mapper.isDirection(&e, direction)) {
              count++;
              SDL_Event e2;
              AB_PollEvent(&e2);  // remove the event from the queue
          } else if (gui->mapper.isDirection(&e, DIR_NONE)) {
              SDL_Event e2;
              AB_PollEvent(&e2);  // remove the event from the queue
          } else {
              return count; // done. not the direction we're looking for.
          }
      } else {
          return count; // done. no event in queue or not the event we're looking for.
      }
    }
}
