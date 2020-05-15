//
// Created by screemer on 2018-12-19.
//
#pragma once

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
#include "gui_font.h"

#define PCS_DEADZONE     32000
#define PCS_BTN_L2       4
#define PCS_BTN_R2       5
#define PCS_BTN_L1       6
#define PCS_BTN_R1       7
#define PCS_BTN_START    9
#define PCS_BTN_SQUARE   3
#define PCS_BTN_TRIANGLE 0
#define PCS_BTN_CROSS    2
#define PCS_BTN_CIRCLE   1
#define PCS_BTN_SELECT   8

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
    Gui() { mapper.init(); }

    string themePath;

public:
    //*******************************
    // Member Variables
    //*******************************

    std::vector<SDL_Joystick *> joysticks;

    int _cb(int button, SDL_Event *e);

    vector<string> joynames;
    PadMapper mapper;
    Inifile themeData;
    Inifile defaultData;

    Coverdb *coverdb = nullptr;
    // db and internalDB are set in main.cpp and remain alive until exit
    Database *db = nullptr;
    Database *internalDB = nullptr;

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

                            //*******************************
                            // Functions
                            //*******************************

    //*******************************
    // Gui Singleton
    //*******************************

    Gui(Gui const &) = delete;

    Gui &operator=(Gui const &) = delete;

    static std::shared_ptr<Gui> getInstance() {
        static std::shared_ptr<Gui> s{new Gui};
        return s;
    }

    //*******************************
    // Misc Functions
    //*******************************
    static void splash(const std::string & message);

    Uint8 getR(const std::string & val);

    Uint8 getG(const std::string & val);

    Uint8 getB(const std::string & val);

    void watchJoystickPort();

    SDL_Shared<SDL_Texture>
    loadThemeTexture(const std::string& themePath, const std::string& defaultPath, const std::string& texname);

    void loadAssets(bool reloadMusic = true);

    void waitForGamepad();

    void criticalException(const std::string & text);

    void display(bool forceScan, const std::string &_pathToGamesDir, Database *db, bool resume);

    void saveSelection();

    void menuSelection();

    void finish();

    void exportDBToRetroarch();

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
    int align_xPosition(XAlignment xAlign, int x, int width);

    //*******************************
    // Text tokenizing structure routines
    //*******************************

    struct TextOrEmojiTokenInfo {
        std::string tokenString;
        SDL_Shared<SDL_Texture> emoji;  // not null only if tokenString is an emoji marker such as "|@X|"
        FC_Size size;                   // width and height of rendered text or emoji texture
    };
    struct AllTextOrEmojiTokenInfo {
        std::vector<TextOrEmojiTokenInfo> info;
        FC_Size totalSize;      // the total width and height of all the tokens
    };

    // break up the text into tokens of pure text or an emoji icon marker
    // return a vector of the text, emoji texture pointers, width and height of each token and the total width and height.
    AllTextOrEmojiTokenInfo getAllTokenInfoForLineOfTextAndEmojis(FC_Font_Shared font, const std::string & text);

    //*******************************
    // Rendering routines
    //*******************************

    // renders/draws the text and emoji icons at the chosen position on the screen
    void renderAllTokenInfo(FC_Font_Shared font,
                            AllTextOrEmojiTokenInfo& allTokenInfo, int x, int y, XAlignment xAlign = XALIGN_LEFT);

    // renders/draws the line of text and emoji icons at the chosen position on the screen.  returns the height.
    int renderText(FC_Font_Shared font, const std::string & text, int x, int y, XAlignment xAlign = XALIGN_LEFT);

    // if background == true it draws a solid grey box around/behind the text
    // this routine does not support emoji icons.  text only.
    void renderTextOnly_WithColor(int x, int y, const std::string & text, SDL_Color textColor,
                                         FC_Font_Shared font, XAlignment xAlign, bool background);

    // returns rectangle height
    int renderTextLine(const std::string & text, int line, int yoffset = 0,
                       XAlignment xAlign = XALIGN_LEFT, int xoffset = 0,
                       FC_Font_Shared font = FC_Font_Shared());   // font will default to themeFont in the cpp

    // returns the SDL_Rect of the screen positions if your rendered this text with these args
    // this is basically renderTextLine but doesn't render the texct and instead returns the bounding rectangle
    SDL_Rect getTextRectangleOnScreen(const std::string & text, int line,
                                      int yoffset = 0, XAlignment xAlign = XALIGN_LEFT, int xoffset = 0,
                                      FC_Font_Shared font = FC_Font_Shared());    // font will default to themeFont in the cpp

    int renderTextLineToColumns(const string &textLeft, const string &textRight, int xLeft, int xRight, int line,
                                int yoffset = 0, FC_Font_Shared font = FC_Font_Shared());

    int renderTextLineOptions(const std::string & text, int line, int yoffset = 0,  XAlignment xAlign = XALIGN_LEFT, int xoffset = 0);

    void renderSelectionBox(int line, int yoffset, int xoffset = 0, FC_Font_Shared font = FC_Font_Shared());

    void renderLabelBox(int line, int yoffset);

    void renderTextChar(const std::string & text, int line, int yoffset, int posx);

    void renderFreeSpace();

    void getTextureAndRect(int x, int y, const char *text,
                           FC_Font_Shared font, SDL_Shared<SDL_Texture> *texture, FC_Rect *rect);

    void renderBackground();

    int renderLogo(bool small);

    void renderStatus(const std::string & text, int pos=-1);

    void renderTextBar();

    void drawText(const std::string & text);
};
