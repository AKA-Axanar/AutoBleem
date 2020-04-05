#pragma once

#include "gui_font_wrapper.h"
#include <map>

class GuiBase;

enum FontEnum {
    FONT_15_BOLD,
    FONT_20_BOLD,
    FONT_22_MED,
    FONT_28_BOLD,
};
enum FontType { FONT_MED, FONT_BOLD };

class Fonts {
    std::string rootPath;
    std::string medPath;
    std::string boldPath;

    struct FontInfo {
        FontEnum    fontEnum;
        int size;
        FontType fontType;
    };

    static FontInfo allFontInfos[];

    std::map<FontEnum, TTF_Font_Shared> fonts;
    std::map<FontEnum, FontInfo> fontInfos;

public:
    Fonts();

    // use operator [] to get or set the shared font
    TTF_Font_Shared & operator [] (FontEnum size) { return fonts[size]; }

    static TTF_Font_Shared openNewSharedFont(const std::string &filename, int fontSize);

    // this will get the four basic fonts in the sony/font dir
    void openAllBasicSonyFonts(GuiBase *gui, const std::string& sonyRootPath);
    // this will look for the four basic fonts in the theme font dir but if not found there will use the sony font
    void openAllBasicThemeFonts(GuiBase *gui, const std::string& themeRootPath);
};
