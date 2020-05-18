//
// Created by screemer on 2019-01-24.
//

#include "gui_about.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "../lang.h"
#include "../engine/scanner.h"
#include "../environment.h"

void GuiAbout::init() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    fx.renderer = renderer;
    font = Fonts::openNewSharedCachedFont(Env::getWorkingPath() + sep + "about.ttf", 17, renderer);
    logo = IMG_LoadTexture(renderer, (Env::getWorkingPath() + sep + "ablogo.png").c_str());
}

//*******************************
// GuiAbout::render
//*******************************
void GuiAbout::render() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    vector<string> credits = {gui->cfg.inifile.values["version"], " ",
                              _(".-= Code C++ and shell scripts =-."),
                              "screemer, Axanar, mGGk, nex, genderbent",
                              _(".-= Graphics =-."),
                              "KaonashiFTW, GeekAndy, rubixcube6, NewbornfromHell",
                              _(".-= Testing =-."),
                              "MagnusRC, xboxiso, Azazel, Solidius, SupaSAIAN, Kingherb, saptis",
                              _(".-= Database maintenance =-."),
                              "Screemer,Kingherb",
                              _(".-= Localization support =-."),
                              "nex(German), Azazel(Polish), gadsby(Turkish), GeekAndy(Dutch), Pardubak(Slovak), SupaSAIAN(Spanish), Mate(Czech)",
                              "Sasha(Italian), Jakejj(BR_Portuguese), jolny(Swedish), StepJefli(Danish), alucard73 / MagnusRC(French), Quenti(Occitan), ",
                              _(".-= Retroboot and emulation cores =-."),
                              "genderbent, KMFDManic"," ",
                              _("Support via Discord:") + " https://discord.gg/AHUS3RM",
                              _("This is free software. It works AS IS and We take no responsibility for any issues or damage."),
                              //_("Download latest:") + " https://github.com/autobleem/AutoBleem"
    };


    gui->renderBackground();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 235);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2;
    rect2.x = 0;
    rect2.y = 0;
    rect2.w = SCREEN_WIDTH;
    rect2.h = SCREEN_HEIGHT;

    SDL_RenderFillRect(renderer, &rect2);

    fx.render();

    int yoffset = 150;
    SDL_Rect rect;
    rect.x = SCREEN_WIDTH/2-100;
    rect.y = 5;
    rect.w = 200;
    rect.h = 141;
    SDL_RenderCopy(renderer, logo, nullptr, &rect);


    int line = 1;
    for (const string &s:credits) {
        gui->renderTextLine(s, line, yoffset, XALIGN_CENTER, 0, font);
        line++;
    }

    gui->renderStatus("|@O| " + _("Go back") + "|",680);
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiAbout::loop
//*******************************
void GuiAbout::loop() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    menuVisible = true;
    while (menuVisible) {
        render();
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            gui->mapper.handleHotPlug(&e);
            gui->mapper.handlePowerBtn(&e);

            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_CONTROLLERBUTTONDOWN:
                    if (e.cbutton.button == SDL_BTN_CIRCLE) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;

                    };


            }

        }
    }
}
