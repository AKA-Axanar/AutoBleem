//
// Created by screemer on 2/12/19.
//

#ifndef AUTOBLEEM_GUI_PS_META_H
#define AUTOBLEEM_GUI_PS_META_H

#include "ps_obj.h"

class PsMeta : public PsObj {
public:

    string gameName;
    string publisher;
    string year;
    string players;

    TTF_Font *font30;
    TTF_Font *font15;
    TTF_Font *font24;

    SDL_Texture *gameNameTex = nullptr;
    SDL_Texture *publisherTex = nullptr;
    SDL_Texture *yearTex = nullptr;
    SDL_Texture *playersTex = nullptr;

    int nextPos = 0;
    int prevPos = 0;
    long animEndTime = 0;
    long animStarted = 0;


    void updateTexts(string gameNameTxt, string publisherTxt,
                     string yearTxt, string playersTxt);

    void destroy();

    void render();

    void update(long time);


    using PsObj::PsObj;
private:
    SDL_Texture *createTextTex(string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font);
};


#endif //AUTOBLEEM_GUI_PS_META_H
