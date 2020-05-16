//
// Created by screemer on 2/11/19.
//

#include "ps_obj.h"
#include <SDL2/SDL_image.h>
using namespace std;

//*******************************
// PsObj::PsObj
//*******************************
PsObj::PsObj(string name1, string texPath) : name(name1) {
    gui = Gui::getInstance();
    renderer = gui->renderer;

    if (texPath != "") {
        load(texPath);
    }
}

//*******************************
// PsObj::load
//*******************************
void PsObj::load(const string & imagePath) {
    tex = IMG_LoadTexture(renderer, imagePath.c_str());
    Uint32 format;
    int access;

    SDL_QueryTexture(tex, &format, &access, &w, &h);
    x = 0, y = 0;
    ow = w;
    oh = h;
}

//*******************************
// PsObj::destroy
//*******************************
void PsObj::destroy() {
}

//*******************************
// PsObj::render
//*******************************
void PsObj::render() {
    if (visible) {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        SDL_Rect fullRect;
        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, tex, &fullRect, &rect);
    }
}