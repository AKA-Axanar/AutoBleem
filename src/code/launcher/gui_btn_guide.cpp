//
// Created by screemer on 2019-03-02.
//

#include "gui_btn_guide.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "../gui/gui.h"
#include "../lang.h"
#include "../engine/scanner.h"

using namespace std;

//*******************************
// GuiBtnGuide::render
//*******************************
void GuiBtnGuide::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();

    gui->renderTextBar();
    // -50 is because the "button guide" line is too low and the lines go below the bottom of the rectangle
    int offset = gui->renderLogo(true) - 50;

    int xLeft = 300;
    int xRight = 520;
    int line = 0;
    auto font = gui->sonyFonts[FONT_20_BOLD];

    auto renderTextLineToColumns = [&] (const string &textLeft, const string &textRight) {
        gui->renderTextLineToColumns(textLeft, textRight, xLeft, xRight, line++, offset, font);
    };

    renderTextLineToColumns("",                          "-=" + _("Button Guide") + "=-");
    line++;
    renderTextLineToColumns("|@X| / |@O|",               _("Select or cancel highlighted option"));
    renderTextLineToColumns("|@S|",                      _("Run using RetroBoot"));
    renderTextLineToColumns("|@R1| / |@L1|",             _("Quick scroll to next letter"));
    renderTextLineToColumns("|@Start|",                  _("Random Game"));
    renderTextLineToColumns("|@Select|",                 _("Games filter"));
    renderTextLineToColumns("|@L2| + |@Select|",         _("Change USB Games Sub-Directory"));
    renderTextLineToColumns("|@L2| + |@Select|",              _("Change RetroBoot System"));
    line++;
    renderTextLineToColumns("",                          "-=" + _("In Game") + "=-");
    renderTextLineToColumns("|@Select| + |@T|",          _("Emulator config MENU"));
    renderTextLineToColumns(_("RESET"),                  _("Quit emulation - back to AutoBleem"));
    line++;
    renderTextLineToColumns("",                          "-=" + _("In Retroarch Game") + "=-");
    renderTextLineToColumns("|@Select| + |@Start|",      _("Open Retroarch Menu"));
    renderTextLineToColumns(_("POWER"),                  _("Exit to EvoUI"));
    line++;
    renderTextLineToColumns("|@L2| + |@R2|",             _("In Boot Menu: Safe Power Off The Console"));

    gui->renderStatus("|@O| " + _("Go back") + "|");
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiBtnGuide::loop
//*******************************
void GuiBtnGuide::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
    menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == AB_CONTROLLERDEVICEADDED)
            {
                gui->registerPad(e.cdevice.which);
            }
            if (e.type == AB_CONTROLLERDEVICEREMOVED)
            {
                gui->removePad(e.cdevice.which);
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP || e.key.keysym.sym == SDLK_ESCAPE) {
                    gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();

                }
            }
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case AB_CONTROLLERBUTTONUP:


                    if (e.cbutton.button == AB_BTN_CIRCLE) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;

                    };


            }

        }
    }
}
