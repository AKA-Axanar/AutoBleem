//
// Created by screemer on 2019-01-24.
//

#include "gui_splash.h"
#include "gui.h"
#include "../ver_migration.h"
#include "../lang.h"
#include "../engine/scanner.h"
using namespace std;

//*******************************
// GuiSplash::render
//*******************************
void GuiSplash::render() {
    std::shared_ptr<Gui> gui(Gui::getInstance());
    int w, h; // texture width & height
    SDL_SetTextureBlendMode(gui->backgroundImg, SDL_BLENDMODE_BLEND);
    SDL_QueryTexture(gui->backgroundImg, NULL, NULL, &w, &h);
    gui->backgroundRect.x = 0;
    gui->backgroundRect.y = 0;
    gui->backgroundRect.w = w;
    gui->backgroundRect.h = h;
    SDL_QueryTexture(gui->logo, NULL, NULL, &w, &h);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_SetTextureAlphaMod(gui->backgroundImg, alpha);
    SDL_SetTextureAlphaMod(gui->logo, alpha);
    Mix_VolumeMusic(alpha / 3);

    SDL_RenderCopy(renderer, gui->backgroundImg, NULL, &gui->backgroundRect);
    SDL_RenderCopy(renderer, gui->logo, NULL, &gui->logoRect);

    string bg = gui->themeData.values["text_bg"];

    int bg_alpha = atoi(gui->themeData.values["textalpha"].c_str()) * alpha / 255;

    SDL_SetRenderDrawColor(renderer, gui->getR(bg), gui->getG(bg), gui->getB(bg), bg_alpha);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect = gui->getTextRectOfTheme();
    SDL_RenderFillRect(renderer, &rect);

    int y = atoi(gui->themeData.values["ttop"].c_str());
    string splashText = _("AutoBleem")+" " + gui->cfg.inifile.values["version"];
    gui->renderText(gui->themeFont, splashText, 0, y, XALIGN_CENTER);

    SDL_RenderPresent(renderer);
}

//*******************************
// GuiSplash::loop
//*******************************
void GuiSplash::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());

    Mix_VolumeMusic(0);
    alpha = 0;
    start = SDL_GetTicks();
    while (1) {
        gui->watchJoystickPort();
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();
                }
            }
            if (e.type == SDL_QUIT)
                break;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                break;
        }
        render();
        int current = SDL_GetTicks();
        int time = current - start;
        if (time > 2) {
            if (alpha < 255) {
                alpha += 10;
                if (alpha > 255) {
                    alpha = 255;
                }
            } else {

                break;
            }
            start = SDL_GetTicks();
        }
    }
}