#include "gui_font.h"
#include <iostream>
#include "../util.h"
#include <cassert>
#include "../DirEntry.h"
#include "gui.h"

using namespace std;

//********************
// static Fonts::allFontInfos
//********************
Fonts::FontInfo Fonts::allFontInfos[] = {
        { FONT_15_BOLD, 15, FONT_BOLD },
        { FONT_20_BOLD, 20, FONT_BOLD},
        { FONT_22_MED,  22, FONT_MED },
        { FONT_28_BOLD, 28, FONT_BOLD }
};

//********************
// Fonts::Fonts
//********************
Fonts::Fonts() { }

//********************
// Fonts::openNewSharedCachedFont
// low level open shared font.  filename is the full path to the ttf file.  fontSize is the font point size.
//********************
FC_Font_Shared Fonts::openNewSharedCachedFont(const string &filename, int fontSize, SDL_Shared<SDL_Renderer> renderer) {
    FC_Font* fc_font = FC_CreateFont();
    FC_LoadFont(fc_font, renderer, filename.c_str(), fontSize, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
    FC_Font_Shared font = FC_Font_Shared(fc_font);
    if (font) {
        //cout << "Success opening font " << filename << " of size " << fontSize << endl;
    } else {
        cout << "FAILURE opening font " << filename << " of size " << fontSize << endl;
        font = nullptr;
        assert(false);
    }

    return font;
}


//********************
// Fonts::openSpecificSharedCachedFont
// low level open shared font.  type is the font type (FONT_MED, FONT_BOLD).  fontSize is the font point size.
//********************
FC_Font_Shared Fonts::openSpecificSharedCachedFont(FontType type, int fontSize) {
    auto gui = Gui::getInstance();
    auto renderer = gui->renderer;

    string rootPath = gui->getCurrentThemeFontPath();
    string fontPath;
    if (type == FONT_MED)
        fontPath = rootPath + sep + "SST-Medium.ttf";
    else
        fontPath = rootPath + sep + "SST-Bold.ttf";

    FC_Font* fc_font = FC_CreateFont();
    FC_LoadFont(fc_font, renderer, fontPath.c_str(), fontSize, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
    FC_Font_Shared font = FC_Font_Shared(fc_font);
    if (font) {
        //cout << "Success opening font " << fontPath << " of size " << fontSize << endl;
    } else {
        cout << "FAILURE opening font " << fontPath << " of size " << fontSize << endl;
        font = nullptr;
        assert(false);
    }

    return font;
}

//********************
// Fonts::openAllFonts
//********************
void Fonts::openAllFonts(const std::string &_rootPath, SDL_Shared<SDL_Renderer> renderer) {
    fonts.clear();
    rootPath = _rootPath;
    medPath = rootPath + sep + "SST-Medium.ttf";
    boldPath = rootPath + sep + "SST-Bold.ttf";

    for (auto fontInfo : allFontInfos) {
        string path;
        if (fontInfo.fontType == FONT_MED)
            path = medPath;
        else
            path = boldPath;
        fonts[fontInfo.fontEnum] = openNewSharedCachedFont(path, fontInfo.size, renderer);
        fontInfos[fontInfo.fontEnum] = fontInfo;
    }
}

//********************
// SizesOfBoldThemeFont::AddFont
// If you ever need to change this to handle both bold and medium fonts change the map key to pair<FontType, pointSize>
// This class is used by ps_meta.cpp to make the game title font smaller if the game name is do long that it 
// displays beyond the right edge of the screen.
//********************

//********************
// SizesOfBoldThemeFont::AddFont
//********************
FC_Font_Shared SizesOfBoldThemeFont::AddFont(int size, FC_Font_Shared font)
{
    auto it = boldFonts.find(size);
    if (it != boldFonts.end())
        return it->second;
    else {
        boldFonts[size] = font; // add the passed font as a new font size font
        return font;
    }
}

//********************
// SizesOfBoldThemeFont::GetFont
//********************
FC_Font_Shared SizesOfBoldThemeFont::GetFont(int size, const Fonts& fonts)
{
    auto it = boldFonts.find(size);
    if (it != boldFonts.end())
        return it->second;     // we already have that size
    else {
        FC_Font_Shared font = fonts.openSpecificSharedCachedFont(FONT_BOLD, size);
        boldFonts[size] = font;
        return font;
    }
}

