#pragma once

#include "gui_font_wrapper.h"
#include "gui_sdl_wrapper.h"
#include <map>

enum FontEnum {
    FONT_15_BOLD,
    FONT_20_BOLD,
    FONT_22_MED,
    FONT_28_BOLD,
};
enum FontType { FONT_MED, FONT_BOLD };

//********************
// Fonts
//********************
class Fonts {
    std::string rootPath;
    std::string medPath;
    std::string boldPath;

    struct FontInfo {
        FontEnum    fontEnum;
        int         size;
        FontType    fontType;
    };

    static FontInfo allFontInfos[];

    std::map<FontEnum, FC_Font_Shared> fonts;
    std::map<FontEnum, FontInfo> fontInfos;

public:
    Fonts();

    // use operator [] to get or set the shared font
    FC_Font_Shared & operator [] (FontEnum size) { return fonts[size]; }

    static FC_Font_Shared openNewSharedCachedFont(const std::string &filename, int fontSize, SDL_Shared<SDL_Renderer> renderer);
    static FC_Font_Shared openSpecificSharedCachedFont(FontType type, int fontSize);

    //static TTF_Font_Shared openNewSharedTTFFont(const std::string &filename, int fontSize);

    // in gui_launcher.cpp this call is used to change all the fonts to use the fonts in the current theme
    void openAllFonts(const std::string &_rootPath, SDL_Shared<SDL_Renderer> renderer);
};

//********************
// SizesOfBoldThemeFont
// If you ever need to change this to handle both bold and medium fonts change the map key to pair<FontType, pointSize>
// This class is used by ps_meta.cpp to make the game title font smaller if the game name is do long that it 
// displays beyond the right edge of the screen.
//********************
class SizesOfBoldThemeFont {
    std::map<int, FC_Font_Shared> boldFonts;

public:
    SizesOfBoldThemeFont() {};
    void Init() { boldFonts.clear(); }
    FC_Font_Shared AddFont(int size, FC_Font_Shared boldFont);
    FC_Font_Shared GetFont(int size, const Fonts& fonts);
};

using FC_Point = SDL_Point;
struct FC_Size { int w=0, h=0; };
