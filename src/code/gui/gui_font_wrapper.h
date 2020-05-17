#pragma once
// from https://blog.galowicz.de/2016/02/21/automatic_resource_release_with_sdl/
// and modified to use a class with a conversion operator
// Take care to not call the destroy function elsewhere causing it to be destroyed twice.

#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "SDL_FontCache.h"

//********************
// TTF_Font_Shared
//********************
struct TTF_Font_Shared {
    std::shared_ptr<TTF_Font> font_shared_ptr;

    TTF_Font_Shared(TTF_Font* font = nullptr) : font_shared_ptr(font, [](TTF_Font *font)
        { TTF_CloseFont(font); } ) {};    // automatically destroy/free when the last shared_ptr goes away

    operator TTF_Font* () { return font_shared_ptr.get(); };
    TTF_Font & operator * () { return *font_shared_ptr.get(); };
    TTF_Font * operator -> () { return font_shared_ptr.get(); };
};

//********************
// FC_Font_Shared
//********************
struct FC_Font_Shared {
    std::shared_ptr<FC_Font> font_shared_ptr;

    FC_Font_Shared(FC_Font* font = nullptr) : font_shared_ptr(font, [](FC_Font *font)
    { FC_FreeFont(font); } ) {};    // automatically destroy/free when the last shared_ptr goes away

    operator FC_Font* () { return font_shared_ptr.get(); };
    FC_Font & operator * () { return *font_shared_ptr.get(); };
    FC_Font * operator -> () { return font_shared_ptr.get(); };
};
