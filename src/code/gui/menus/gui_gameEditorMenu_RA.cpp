//
// Created by steve on 3/27/22.
//

#include "gui_gameEditorMenu_RA.h"
#include "../gui.h"
#include "../gui_keyboard.h"
#include "../gui_selectmemcard.h"
#include "../../engine/memcard.h"
#include "../../engine/cfgprocessor.h"
#include <SDL2/SDL_image.h>
#include "../../lang.h"
#include <sstream>
#include "../../environment.h"
#include "../../LightgunGames.h"
#include <iostream>
#include "../../launcher/ra_integrator.h"

using namespace std;

#define OPT_FIRST           5
#define OPT_LIGHTGUN        5
#define OPT_LAST            5

//*******************************
// GuiEditor_RA::processOptionChange
//*******************************
void GuiEditor_RA::processOptionChange(bool direction) {
    shared_ptr<Gui> gui(Gui::getInstance());

    stringstream ss;
    string s;

    switch (selOption) {
        case OPT_LIGHTGUN:
        {
            auto &lightgunGames = Gui::getInstance()->lightgunGames;
            bool isLightgun = lightgunGames.IsGameALightgunGame(gameData);

            if (direction == true) {
                if (isLightgun == false) {
                    lightgunGames.AddGame(gameData);
                    isLightgun = true;
                }
            } else {
                if (isLightgun == true) {
                    lightgunGames.RemoveGame(gameData);
                    isLightgun = false;
                }
            }
        }
            break;
    }
}

//*******************************
// GuiEditor_RA::refreshData
//*******************************
void GuiEditor_RA::refreshData() {
    shared_ptr<Gui> gui(Gui::getInstance());
}

//*******************************
// GuiEditor_RA::GetBoxArtTexture
//*******************************
SDL_Shared<SDL_Texture> GuiEditor_RA::GetBoxArtTexture() {
    auto makeBoxArtPath = [&] (const string& boxartDir) -> string
    { return Env::getPathToRetroarchDir() + sep + "thumbnails" + sep +
             DirEntry::getFileNameWithoutExtension(gameData->db_name) + sep +
             boxartDir + sep + RAIntegrator::escapeName(gameData->title) + ".png";
    };

    SDL_Shared<SDL_Texture> coverPng;
    string imagePath = makeBoxArtPath("Named_Boxarts");
    string imagePath2 = makeBoxArtPath("Named_Titles");
    string imagePath3 = makeBoxArtPath("Named_Snaps");
    if (DirEntry::exists(imagePath)) {
        coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
    } else if (DirEntry::exists(imagePath2)) {
        imagePath = imagePath2;
        coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
    } else if (DirEntry::exists(imagePath3)) {
        imagePath = imagePath3;
        coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
    } else {
        // use default
        cout << "boxart image NOT found for " << imagePath << endl;
        coverPng = IMG_LoadTexture(renderer, (Env::getWorkingPath() + sep + "evoimg/ra-cover.png").c_str());
    }

    return coverPng;
}

//*******************************
// GuiEditor_RA::init
//*******************************
void GuiEditor_RA::init() {
    shared_ptr<Gui> gui(Gui::getInstance());
    bool pngLoaded = false;

    cover = GetBoxArtTexture();

    refreshData();
}

//*******************************
// GuiEditor_RA::render
//*******************************
void GuiEditor_RA::render() {
    shared_ptr<Gui> gui(Gui::getInstance());

    int line = 0;
    gui->renderBackground();
    gui->renderTextBar();
    int yoffset = gui->renderLogo(true);

    // Game.ini
    gui->renderTextLine(_("Title:") + " " + gameData->title, line++, yoffset, XALIGN_CENTER);
    gui->renderTextLine(_("Game File:") + " " + gameData->image_path, line++, yoffset, XALIGN_CENTER);
    gui->renderTextLine(_("Game Core:") + " " + gameData->core_name, line++, yoffset, XALIGN_CENTER);

    gui->renderTextLineOptions(
            _("Lightgun Game:") + (Gui::getInstance()->lightgunGames.IsGameALightgunGame(gameData) ? string("|@Check|") : string("|@Uncheck|")),
            OPT_LIGHTGUN, yoffset, XALIGN_LEFT, 300);

    gui->renderSelectionBox(selOption, yoffset, 300);

    //string guiMenu = "|@T| " + _("Rename");

    string guiMenu = " |@O| " + _("Go back") + "|";

    gui->renderStatus(guiMenu);

    // ******************************************************************************
    // display RA cover here
    // ******************************************************************************

    Uint32 format;
    int access;
    int cover_w, cover_h;
    SDL_QueryTexture(cover, &format, &access, &cover_w, &cover_h);

    // calculate output rect with aspect ratio
    int biggerSize = cover_w > cover_h ? cover_w : cover_h;
    float magnify = 226.0 / biggerSize;

    SDL_Rect outputRect;
    outputRect.x = atoi(gui->themeData.values["ecoverx"].c_str());
    outputRect.y = atoi(gui->themeData.values["ecovery"].c_str());
    outputRect.w = cover_w * magnify;
    outputRect.h = cover_h * magnify;
    outputRect.x += (226-outputRect.w)/2;
    outputRect.y += (226-outputRect.h)/2;

    SDL_RenderCopy(renderer, cover, NULL, &outputRect);

    SDL_RenderPresent(renderer);
}

//*******************************
// GuiEditor_RA::loop
//*******************************
void GuiEditor_RA::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
    menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            gui->mapper.handleHotPlug(&e);
            gui->mapper.handlePowerBtn(&e);
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
                case SDL_CONTROLLERHATMOTIONDOWN:  /* Handle Joystick Motion */
                case SDL_CONTROLLERHATMOTIONUP:

                    if (gui->mapper.isDown(&e)) {
                        do {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selOption++;
                            if (selOption > OPT_LAST) {
                                selOption = OPT_LAST;
                            }
                            render();
                        } while (fastForwardUntilAnotherEvent(120));
                    }
                    if (gui->mapper.isUp(&e)) {
                        do {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selOption--;
                            if (selOption < OPT_FIRST) {
                                selOption = OPT_FIRST;
                            }
                            render();
                        } while (fastForwardUntilAnotherEvent(120));
                    }


                    if (gui->mapper.isRight(&e)) {
                        do {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            processOptionChange(true);
                            render();
                        } while (fastForwardUntilAnotherEvent(80));
                    }
                    if (gui->mapper.isLeft(&e)) {
                        do {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            processOptionChange(false);
                            render();
                        } while (fastForwardUntilAnotherEvent(80));
                    }
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                    if (e.cbutton.button == SDL_BTN_CIRCLE) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        cover = nullptr;
                        menuVisible = false;

                    };
            }
        }
        render();
    }
}
