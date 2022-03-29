//
// Created by steve on 3/27/22.
//
#pragma once

#include "../gui_screen.h"
#include "../../engine/inifile.h"
#include "../../launcher/ps_game.h"

//********************
// GuiEditor_RA
//********************
class GuiEditor_RA : public GuiScreen{
public:
    void init();
    void render();
    void loop();

    void refreshData();

    PsGamePtr gameData;

    int selOption = 5;
    bool changes=false;
    SDL_Shared<SDL_Texture> cover;

    void processOptionChange(bool direction);
    SDL_Shared<SDL_Texture> GetBoxArtTexture();

    using GuiScreen::GuiScreen;
};
