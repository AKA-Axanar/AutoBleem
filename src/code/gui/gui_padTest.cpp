
#include "gui_padTest.h"
#include <unistd.h>
#include <SDL2/SDL.h>
#include <string>
#include "gui.h"
#include "../lang.h"
#include <iostream>
#include <sstream>

using namespace std;

//*******************************
// GuiPadTest::init
//*******************************
void GuiPadTest::init() {
    GuiScrollWin::init();
    if (joyid != -1) {
        SDL_Joystick *joy = SDL_JoystickFromInstanceID(joyid);

        appendLine("-=" + _("New GamePad found") + "=-");
        appendLine(SDL_JoystickName(joy));
    }
    appendLine("Hold down three buttons to exit");
}

//*******************************
// GuiPadTest::loop
//*******************************
void GuiPadTest::loop() {
    // eat any events in the queue
    SDL_Event e;
    while (AB_PollEvent(&e))
        ;

    shared_ptr<Gui> gui(Gui::getInstance());
    int buttonDownCount = 0;
    while (buttonDownCount >= 0 && buttonDownCount < 3) {
        gui->watchJoystickPort();
        SDL_Event e;
        auto status = AB_PollEvent(&e);
        if (status) {
            if (e.type == SDL_KEYDOWN) {
                appendLine("SDL_KEYDOWN = " + to_string(e.key.keysym.scancode));
            } else if (e.type == SDL_KEYUP) {
                appendLine("SDL_KEYUP = " + to_string(e.key.keysym.scancode));

            } else if (e.type == AB_CONTROLLERBUTTONDOWN) {
                appendLine("AB_CONTROLLERBUTTONDOWN = " + to_string(e.cbutton.button));
                buttonDownCount++;
            } else if (e.type == AB_CONTROLLERBUTTONUP) {
                appendLine("AB_CONTROLLERBUTTONUP = " + to_string(e.cbutton.button));
                if (buttonDownCount > 0)
                    buttonDownCount--;

            } else if (e.type == AB_HATMOTIONUP) {
                appendLine("AB_HATMOTIONUP = " + to_string(e.jhat.value));
            } else if (e.type == AB_HATMOTIONDOWN) {
#if 0
                // controller on valium
                if (e.jaxis.value == -32768 || e.jaxis.value == 32767 || e.jaxis.value == 0 ||
                    e.jaxis.value == -1 || e.jaxis.value == 1) {
                    appendLine("AB_HATMOTIONDOWN = " + to_string(e.jaxis.axis) + ", " + to_string(e.jaxis.value));
                }
#else
                appendLine("AB_HATMOTIONDOWN = " + to_string(e.jaxis.axis) + ", " + to_string(e.jaxis.value));
#endif
            } else if (e.type == SDL_MOUSEMOTION ||
                       e.type == SDL_MOUSEBUTTONDOWN ||
                       e.type == SDL_MOUSEBUTTONUP ||
                       e.type == SDL_MOUSEWHEEL ||
                       e.type == SDL_WINDOWEVENT
                      ) {
                ; // ignore
            } else {
                stringstream strhex;
                strhex << std::hex << e.type;
                appendLine("something else = 0x" + strhex.str());
            }
            render();
        }
    }
    appendLine("Release all buttons now");
    sleep(3);
    while (AB_PollEvent(&e))
        ;   // eat any events in the queue
}
