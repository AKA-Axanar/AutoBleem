//
// Created by screemer on 2019-01-24.
//

#ifndef AUTOBLEEM_GUI_GUI_SCREEN_H
#define AUTOBLEEM_GUI_GUI_SCREEN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class GuiScreen {
public:
    virtual void init(){};
    virtual void render()=0;
    virtual void loop()=0;

    void show()
    {
        init();
        render();
        loop();
    }

    GuiScreen(SDL_Renderer * renderer1)
    {
        renderer=renderer1;

    };
    SDL_Renderer * renderer;
};


#endif //AUTOBLEEM_GUI_GUI_SCREEN_H
