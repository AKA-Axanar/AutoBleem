#include "gui_font.h"
#include <iostream>
#include "../util.h"
#include <assert.h>
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
Fonts::Fonts() {
//    // insert an empty TTF_Font_Shared pointing to nullptr for now
//    for (auto fontInfo : allFontInfos)
//        fonts.emplace(fontInfo.fontEnum, TTF_Font_Shared());
}

//********************
// Fonts::openNewSharedFont
// low level open shared font.  filename is the full path to the ttf file.  fontSize is the font point size.
//********************
TTF_Font_Shared Fonts::openNewSharedFont(const string &filename, int fontSize) {
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

//********************
// Fonts::openAllBasicSonyFonts
// this will get the four basic fonts in the sony/font dir
//********************
void Fonts::openAllBasicSonyFonts(Gui *gui, const std::string& sonyRootPath) {
    fonts.clear();
    rootPath = sonyRootPath;
    medPath = rootPath + sep + "font" + sep + "SST-Medium.ttf";
    boldPath = rootPath + sep + "font" + sep + "SST-Bold.ttf";

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
// Fonts::void Fonts::openAllBasicThemeFonts
// this will look for the four basic fonts in the theme font dir but if not found there will use the sony font
//********************
void Fonts::openAllBasicThemeFonts(Gui *gui, const std::string& themeRootPath) {
    fonts.clear();
    rootPath = themeRootPath;
    medPath = gui->getCurrentThemeFontFile("SST-Medium.ttf", themeRootPath);
    boldPath = gui->getCurrentThemeFontFile("SST-Bold.ttf", themeRootPath);

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
