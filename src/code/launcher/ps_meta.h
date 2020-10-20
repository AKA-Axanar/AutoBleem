//
// Created by screemer on 2/12/19.
//

#pragma once

#include "ps_obj.h"
#include "ps_game.h"
#include <SDL2/SDL_ttf.h>
#include "../gui/gui_sdl_wrapper.h"
#include "../gui/gui_font_wrapper.h"
#include "../gui/gui_font.h"

class PsGame;

//******************
// PsMeta
//******************
class PsMeta : public PsObj {
public:
    std::string gameName;
    std::string publisher;
    std::string year;
    std::string players;
    std::string serial;
    std::string region;
    std::string last_played;
    Fonts fonts;

    SDL_Shared<SDL_Texture> discsTex;
    SDL_Shared<SDL_Texture> gameNameTex;
    SDL_Shared<SDL_Texture> publisherAndYearTex;
    SDL_Shared<SDL_Texture> serialAndRegionTex;
    SDL_Shared<SDL_Texture> playersTex;
    SDL_Shared<SDL_Texture> datePlayedTex;

    SDL_Shared<SDL_Texture> internalOnTex;
    SDL_Shared<SDL_Texture> internalOffTex;
    SDL_Shared<SDL_Texture> hdOnTex;
    SDL_Shared<SDL_Texture> hdOffTex;
    SDL_Shared<SDL_Texture> lockOnTex;
    SDL_Shared<SDL_Texture> lockOffTex;
    SDL_Shared<SDL_Texture> cdTex;
    SDL_Shared<SDL_Texture> favoriteTex;
    SDL_Shared<SDL_Texture> raTex;

    int nextPos = 0;
    int prevPos = 0;
    long animEndTime = 0;
    long animStarted = 0;

    bool internal = false;
    bool hd = false;
    bool locked = false;
    bool discs = 1;
    bool favorite = false;
    bool play_using_ra = false;
    bool foreign = false;
    bool app = false;

    void updateTexts(const std::string & gameNameTxt, const std::string & publisherTxt,
                     const std::string & yearTxt, const std::string & serial, const std::string & region,
                     const std::string & playersTxt, bool internal, bool hd, bool locked, int discs, bool favorite,
                     bool play_using_ra, bool foreign, bool app, const std::string& last_played,
                     int r, int g, int b);

    void updateTexts(PsGamePtr & game, int r, int g, int b);

    void destroy();

    void render();

    void update(long time);

    using PsObj::PsObj;
private:
    SDL_Shared<SDL_Texture> createTextTex(const std::string & text, Uint8 r, Uint8 g, Uint8 b, TTF_Font_Shared font);
};
