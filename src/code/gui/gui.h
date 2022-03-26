//
// Created by screemer on 2018-12-19.
//
#pragma once

#include "abl.h"
#include "../main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>
#include "../engine/database.h"
#include "../engine/config.h"
#include "../engine/coverdb.h"
#include "../engine/scanner.h"
#include "../engine/padmapper.h"
#include "gui_sdl_wrapper.h"
//#include "gui_font_wrapper.h"
#include "gui_font.h"
#include "../environment.h"
#include "../LightgunGames.h"

enum MenuOption { MENU_OPTION_SCAN = 1, MENU_OPTION_RUN, MENU_OPTION_SONY, MENU_OPTION_RETRO, MENU_OPTION_START };

#define EMU_PCSX          0
#define EMU_RETROARCH     1
#define EMU_LAUNCHER      2

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

enum XAlignment { XALIGN_LEFT, XALIGN_CENTER, XALIGN_RIGHT };

// if you add a new set also update setNames in gui_launcher.cpp
#define SET_PS1      0
#define SET_RETROARCH 1
#define SET_APPS 2
#define SET_LAST 2

// SET_PS1 select sub states. keep SET_PS1_Games_Subdir last as it's going to be left off the L2+Select menu
enum { SET_PS1_All_Games=0, SET_PS1_Internal_Only, SET_PS1_Favorites, SET_PS1_History, SET_PS1_Games_Subdir };

//********************
// GuiBase
//********************
class GuiBase {
public:
    SDL_Shared<SDL_Window> window;
    SDL_Shared<SDL_Renderer> renderer;

    Fonts themeFonts;
    Fonts sonyFonts;
    Config cfg;
    bool inGuiLauncher = false;

    std::string getCurrentThemePath();
    std::string getCurrentThemeImagePath();
    std::string getCurrentThemeFontPath();
    std::string getCurrentThemeSoundPath();

    GuiBase();
    ~GuiBase();
};

//********************
// Gui
//********************
class Gui : public GuiBase {
private:

    Gui() { mapper.probePads(); }

    string themePath;

public:
    PadMapper mapper;
    Inifile themeData;
    Inifile defaultData;

    Coverdb *coverdb = nullptr;
    // db and internalDB are set in main.cpp and remain alive until exit
    Database *db = nullptr;
    Database *internalDB = nullptr;
    LightgunGames lightgunGames;

    void loadAssets(bool reloadMusic = true);

    void display(bool forceScan, const std::string &_pathToGamesDir, Database *db, bool resume);

    void hideMouseCursor();

    void finish();


    static void splash(const std::string & message);

    void menuSelection();

    void saveSelection();

    Uint8 getR(const std::string &val);

    Uint8 getG(const std::string &val);

    Uint8 getB(const std::string &val);

    void criticalException(const std::string &text);

    SDL_Shared<SDL_Texture>
    loadThemeTexture(const string& themePath, const string& defaultPath, const string& texname);

    void exportDBToRetroarch();

    void stopAudio();
    void playMusic(bool customMusic, string musicPath);
    void restartAudio(int freq);
    void freeMusic();
    bool customMusic=false;
    int freq = 44100;
    string musicPath;

    MenuOption menuOption = MENU_OPTION_SCAN;

    // these are saved in gui so the next time Start brings up the carousel it can restore to last state
    int lastSet = SET_PS1;          // one of these: all games, internal, usb game dir, favorites, RA playlist
    // SET_PS1_All_Games, SET_PS1_Internal_Only, SET_PS1_Favorites, SET_PS1_History, SET_PS1_Games_Subdir
    int lastPS1_SelectState = SET_PS1_All_Games;
    int lastSelIndex = 0;           // index into carouselGames
    int lastUSBGameDirIndex = 0;    // top row in menu = /Games
    int lastRAPlaylistIndex = 0;    // top row in menu = first playlist name
    string lastRAPlaylistName = "";

    SDL_Rect backgroundRect;
    SDL_Rect logoRect;

    SDL_Shared<SDL_Texture> backgroundImg;
    SDL_Shared<SDL_Texture> logo;
    SDL_Shared<SDL_Texture> cdJewel;
    std::map<std::string, SDL_Shared<SDL_Texture>> buttonTextureMap;

    std::string pathToGamesDir; // path to /Games.  "/media/Games" or "/debugSystemPath/Games".

    Mix_Music *music = nullptr;
    FC_Font_Shared themeFont;
    bool forceScan = false;

    Mix_Chunk *cancel = nullptr;
    Mix_Chunk *cursor = nullptr;
    Mix_Chunk *home_down = nullptr;
    Mix_Chunk *home_up = nullptr;
    Mix_Chunk *resume = nullptr;

    bool startingGame = false;
    bool resumingGui = false;
    PsGamePtr runningGame;
    int emuMode = EMU_PCSX;
    int resumepoint = -1;
    string padMapping;

    Gui(Gui const &) = delete;

    Gui &operator=(Gui const &) = delete;

    static std::shared_ptr<Gui> getInstance() {
        static std::shared_ptr<Gui> s{new Gui};
        return s;
    }

    static bool sortByTitle(const PsGamePtr &i, const PsGamePtr &j) { return SortByCaseInsensitive(i->title, j->title); }

    //*******************************
    // Rect and Size routines
    //*******************************

    // return FC_Size w of font text and h of font
    FC_Size FC_getFontTextSize(FC_Font_Shared font, const char *text=nullptr);
    FC_Size FC_getFontTextSize(FC_Font_Shared font, const string& text="") {
        return FC_getFontTextSize(font, text.c_str());
    }
    // return FC_Rect w of font text and h of font
    FC_Rect FC_getFontTextRect(FC_Font_Shared font, const char *text=nullptr, int x=0, int y=0);
    FC_Rect FC_getFontTextRect(FC_Font_Shared font, const string& text="", int x=0, int y=0) {
        return FC_getFontTextRect(font, text.c_str(), x, y);
    }

    FC_Rect getOpscreenRectOfTheme();
    FC_Rect getTextRectOfTheme();

    int getCheckIconWidth();    // returns the width of the check icon texture.  used to compute the x position.
    static int align_xPosition(XAlignment xAlign, int x, int width);

    //*******************************
    // Text tokenizing structure routines
    //*******************************

    struct TextOrEmojiTokenInfo {
        std::string tokenString;
        SDL_Shared<SDL_Texture> emoji;  // not null only if tokenString is an emoji marker such as "|@X|"
        FC_Rect rect;                   // position, width, and height of rendered text or emoji texture
                                        // the x, y position is relative to the upper left corner of the string
    };

    // break up the text into tokens of text or the token of an emoji icon
    // build a vector of the text, emoji texture pointers, position, width and height of each token and the
    // total width and height of the entire line.
    struct AllTextOrEmojiTokenInfo {
        std::vector<TextOrEmojiTokenInfo> tokenInfos;

        FC_Font_Shared font;
        int x=0,y=0;            // upper left corner of the string on the display
        FC_Size totalSize;      // the total width and height of all the tokens
        bool useTextColor = false;
        SDL_Color textColor;
        bool drawBackgroundRect = false;

        AllTextOrEmojiTokenInfo() { }
        AllTextOrEmojiTokenInfo(FC_Font_Shared _font, const std::string & _text) { getTokenInfo(_font, _text); }
        void getTokenInfo(FC_Font_Shared _font, const std::string & _text);

        void compute_xy_relativeOffsets(); // compute x offset, center the y offset of each token to the total height
        void setTextColor(SDL_Color color) { textColor = color; textColor.a = SDL_ALPHA_OPAQUE; useTextColor = true; }

        // renders/draws the text and emoji icons at the chosen position on the screen
        void render(int x, int y, XAlignment xAlign = XALIGN_LEFT);
    };

    //*******************************
    // Text Line Rendering routines
    //*******************************

    // renders/draws the line of text and emoji icons at the chosen position on the screen.  returns the height.
    int renderText(FC_Font_Shared font, const std::string & text, int x, int y, XAlignment xAlign = XALIGN_LEFT);

    // if background == true it draws a solid grey box around/behind the text
    // this routine does not support emoji icons.  text only.
    int renderText_WithColor(FC_Font_Shared font, const std::string & text, int x, int y, SDL_Color textColor,
                             XAlignment xAlign = XALIGN_LEFT, bool background = false);

    // returns rectangle height
    int renderTextLine(const std::string & text, int line, int yoffset = 0,
                       XAlignment xAlign = XALIGN_LEFT, int xoffset = 0,
                       FC_Font_Shared font = FC_Font_Shared());   // font will default to themeFont in the cpp

    int renderTextLineToColumns(const string &textLeft, const string &textRight, int xLeft, int xRight, int line,
                                int yoffset = 0, FC_Font_Shared font = FC_Font_Shared());

    int renderTextLineOptions(const std::string & text, int line, int yoffset = 0,  XAlignment xAlign = XALIGN_LEFT, int xoffset = 0);

    void renderSelectionBox(int line, int yoffset, int xoffset = 0, FC_Font_Shared font = FC_Font_Shared());

    void renderLabelBox(int line, int yoffset);

    void renderTextChar(const std::string & text, int line, int yoffset, int posx);

    void renderFreeSpace();

    void renderBackground();

    int renderLogo(bool small);

    void renderStatus(const std::string & text, int pos=-1);

    void renderTextBar();

    void drawText(const std::string &text, const string &topLine="");
};
