//
// Created by screemer on 2019-02-21.
//

#pragma once

#include "ps_obj.h"
#include "../gui/gui_font_wrapper.h"
#include "../gui/gui_font.h"

//******************
// PsCenterLabel
//******************
class PsCenterLabel : public PsObj {
public:
    std::string text;
    FC_Font_Shared font;
    SDL_Color textColor;
    FC_Size textSize;

    void render();

    void setText(const std::string & _text, SDL_Color _textColor);

    PsCenterLabel(SDL_Shared<SDL_Renderer> renderer1, const std::string & name1, const std::string & texPath = "");
    ~PsCenterLabel();
};
