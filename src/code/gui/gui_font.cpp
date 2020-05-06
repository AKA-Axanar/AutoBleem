#include "gui_font.h"
#include <iostream>
#include "../util.h"
#include <cassert>
#include "../DirEntry.h"

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

#if 0
//********************
// Fonts::Fonts
//********************
Fonts::Fonts() { }

//********************
// Fonts::openNewSharedFont
// low level open shared font.  filename is the full path to the ttf file.  fontSize is the font point size.
//********************
FC_Font_Shared Fonts::openNewSharedFont(const string &filename, int fontSize) {
    FC_Font_Shared font = FC_Font_Shared(TTF_OpenFont(filename.c_str(), fontSize));
    if (font) {
        cout << "Success opening font " << filename << " of size " << fontSize << endl;
    } else {
        cout << "FAILURE opening font " << filename << " of size " << fontSize << endl;
        font = nullptr;
        assert(false);
    }

    return font;
}

//********************
// Fonts::openAllFonts
//********************
void Fonts::openAllFonts(const std::string &_rootPath) {
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
        fonts[fontInfo.fontEnum] = openNewSharedFont(path, fontInfo.size);
        fontInfos[fontInfo.fontEnum] = fontInfo;
    }
}

//********************
// Fonts::openAllFonts
//********************
void Fonts::openAllFonts(const std::string &_rootPath) {
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
        fonts[fontInfo.fontEnum] = openNewSharedFont(path, fontInfo.size);
        fontInfos[fontInfo.fontEnum] = fontInfo;
    }
}
#endif

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
    FC_LoadFont(fc_font, renderer, filename.c_str(), fontSize, FC_MakeColor(0,0,0,255), TTF_STYLE_NORMAL);
    FC_Font_Shared font = FC_Font_Shared(fc_font);
    if (font) {
        cout << "Success opening font " << filename << " of size " << fontSize << endl;
    } else {
        cout << "FAILURE opening font " << filename << " of size " << fontSize << endl;
        font = nullptr;
        assert(false);
    }

    return font;
}

#if 0
//********************
// Fonts::openNewSharedTTFFont
// low level open shared font.  filename is the full path to the ttf file.  fontSize is the font point size.
//********************
TTF_Font_Shared Fonts::openNewSharedTTFFont(const string &filename, int fontSize) {
    TTF_Font_Shared font = TTF_Font_Shared(TTF_OpenFont(filename.c_str(), fontSize));
    if (font) {
        cout << "Success opening font " << filename << " of size " << fontSize << endl;
    } else {
        cout << "FAILURE opening font " << filename << " of size " << fontSize << endl;
        font = nullptr;
        assert(false);
    }

    return font;
}
#endif

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
