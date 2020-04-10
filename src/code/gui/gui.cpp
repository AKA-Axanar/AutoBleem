//
// Created by screemer on 2018-12-19.
//

#include "gui.h"
#include "gui_about.h"
#include "gui_splash.h"
#include "menus/gui_optionsMenu.h"
#include "menus/gui_memCardsMenu.h"
#include "menus/gui_gameManagerMenu.h"
#include "gui_confirm.h"
#include <SDL2/SDL_image.h>
#include "../ver_migration.h"
#include "../lang.h"
#include "../launcher/gui_launcher.h"
#include "gui_padconfig.h"
#include "gui_padTest.h"
#include <fstream>
#include <unistd.h>
#include "../util.h"
#include <iostream>
#include <iomanip>
#include "../engine/scanner.h"
#include <json.h>
#include "../nlohmann/fifo_map.h"
#include "../environment.h"

using namespace std;
using namespace nlohmann;

// A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
template<class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using ordered_json = basic_json<my_workaround_fifo_map>;

#define RA_PLAYLIST "AutoBleem.lpl"

//********************
// Gui
//********************

//*******************************
// Gui::Gui ctor
//*******************************
Gui::Gui() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

#if defined(__x86_64__) || defined(_M_X64)

#else
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif

    TTF_Init();
    sonyFonts.openAllBasicSonyFonts(this, Env::getSonyPath());
    themeFonts.openAllBasicThemeFonts(this, getCurrentThemePath());

    mapper.init();
}

//*******************************
// Gui::~Gui dtor
//*******************************
Gui::~Gui() {
    SDL_Quit();
}

//
// Themes
//
//*******************************
// Gui::setThemePath
//*******************************
void Gui::setThemePath(const std::string& path) {
    cout << "set currentThemePath = '" << path << "'" << endl;
    currentThemePath = path;
}

//*******************************
// Gui::loadThemeIni
//*******************************
void Gui::loadThemeIni() {
    themeIni.reload(Env::getPathToSharedThemeFiles() + sep + "theme.ini");
    themeIni.OverwriteAndAppend(currentThemePath + sep + "theme.ini");    // adds to default/theme.ini values
}

//*******************************
// Gui::saveCurrentThemePathAndFillThemeIni
// find the UI or RA playlist theme path.  If valid, save the themePath and fill themeIni with the theme settings.
//*******************************
void Gui::saveCurrentThemePathAndFillThemeIni() {
    // check theme exists - otherwise back to default
    string newThemePath = getCurrentThemePath();

    if (currentSet != SET_RETROARCH) {
        // not in RA playlist. using regular UI theme. if theme.ini doesn't exist switch UI theme to default UI theme
        if (!DirEntry::exists(newThemePath + sep + "theme.ini")) {  // if no theme.ini in path
            cfg.inifile.values["theme"] = "default";            // change to default theme in the config.ini
            cfg.save();
            newThemePath = getCurrentThemePath();               // use the default UI theme path
        }
    }

    cout << "Saving theme path and filling themeIni for: " << currentThemePath << endl;
    setThemePath(newThemePath);
    loadThemeIni();
}

//*******************************
// Gui::getCurrentThemePath
// looks up the theme in config.ini, example return: /themes/aergb.  it does not return or set currentThemePath
//*******************************
string Gui::getCurrentThemePath() {
    string themePath = Env::getPathToThemesDir() + sep + cfg.inifile.values["theme"];
#if 0   // this is only for when we support RA themes
    if (currentSet == SET_RETROARCH) {
        string RAthemePath = Env::getPathToRetroarchThemesDir() + sep + cfg.inifile.values["ratheme"] + sep + currentRAPlaylistName;
        if (DirEntry::exists(RAthemePath + sep + "theme.ini"))
            themePath = RAthemePath;
    }
#endif
    if (!DirEntry::exists(themePath)) {
        themePath = Env::getSonyPath();
    }
    return themePath;
}

//*******************************
// Gui::getCurrentThemeFile
//*******************************
// give them the filename with an optional sub dir and theme path and it searches for the file in the theme paths.
// it will search 1) the current theme path, 2) Autobleem/bin/autobleem/sharedThemeFiles, 3) /usr/sony/share/data
// example file name or path to search for: ("cross.png"), ("error.wav",SOUNDS), ("SST-Medium.ttf",FONT)
string Gui::getCurrentThemeFile(const std::string& filename, const std::string& subdirToFile, const std::string& _themePath, bool reportError) {
    string relativePathToFile = _themePath;

    if (subdirToFile != "")
        relativePathToFile = subdirToFile + sep + filename;
    else
        relativePathToFile = filename;

    string testPath = getCurrentThemePath() + sep + relativePathToFile;
    if (DirEntry::exists(testPath)) {
        //cout << "found theme file " << testPath << endl;
        return testPath;
    }

    testPath = Env::getPathToSharedThemeFiles() + sep + relativePathToFile;
    if (DirEntry::exists(testPath)) {
        //cout << "found theme file " << testPath << endl;
        return testPath;
    }

    testPath = Env::getSonyPath() + sep + relativePathToFile;
    if (DirEntry::exists(testPath)) {
        //cout << "found theme file " << testPath << endl;
        return testPath;
    }

    if (reportError) {
        string errorMsg = "theme file " + relativePathToFile + " not found for theme " + currentThemePath;
        cout << errorMsg << endl;
        splash(errorMsg);
        usleep(5 * 1000);
    }
    return "";
}

string Gui::getCurrentThemeFileFromIniValue(const std::string& iniKey, const std::string& subdirToFile, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFile(themeIni.values[iniKey], subdirToFile, _themePath, reportError);
}

string Gui::getCurrentThemeRootFile(const std::string& file, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFile(file, "", _themePath, reportError);
}
string Gui::getCurrentThemeRootFileFromIniValue(const std::string& iniKey, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFileFromIniValue(iniKey, "", _themePath, reportError);
}

string Gui::getCurrentThemeImageFile(const std::string& file, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFile(file, "images", _themePath, reportError);
}
string Gui::getCurrentThemeImageFileFromIniValue(const std::string& iniKey, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFileFromIniValue(iniKey, "images", _themePath, reportError);
}

string Gui::getCurrentThemeFontFile(const std::string& file, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFile(file, "font", _themePath, reportError);
}
string Gui::getCurrentThemeFontFileFromIniValue(const std::string& iniKey, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFileFromIniValue(iniKey, "font", _themePath, reportError);
}

string Gui::getCurrentThemeSoundFile(const std::string& file, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFile(file, "sounds", _themePath, reportError);
}
string Gui::getCurrentThemeSoundFileFromIniValue(const std::string& iniKey, const std::string& _themePath, bool reportError) {
    return getCurrentThemeFileFromIniValue(iniKey, "sounds", _themePath, reportError);
}

//*******************************
// Gui::splash
//*******************************
void Gui::splash(const string &message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}

extern "C"
{
//*******************************
// Gui::splash
//*******************************
void splash(char *message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}
}

//*******************************
// Gui::getR
//*******************************
Uint8 Gui::getR(const string &val) {
    return atoi(Util::commaSep(val, 0).c_str());
}

//*******************************
// Gui::getG
//*******************************
Uint8 Gui::getG(const string &val) {
    return atoi(Util::commaSep(val, 1).c_str());
}

//*******************************
// Gui::getB
//*******************************
Uint8 Gui::getB(const string &val) {
    return atoi(Util::commaSep(val, 2).c_str());
}

//*******************************
// Gui::getTextAndRect
//*******************************
void Gui::getTextAndRect(SDL_Shared<SDL_Renderer> renderer, int x, int y, const char *text, TTF_Font_Shared font,
                         SDL_Shared<SDL_Texture> *texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Shared<SDL_Surface> surface;
    string fg = themeIni.values["text_fg"];
    SDL_Color textColor = {getR(fg), getG(fg), getB(fg), 0};

    if (strlen(text) == 0) {
        *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);
        rect->x = 0;
        rect->y = 0;
        rect->h = 0;
        rect->w = 0;
        return;
    }

    surface = TTF_RenderUTF8_Blended(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

//*******************************
// Gui::renderBackground
//*******************************
void Gui::renderBackground() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundImg, nullptr, &backgroundRect);
}

//*******************************
// Gui::renderLogo
//*******************************
int Gui::renderLogo(bool small) {
    if (!small) {
        SDL_RenderCopy(renderer, logo, nullptr, &logoRect);
        return 0;
    } else {
        SDL_Rect rect;
        rect.x = atoi(themeIni.values["opscreenx"].c_str());
        rect.y = atoi(themeIni.values["opscreeny"].c_str());
        rect.w = logoRect.w / 3;
        rect.h = logoRect.h / 3;
        SDL_RenderCopy(renderer, logo, nullptr, &rect);
        return rect.y + rect.h;
    }
}

//*******************************
// Gui::loadThemeTexture
// texname is the theme.ini key string
//*******************************
SDL_Shared<SDL_Texture>
Gui::loadThemeTexture(SDL_Shared<SDL_Renderer> renderer, string texname) {
    SDL_Shared<SDL_Texture> tex = nullptr;
    tex = IMG_LoadTexture(renderer, getCurrentThemeRootFileFromIniValue(texname).c_str());

    return tex;
}

//*******************************
// Gui::loadAssets
//*******************************
void Gui::loadAssets(bool reloadMusic) {

    saveCurrentThemePathAndFillThemeIni();

    bool reloading = false;

    if (backgroundImg != nullptr) {
        Mix_FreeChunk(cursor);
        Mix_FreeChunk(cancel);
        Mix_FreeChunk(home_down);
        Mix_FreeChunk(home_up);
        reloading = true;
        backgroundImg = nullptr;
    }

    logoRect.x = atoi(themeIni.values["lpositionx"].c_str());
    logoRect.y = atoi(themeIni.values["lpositiony"].c_str());
    logoRect.w = atoi(themeIni.values["lw"].c_str());
    logoRect.h = atoi(themeIni.values["lh"].c_str());

    backgroundImg = loadThemeTexture(renderer, "background");
    logo = loadThemeTexture(renderer, "logo");

    buttonO = loadThemeTexture(renderer, "circle");
    buttonX = loadThemeTexture(renderer, "cross");
    buttonT = loadThemeTexture(renderer, "triangle");
    buttonS = loadThemeTexture(renderer, "square");
    buttonSelect = loadThemeTexture(renderer, "select");
    buttonStart = loadThemeTexture(renderer, "start");
    buttonL1 = loadThemeTexture(renderer, "l1");
    buttonR1 = loadThemeTexture(renderer, "r1");
    buttonL2 = loadThemeTexture(renderer, "l2");
    buttonR2 = loadThemeTexture(renderer, "r2");
    buttonCheck = loadThemeTexture(renderer, "check");
    buttonUncheck = loadThemeTexture(renderer, "uncheck");
    buttonEsc = loadThemeTexture(renderer, "esc");
    buttonEnter = loadThemeTexture(renderer, "enter");
    buttonTab = loadThemeTexture(renderer, "tab");
    if (cfg.inifile.values["jewel"] != "none") {
        if (cfg.inifile.values["jewel"] == "default") {
            cdJewel = IMG_LoadTexture(renderer, (Env::getWorkingPath() + sep + "evoimg/nofilter.png").c_str());
        } else {
            cdJewel = IMG_LoadTexture(renderer,
                                      (Env::getWorkingPath() + sep + "evoimg/frames/" +
                                       cfg.inifile.values["jewel"]).c_str());
        }
    } else {
        cdJewel = nullptr;
    }
    int fontSize = 0;
    string fontSizeString = themeIni.values["fsize"];
    if (fontSizeString != "")
        fontSize = atoi(fontSizeString.c_str());
    themeFont = Fonts::openNewSharedFont(getCurrentThemeFileFromIniValue("font"), fontSize);

    if (reloadMusic) {
        if (music != nullptr) {

            Mix_FreeMusic(music);
            music = nullptr;
        }
    }
    bool customMusic = false;
    int freq = 32000;
    string musicPath = themeIni.values["music"];
    if (cfg.inifile.values["music"] != "--") {
        customMusic = true;
        musicPath = cfg.inifile.values["music"];
    }

    if (DirEntry::getFileExtension(musicPath) == "ogg") {
        freq = 44100;
    }

    if (reloadMusic) {
        int numtimesopened, frequency, channels;
        Uint16 format;
        numtimesopened = Mix_QuerySpec(&frequency, &format, &channels);
        for (int i = 0; i < numtimesopened; i++) {
            Mix_CloseAudio();
        }
        numtimesopened = Mix_QuerySpec(&frequency, &format, &channels);

        if (Mix_OpenAudio(freq, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
            printf("Unable to open audio: %s\n", Mix_GetError());
        }
    }
    cursor = Mix_LoadWAV(getCurrentThemeSoundFile("cursor.wav").c_str());
    cancel = Mix_LoadWAV(getCurrentThemeSoundFile("cancel.wav").c_str());
    home_up = Mix_LoadWAV(getCurrentThemeSoundFile("home_up.wav").c_str());
    home_down = Mix_LoadWAV(getCurrentThemeSoundFile("home_down.wav").c_str());
    resume = Mix_LoadWAV(getCurrentThemeSoundFile("resume_new.wav").c_str());

    if (reloadMusic) {
        if (cfg.inifile.values["nomusic"] != "true") {
            if (themeIni.values["loop"] != "-1") {
                if (!customMusic) {
                    music = Mix_LoadMUS(getCurrentThemeRootFileFromIniValue("music").c_str());
                    if (music == nullptr) { printf("Unable to load Music file: %s\n", Mix_GetError()); }
                    if (Mix_PlayMusic(music, themeIni.values["loop"] == "1" ? -1 : 0) == -1) {
                        printf("Unable to play music file: %s\n", Mix_GetError());
                    }
                } else {
                    music = Mix_LoadMUS((Env::getWorkingPath() + sep + "music/" + musicPath).c_str());
                    if (music == nullptr) { printf("Unable to load Music file: %s\n", Mix_GetError()); }
                    if (Mix_PlayMusic(music, -1) == -1) {
                        printf("Unable to play music file: %s\n", Mix_GetError());
                    }
                }
            }
        }
    }
}

//*******************************
// Gui::waitForGamepad
//*******************************
void Gui::waitForGamepad() {
    int joysticksFound = SDL_NumJoysticks();
    while (joysticksFound == 0) {
        drawText(_("PLEASE CONNECT GAMEPAD TO PLAYSTATION CLASSIC"));
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
        joysticksFound = SDL_NumJoysticks();
#if defined(__x86_64__) || defined(_M_X64)

#else
        SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    }
}

//*******************************
// Gui::criticalException
//*******************************
void Gui::criticalException(const string &text) {
    drawText(text);
    while (true) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();
                }
            }
            if (e.type == SDL_QUIT)
                return;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                return;

            if (e.type == SDL_JOYBUTTONDOWN) {
                return;
            }
        }
    }
}

//*******************************
// Gui::display
//*******************************
void Gui::display(bool forceScan, const string &_pathToGamesDir, Database *db, bool resume) {
    joysticks.clear();
    joynames.clear();
    this->db = db;
    this->pathToGamesDir = _pathToGamesDir;
    this->forceScan = forceScan;

    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    printf("We compiled against SDL version %d.%d.%d ...\n",
           compiled.major, compiled.minor, compiled.patch);
    printf("But we are linking against SDL version %d.%d.%d.\n",
           linked.major, linked.minor, linked.patch);

    Mix_Init(0);
    TTF_Init();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    loadAssets();

    if (!resume) {
        auto *splashScreen = new GuiSplash(renderer);
        splashScreen->show();
        delete splashScreen;

#if 1
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            SDL_Joystick *joystick = SDL_JoystickOpen(i);
            if (!mapper.isKnownPad(SDL_JoystickInstanceID(joystick))) {
                cout << "New pad type" << endl;
                // new controller configuration
                auto cfgPad = new GuiPadConfig(renderer);
                cfgPad->joyid = SDL_JoystickInstanceID(joystick);
                cfgPad->show();
                delete cfgPad;
            }
        }
#else   // build verion to test controllers.  GuiPadTest displays the SDL event coming from the controller.
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            SDL_Joystick *joystick = SDL_JoystickOpen(i);
            // for debugging new controllers
            // this will display a scrollable window displaying all the events coming from the new controller
            // for testing.  it also writes the output to ab_out.txt
            // hold down three buttons to exit and do a safe shutdown!
            auto cfgTest = new GuiPadTest(renderer);
            cfgTest->joyid = SDL_JoystickInstanceID(joystick);
            cfgTest->alsoWriteToCout = true;
                cfgTest->show();
            delete cfgTest;
            Util::powerOff();
        }
#endif

        waitForGamepad();
    } else {
        resumingGui = true;
    }
}

//*******************************
// Gui::saveSelection
//*******************************
void Gui::saveSelection() {
    ofstream os;
    string path = cfg.inifile.values["cfg"];
    os.open(path);
    os << "#!/bin/sh" << endl << endl;
    os << "AB_SELECTION=" << menuOption << endl;
    os << "AB_THEME=" << cfg.inifile.values["theme"] << endl;
    os << "AB_PCSX=" << cfg.inifile.values["pcsx"] << endl;
    os << "AB_MIP=" << cfg.inifile.values["mip"] << endl;

    os.flush();
    os.close();
}

bool otherMenuShift = false;
bool powerOffShift = false;

int Gui::_cb(int button, SDL_Event *e) {
    return mapper.translateButton(button, e);
}

//*******************************
// Gui::menuSelection
//*******************************
void Gui::menuSelection() {
    shared_ptr<Scanner> scanner(Scanner::getInstance());

    SDL_Joystick *joystick;
    if (joysticks.empty())
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            joystick = SDL_JoystickOpen(i);
            joysticks.push_back(joystick);
            cout << "--" << SDL_JoystickName(joystick) << endl;
        }
    // Check if all OK
// commented this part out.  it was just annoying people who only played RA
//    if (scanner->noGamesFoundDuringScan) {
//        criticalException(_("WARNING: NO GAMES FOUND. PRESS ANY BUTTON."));
//    }
    //
    if (!coverdb->isValid()) {
        criticalException(_("WARNING: NO COVER DB FOUND. PRESS ANY BUTTON."));
    }
    otherMenuShift = false;
    powerOffShift = false;
    string mainMenu = "|@Start| " + _("AutoBleem") + "    |@X|  " + _("Re/Scan") + " ";
    if (cfg.inifile.values["ui"] == "classic") {
        mainMenu += "  |@O|  " + _("Original") + "  ";
    }
    mainMenu += "|@S|  " + _("RetroArch") + "   ";
    mainMenu += "|@T|  " + _("About") + "  |@Select|  " + _("Options") + " ";
    mainMenu += "|@L1| " + _("Advanced");
    mainMenu += " |@L2|+|@R2|" + _("Power Off");

    string forceScanMenu = _("Games changed. Press") + "  |@X|  " + _("to scan") + "|";
    string otherMenu = "|@S|  " + _("Hardware Information") + "  |@X|  " + _("Memory Cards") + "   |@O|  " + _("Game Manager");
    cout << SDL_NumJoysticks() << "joysticks were found." << endl;

    if (!forceScan) {
        drawText(mainMenu);

    } else {
        drawText(forceScanMenu);
    }
    bool menuVisible = true;
    while (menuVisible) {
        watchJoystickPort();
        if (startingGame) {
            drawText(runningGame->title);
            this->menuOption = MENU_OPTION_START;
            menuVisible = false;
            startingGame = false;
            return;
        }

        if (resumingGui) {
            auto launcherScreen = new GuiLauncher(renderer);
            launcherScreen->show();
            delete launcherScreen;
            drawText("");
            resumingGui = false;
            menuSelection();
            menuVisible = false;
        }
        SDL_Event e;
        if (SDL_PollEvent(&e)) {

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP || e.key.keysym.sym == SDLK_ESCAPE) {
                    drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();
                }
            }

            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYBUTTONUP:
                    if (!forceScan) {
                        if (e.jbutton.button == _cb(PCS_BTN_L1, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            drawText(mainMenu);
                            otherMenuShift = false;
                        }
                        if (e.jbutton.button == _cb(PCS_BTN_L2, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            powerOffShift = false;
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    if (!forceScan) {
                        if (e.jbutton.button == _cb(PCS_BTN_L1, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            drawText(otherMenu);
                            otherMenuShift = true;
                        }
                        if (e.jbutton.button == _cb(PCS_BTN_L2, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            powerOffShift = true;
                        }
                    }

                    if (powerOffShift) {
                        if (e.jbutton.button == _cb(PCS_BTN_R2, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            drawText(_("POWERING OFF... PLEASE WAIT"));
#if defined(__x86_64__) || defined(_M_X64)
                            exit(0);
#else
                            Util::execUnixCommand("shutdown -h now");
                                    sync();
                                    exit(1);
#endif
                        };
                    }

                    if (!otherMenuShift) {
                        if (!forceScan)
                            if (e.jbutton.button == _cb(PCS_BTN_START, &e)) {
                                if (cfg.inifile.values["ui"] == "classic") {
                                    Mix_PlayChannel(-1, cursor, 0);
                                    this->menuOption = MENU_OPTION_RUN;
                                    menuVisible = false;
                                } else {
                                    if (lastSet < 0) {
                                        lastSet = SET_PS1;
                                        lastSelIndex=0;
                                        resumingGui = false;
                                    }
                                    Mix_PlayChannel(-1, cursor, 0);
                                    drawText(_("Starting EvolutionUI"));
                                    loadAssets(false);
                                    auto launcherScreen = new GuiLauncher(renderer);
                                    launcherScreen->show();
                                    delete launcherScreen;

                                    menuSelection();
                                    menuVisible = false;
                                }
                            };

                        if (!forceScan)
                            if (e.jbutton.button == _cb(PCS_BTN_SQUARE, &e)) {
                                Mix_PlayChannel(-1, cursor, 0);
                                if (!DirEntry::exists(Env::getPathToRetroarchDir() + sep + "retroarch")) {

                                    auto confirm = new GuiConfirm(renderer);
                                    confirm->label = _("RetroArch is not installed");
                                    confirm->show();
                                    bool result = confirm->result;
                                    delete confirm;
                                    if (result) {
                                        this->menuOption = MENU_OPTION_RETRO;
                                        menuVisible = false;
                                    } else {
                                        menuSelection();
                                        menuVisible = false;
                                    }
                                } else {
                                    exportDBToRetroarch();
                                    this->menuOption = MENU_OPTION_RETRO;
                                    menuVisible = false;
                                }
                            };

                        if (e.jbutton.button == _cb(PCS_BTN_CROSS, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            this->menuOption = MENU_OPTION_SCAN;

                            menuVisible = false;
                        };
                        if (e.jbutton.button == _cb(PCS_BTN_TRIANGLE, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto *aboutScreen = new GuiAbout(renderer);
                            aboutScreen->show();
                            delete aboutScreen;

                            menuSelection();
                            menuVisible = false;
                        };
                        if (e.jbutton.button == _cb(PCS_BTN_SELECT, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto options = new GuiOptions(renderer);
                            options->show();
                            delete options;
                            menuSelection();
                            menuVisible = false;
                        };
                        if (!forceScan)
                            if (cfg.inifile.values["ui"] == "classic")
                                if (e.jbutton.button == _cb(PCS_BTN_CIRCLE, &e)) {
                                    Mix_PlayChannel(-1, cancel, 0);
                                    this->menuOption = MENU_OPTION_SONY;
                                    menuVisible = false;
                                };
                        break;
                    } else {
                        if (e.jbutton.button == _cb(PCS_BTN_SQUARE, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            string cmd = Env::getPathToAppsDir() + sep + "pscbios/run.sh";
                            vector<const char *> argvNew { cmd.c_str(), nullptr };
                            Util::execFork(cmd.c_str(), argvNew);
                        };

                        if (e.jbutton.button == _cb(PCS_BTN_CROSS, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto memcardsScreen = new GuiMemcards(renderer);
                            memcardsScreen->show();
                            delete memcardsScreen;

                            menuSelection();
                            menuVisible = false;
                        };

                        if (e.jbutton.button == _cb(PCS_BTN_CIRCLE, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto managerScreen = new GuiManager(renderer);
                            managerScreen->show();
                            delete managerScreen;

                            menuSelection();
                            menuVisible = false;
                        };
                    }
            }
        }
    }
}

//*******************************
// Gui::finish
//*******************************
void Gui::finish() {

    if (Mix_PlayingMusic()) {
        Mix_FadeOutMusic(300);
        while (Mix_PlayingMusic()) {
        }
    } else {
        usleep(300 * TicksPerSecond);
    }

    Mix_HaltMusic();
    Mix_FreeMusic(music);
    Mix_FreeChunk(cursor);
    Mix_FreeChunk(cancel);
    Mix_FreeChunk(home_down);
    Mix_FreeChunk(home_up);
    Mix_CloseAudio();
    music = nullptr;
    backgroundImg = nullptr;
}

//*******************************
// Gui::getEmojiTextTexture
//*******************************
void Gui::getEmojiTextTexture(SDL_Shared<SDL_Renderer> renderer, string text, TTF_Font_Shared font,
                              SDL_Shared<SDL_Texture> *texture,
                              SDL_Rect *rect) {
    if (text.empty()) text = " ";
    if (text.back() != '|') {
        text = text + "|";
    }

    vector<SDL_Shared<SDL_Texture>> textTexures;
    vector<string> textParts;
    std::string delimiter = "|";

    size_t pos = 0;
    std::string token;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        token = text.substr(0, pos);
        if (!token.empty())
            textParts.push_back(token);
        text.erase(0, pos + delimiter.length());
    }

    for (const string &str:textParts) {
        if (str.empty()) continue;
        if (str[0] == '@') {
            string icon = str.substr(1);
            if (icon == "Start") {
                textTexures.push_back(buttonStart);
            }
            if (icon == "S") {
                textTexures.push_back(buttonS);
            }
            if (icon == "O") {
                textTexures.push_back(buttonO);
            }
            if (icon == "Select") {
                textTexures.push_back(buttonSelect);
            }
            if (icon == "L1") {
                textTexures.push_back(buttonL1);
            }
            if (icon == "R1") {
                textTexures.push_back(buttonR1);
            }
            if (icon == "L2") {
                textTexures.push_back(buttonL2);
            }
            if (icon == "R2") {
                textTexures.push_back(buttonR2);
            }
            if (icon == "T") {
                textTexures.push_back(buttonT);
            }
            if (icon == "X") {
                textTexures.push_back(buttonX);
            }
            if (icon == "Check") {
                textTexures.push_back(buttonCheck);
            }
            if (icon == "Uncheck") {
                textTexures.push_back(buttonUncheck);
            }
            if (icon == "Esc") {
                textTexures.push_back(buttonEsc);
            }
            if (icon == "Enter") {
                textTexures.push_back(buttonEnter);
            }
            if (icon == "Tab") {
                textTexures.push_back(buttonTab);
            }
        } else {
            SDL_Shared<SDL_Texture> textTex = nullptr;
            SDL_Rect textRec;
            getTextAndRect(renderer, 0, atoi(themeIni.values["ttop"].c_str()), str.c_str(), font, &textTex,
                           &textRec);
            textTexures.push_back(textTex);
        }
    }

    int w = 0;
    int h = 0;

    for (SDL_Shared<SDL_Texture> tex:textTexures) {
        Uint32 format;
        int access;
        int tw, th;
        SDL_QueryTexture(tex, &format, &access, &tw, &th);

        w += tw;
        if (th > h) h = th;
    }

    *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, *texture);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int xpos = 0;
    for (SDL_Shared<SDL_Texture> tex:textTexures) {
        Uint32 format;
        int access;
        int tw, th;
        SDL_QueryTexture(tex, &format, &access, &tw, &th);

        SDL_Rect posRect;
        posRect.x = xpos;

        posRect.y = 0;

        if (th != h) {
            posRect.y = (h - th) / 2;
        }
        posRect.w = tw;
        posRect.h = th;
        xpos += tw;
        SDL_RenderCopy(renderer, tex, nullptr, &posRect);
    }
    rect->w = w;
    rect->h = h;
    rect->x = 0;
    rect->y = 0;
    SDL_SetRenderTarget(renderer, nullptr);

    textTexures.clear();
}

//*******************************
// Gui::renderStatus
//*******************************
void Gui::renderStatus(const string &text, int posy) {
    string bg = themeIni.values["text_bg"];

    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;
    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeIni.values["textalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(themeIni.values["textx"].c_str());
    rect.y = atoi(themeIni.values["texty"].c_str());
    rect.w = atoi(themeIni.values["textw"].c_str());
    rect.h = atoi(themeIni.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    getEmojiTextTexture(renderer, text, themeFont, &textTex, &textRec);
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    textRec.y = atoi(themeIni.values["ttop"].c_str());
    if (posy!=-1)
    {
        textRec.y=posy;
    }
    if (textRec.w > atoi(themeIni.values["textw"].c_str()))
        textRec.w = atoi(themeIni.values["textw"].c_str());
    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
}

//*******************************
// Gui::drawText
//*******************************
void Gui::drawText(const string &text) {
    renderBackground();
    renderLogo(false);
    renderStatus(text);
    SDL_RenderPresent(renderer);
}

//*******************************
// Gui::renderLabelBox
//*******************************
void Gui::renderLabelBox(int line, int offset) {
    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    string bg = themeIni.values["label_bg"];

    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);

    SDL_Rect rect2;
    rect2.x = atoi(themeIni.values["opscreenx"].c_str());
    rect2.y = atoi(themeIni.values["opscreeny"].c_str());
    rect2.w = atoi(themeIni.values["opscreenw"].c_str());
    rect2.h = atoi(themeIni.values["opscreenh"].c_str());

    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5;
    rectSelection.y = offset + textRec.h * (line);
    rectSelection.w = rect2.w - 10;
    rectSelection.h = textRec.h;


    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeIni.values["keyalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &rectSelection);
}

//*******************************
// Gui::renderSelectionBox
//*******************************
void Gui::renderSelectionBox(int line, int offset, int xoffset, TTF_Font_Shared font) {
    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    if (!font)
        font = themeFont;

    string fg = themeIni.values["text_fg"];

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);

    SDL_Rect rect2;
    rect2.x = atoi(themeIni.values["opscreenx"].c_str());
    rect2.y = atoi(themeIni.values["opscreeny"].c_str());
    rect2.w = atoi(themeIni.values["opscreenw"].c_str());
    rect2.h = atoi(themeIni.values["opscreenh"].c_str());

    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5 + xoffset;
    rectSelection.y = offset + textRec.h * (line);
    rectSelection.w = rect2.w - 10 - xoffset;
    rectSelection.h = textRec.h;

    SDL_SetRenderDrawColor(renderer, getR(fg), getG(fg), getB(fg), 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawRect(renderer, &rectSelection);
}

//*******************************
// Gui::renderTextLineOptions
//*******************************
int Gui::renderTextLineOptions(const string &_text, int line, int offset, int position, int xoffset) {
    string text = _text;
    int button = -1;
    if (text.find("|@Check|") != std::string::npos) {
        button = 1;
    }
    if (text.find("|@Uncheck|") != std::string::npos) {
        button = 0;
    }
    if (button != -1) {
        text = text.substr(0, text.find("|"));
    }

    int h = renderTextLine(text, line, offset, position, xoffset);

    SDL_Shared<SDL_Texture> buttonTex;
//    SDL_Rect rect;

    if (button == -1) {
        return h;
    }

    SDL_Rect textRec;
    SDL_Rect rect2;

    rect2.x = atoi(themeIni.values["opscreenx"].c_str());
    rect2.y = atoi(themeIni.values["opscreeny"].c_str());
    rect2.w = atoi(themeIni.values["opscreenw"].c_str());
    rect2.h = atoi(themeIni.values["opscreenh"].c_str());
    getTextAndRect(renderer, 0, 0, "*", themeFont, &buttonTex, &textRec);
    int lineh = textRec.h;
    if (button == 1) {
        getEmojiTextTexture(renderer, "|@Check|", themeFont, &buttonTex, &textRec);
    } else if (button == 0) {
        getEmojiTextTexture(renderer, "|@Uncheck|", themeFont, &buttonTex, &textRec);
    }

    textRec.x = rect2.x + rect2.w - 10 - textRec.w;
    textRec.y = (lineh * line) + offset;

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (position==POS_CENTER) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }
    if (position==POS_RIGHT) {
        textRec.x = 1280 - textRec.x - textRec.w;
    }

    SDL_RenderCopy(renderer, buttonTex, nullptr, &textRec);
    return h;
}

//*******************************
// Gui::renderTextLine
//*******************************
int Gui::renderTextLine(const string &text, int line, int offset,  int position, int xoffset, TTF_Font_Shared font) {
    if (!font)
        font = themeFont;   // default to themeFont

    SDL_Rect rect2;
    rect2.x = atoi(themeIni.values["opscreenx"].c_str());
    rect2.y = atoi(themeIni.values["opscreeny"].c_str());
    rect2.w = atoi(themeIni.values["opscreenw"].c_str());
    rect2.h = atoi(themeIni.values["opscreenh"].c_str());

    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);
    int lineh = textRec.h;
    getEmojiTextTexture(renderer, text, font, &textTex, &textRec);
    textRec.x = rect2.x + 10 + xoffset;
    textRec.y = (lineh * line) + offset;

    if (line<0)
    {
        line=-line;
        textRec.y=line;
    }

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (position==POS_CENTER) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }
    if (position==POS_RIGHT) {
        textRec.x = 1280 - textRec.x - textRec.w;
    }

    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);

    return textRec.h;
}

//*******************************
// Gui::getTextRectangleOnScreen
//*******************************
// returns the SDL_Rect of the screen positions if your rendered this text with these args
SDL_Rect Gui::getTextRectangleOnScreen(const string &text, int line, int offset,  int position, int xoffset, TTF_Font_Shared font) {
    if (!font)
        font = themeFont;   // default to themeFont

    SDL_Rect rect2;
    rect2.x = atoi(themeIni.values["opscreenx"].c_str());
    rect2.y = atoi(themeIni.values["opscreeny"].c_str());
    rect2.w = atoi(themeIni.values["opscreenw"].c_str());
    rect2.h = atoi(themeIni.values["opscreenh"].c_str());

    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);
    int lineh = textRec.h;
    getEmojiTextTexture(renderer, text, font, &textTex, &textRec);
    textRec.x = rect2.x + 10 + xoffset;
    textRec.y = (lineh * line) + offset;

    if (line<0)
    {
        line=-line;
        textRec.y=line;
    }

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (position==POS_CENTER) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }
    if (position==POS_RIGHT) {
        textRec.x = 1280 - textRec.x - textRec.w;
    }

    //SDL_RenderCopy(renderer, textTex, nullptr, &textRec);

    return textRec;
}

//*******************************
// Gui::renderTextLineToColumns
//*******************************
int Gui::renderTextLineToColumns(const string &textLeft, const string &textRight,
                                 int xLeft, int xRight,
                                 int line, int offset, TTF_Font_Shared font) {

            renderTextLine(textLeft,  line, offset, POS_LEFT, xLeft, font);
    int h = renderTextLine(textRight, line, offset, POS_LEFT, xRight, font);

    return h;   // rectangle height
}

//*******************************
// Gui::renderTextChar
//*******************************
void Gui::renderTextChar(const string &text, int line, int offset, int posx) {
#if 0
    SDL_Rect rect2;
    rect2.x = atoi(themeIni.values["opscreenx"].c_str());
    rect2.y = atoi(themeIni.values["opscreeny"].c_str());
    rect2.w = atoi(themeIni.values["opscreenw"].c_str());
    rect2.h = atoi(themeIni.values["opscreenh"].c_str());
#endif

    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);
    getTextAndRect(renderer, posx, (textRec.h * line) + offset,
                   text.c_str(), themeFont, &textTex, &textRec);

    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
}

//*******************************
// Gui::renderTextBar
//*******************************
void Gui::renderTextBar() {
    string bg = themeIni.values["main_bg"];
    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeIni.values["mainalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2;
    rect2.x = atoi(themeIni.values["opscreenx"].c_str());
    rect2.y = atoi(themeIni.values["opscreeny"].c_str());
    rect2.w = atoi(themeIni.values["opscreenw"].c_str());
    rect2.h = atoi(themeIni.values["opscreenh"].c_str());

    SDL_RenderFillRect(renderer, &rect2);
}

//*******************************
// Gui::renderFreeSpace
//*******************************
void Gui::renderFreeSpace() {
    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;
    SDL_Rect rect;

    rect.x = atoi(themeIni.values["fsposx"].c_str());
    rect.y = atoi(themeIni.values["fsposy"].c_str());
    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);
    getEmojiTextTexture(renderer, _("Free space") + " : " + Util::getAvailableSpace(), themeFont, &textTex, &textRec);
    rect.w = textRec.w;
    rect.h = textRec.h;
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, textTex, nullptr, &rect);
}

//*******************************
// Gui::watchJoystickPort
//*******************************
void Gui::watchJoystickPort() {

    int numJoysticks = SDL_NumJoysticks();
    if (numJoysticks != joysticks.size()) {
        cout << "Pad changed" << endl;
        for (SDL_Joystick *joy:joysticks) {
            if (SDL_JoystickGetAttached(joy)) {
                SDL_JoystickClose(joy);
            }
            joysticks.clear();
            joynames.clear();
        }
        SDL_Joystick *joystick;
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            joystick = SDL_JoystickOpen(i);
            joysticks.push_back(joystick);
            joynames.push_back(SDL_JoystickName(joystick));
            cout << "Pad connected" << endl;
            cout << "--" << SDL_JoystickName(joystick) << endl;
        }
    }
}

void Gui::exportDBToRetroarch() {
    ordered_json j;
    j["version"]="1.0";

    PsGames gamesList;
    db->getGames(&gamesList);
    sort(gamesList.begin(), gamesList.end(), sortByTitle);

    ordered_json items = ordered_json::array();
    // copy the gamesList into json object
    for_each(begin(gamesList), end(gamesList), [&](PsGamePtr &game)
    {
        ordered_json item = ordered_json::object();

        string gameFile = (game->folder + sep + game->base);
        if (!DirEntry::matchExtension(game->base, ".pbp")) {
            gameFile += ".cue";
        }
        gameFile += "";

        string base;
        if (DirEntry::isPBPFile(game->base)) {
            base = game->base.substr(0, game->base.length() - 4);
        } else {
            base = game->base;
        }
        if (DirEntry::exists(game->folder + sep + base + ".m3u")) {
            gameFile = game->folder + sep + base + ".m3u";
        }


        item["path"]=gameFile;
        item["label"]=game->title;
        item["core_path"]=Env::getPathToRetroarchCoreFile();
        item["core_name"]="DETECT";
        item["crc32"]="00000000|crc";
        item["db_name"]=RA_PLAYLIST;

        items.push_back(item);
    });

    j["items"] = items;

    cout << j.dump() << endl;
    std::ofstream o(Env::getPathToRetroarchDir() + sep + "playlists/" + RA_PLAYLIST);
    o << std::setw(2) << j << std::endl;
    o.flush();
    o.close();
}