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
//#include "gui_font_wrapper.h"
#include "gui_font.h"
#include "../environment.h"

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

#define POS_LEFT 0
#define POS_CENTER 1
#define POS_RIGHT 2

// if you add a new set also update setNames in gui_launcher.cpp
#define SET_PS1      0
#define SET_RETROARCH 1
#define SET_APPS 2
#define SET_LAST 2

// SET_PS1 select sub states. keep SET_PS1_Games_Subdir last as it's going to be left off the L2+Select menu
enum { SET_PS1_All_Games=0, SET_PS1_Internal_Only, SET_PS1_Favorites, SET_PS1_History, SET_PS1_Games_Subdir };

//********************
// Gui
//********************
class Gui {
private:

    Gui();

public:
    ~Gui();

    SDL_Shared<SDL_Window> window;
    SDL_Shared<SDL_Renderer> renderer;

    Fonts themeFonts;
    Fonts sonyFonts;
    Config cfg;
    bool inGuiLauncher = false;

    //
    // Themes
    //
    std::string currentThemePath;   // set by Gui::loadAssets()
    void setThemePath(const std::string& path);

    Inifile themeIni;
    void loadThemeIni();

    // find the UI or RA playlist theme path.  If valid, save the themePath and fill themeIni with the theme settings.
    void saveCurrentThemePathAndFillThemeIni();

    // looks up the theme in config.ini, example return: /themes/aergb.  it does not return or set currentThemePath
    std::string getCurrentThemePath();

    // give it the filename or path/filename and it searches for the file in the theme paths.
    // it will search 1) the current theme path, 2) Autobleem/bin/autobleem/sharedThemeFiles, 3) /usr/sony/share/data
    // example file name or path to search for: "cross.png", "sounds/error.wav", "font/SST-Medium.ttf"
    std::string getCurrentThemeFile(const std::string& filename, const std::string& subdirToFile="", const std::string& _themePath="", bool reportError=true);
    std::string getCurrentThemeFileFromIniValue(const std::string& iniKey, const std::string& subdirToFile="", const std::string& _themePath="", bool reportError=true);

    std::string getCurrentThemeRootFile(const std::string& file, const std::string& _themePath="", bool reportError=true);
    std::string getCurrentThemeRootFileFromIniValue(const std::string& iniKey, const std::string& _themePath="", bool reportError=true);

    std::string getCurrentThemeImageFile(const std::string& file, const std::string& _themePath="", bool reportError=true);
    std::string getCurrentThemeImageFileFromIniValue(const std::string& iniKey, const std::string& _themePath="", bool reportError=true);

    std::string getCurrentThemeFontFile(const std::string& file, const std::string& _themePath="", bool reportError=true);
    std::string getCurrentThemeFontFileFromIniValue(const std::string& iniKey, const std::string& _themePath="", bool reportError=true);

    std::string getCurrentThemeSoundFile(const std::string& file, const std::string& _themePath="", bool reportError=true);
    std::string getCurrentThemeSoundFileFromIniValue(const std::string& iniKey, const std::string& _themePath="", bool reportError=true);

    std::vector<SDL_Joystick *> joysticks;

    int _cb(int button, SDL_Event *e);

    vector<string> joynames;
    PadMapper mapper;

    Coverdb *coverdb = nullptr;
    // db and internalDB are set in main.cpp and remain alive until exit
    Database *db = nullptr;
    Database *internalDB = nullptr;

    void watchJoystickPort();

    void loadAssets(bool reloadMusic = true);

    void display(bool forceScan, const std::string &_pathToGamesDir, Database *db, bool resume);

    void waitForGamepad();

    void finish();

    void drawText(const std::string & text);

    void getEmojiTextTexture(SDL_Shared<SDL_Renderer> renderer, std::string text,
                             TTF_Font_Shared font, SDL_Shared<SDL_Texture> *texture, SDL_Rect *rect);

    static void splash(const std::string & message);

    void menuSelection();

    void saveSelection();

    void renderBackground();

    int renderLogo(bool small);

    void renderStatus(const std::string & text, int pos=-1);

    void renderTextBar();

    // returns rectangle height
    int renderTextLine(const std::string & text, int line,
                       int offset = 0, int position = POS_LEFT, int xoffset = 0,
                       TTF_Font_Shared font = TTF_Font_Shared());   // font will default to themeFont in the cpp

    // returns the SDL_Rect of the screen positions if your rendered this text with these args
    // this is basically renderTextLine but doesn't render the texct and instead returns the bounding rectangle
    SDL_Rect getTextRectangleOnScreen(const std::string & text, int line,
                       int offset = 0, int position = POS_LEFT, int xoffset = 0,
                       TTF_Font_Shared font = TTF_Font_Shared());    // font will default to themeFont in the cpp

    int renderTextLineToColumns(const string &textLeft, const string &textRight, int xLeft, int xRight, int line,
                                int offset = 0, TTF_Font_Shared font = TTF_Font_Shared());

    int renderTextLineOptions(const std::string & text, int line, int offset = 0,  int position = POS_LEFT, int xoffset = 0);

    void renderSelectionBox(int line, int offset, int xoffset = 0, TTF_Font_Shared font = TTF_Font_Shared());

    void renderLabelBox(int line, int offset);

    void renderTextChar(const std::string & text, int line, int offset, int posx);

    void renderFreeSpace();

    void getTextAndRect(SDL_Shared<SDL_Renderer> renderer, int x, int y, const char *text,
                        TTF_Font_Shared font, SDL_Shared<SDL_Texture> *texture, SDL_Rect *rect);

    Uint8 getR(const std::string & val);

    Uint8 getG(const std::string & val);

    Uint8 getB(const std::string & val);

    void criticalException(const std::string & text);

    SDL_Shared<SDL_Texture>
    loadThemeTexture(SDL_Shared<SDL_Renderer> renderer, std::string texname);   // texname is the theme.ini key string

    void exportDBToRetroarch();

    MenuOption menuOption = MENU_OPTION_SCAN;

    int currentSet = SET_PS1;
    int currentPS1_SelectState = SET_PS1_All_Games;    // SET_PS1_All_Games, SET_PS1_Internal_Only, SET_PS1_Favorites, SET_PS1_Games_Subdir

    // current USB Game Dir
    int currentUSBGameDirIndex = 0;
    std::string currentUSBGameDirName = "";

    // current RetroArch Playlist
    int currentRAPlaylistIndex = 0;
    std::string currentRAPlaylistName = "";

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
    SDL_Shared<SDL_Texture> buttonX;
    SDL_Shared<SDL_Texture> buttonO;
    SDL_Shared<SDL_Texture> buttonT;
    SDL_Shared<SDL_Texture> buttonS;
    SDL_Shared<SDL_Texture> buttonStart;
    SDL_Shared<SDL_Texture> buttonSelect;
    SDL_Shared<SDL_Texture> buttonL1;
    SDL_Shared<SDL_Texture> buttonR1;
    SDL_Shared<SDL_Texture> buttonL2;
    SDL_Shared<SDL_Texture> buttonR2;
    SDL_Shared<SDL_Texture> buttonCheck;
    SDL_Shared<SDL_Texture> buttonUncheck;
    SDL_Shared<SDL_Texture> buttonEsc;
    SDL_Shared<SDL_Texture> buttonEnter;
    SDL_Shared<SDL_Texture> buttonBackspace;
    SDL_Shared<SDL_Texture> buttonTab;
    SDL_Shared<SDL_Texture> cdJewel;

    std::string pathToGamesDir; // path to /Games.  "/media/Games" or "/debugSystemPath/Games".

    Mix_Music *music = nullptr;
    TTF_Font_Shared themeFont;
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
};
