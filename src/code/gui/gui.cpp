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
#include "../launcher/gui_launcher.h"
#include "gui_padconfig.h"
#include "gui_padTest.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <json.h>
#include "../nlohmann/fifo_map.h"

using namespace std;
using namespace nlohmann;

// A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
template<class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using ordered_json = basic_json<my_workaround_fifo_map>;

#define RA_PLAYLIST "AutoBleem.lpl"

                                    //*******************************
                                    // GuiBase
                                    //*******************************

//********************
// GuiBase::GuiBase
//********************
GuiBase::GuiBase() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

#if defined(__x86_64__) || defined(_M_X64)

#else
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif

    TTF_Init();
    sonyFonts.openAllFonts(Env::getSonyFontPath(), renderer);
    themeFonts.openAllFonts(getCurrentThemeFontPath(), renderer);
}

//********************
// GuiBase::~GuiBase
//********************
GuiBase::~GuiBase() {
    SDL_Quit();
}

//*******************************
// GuiBase::getCurrentThemePath
//*******************************
string GuiBase::getCurrentThemePath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = Env::getPathToThemesDir() + sep + cfg.inifile.values["theme"];
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath();
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data";
    }
    return path;
#endif
}

//*******************************
// GuiBase::getCurrentThemeImagePath
//*******************************
string GuiBase::getCurrentThemeImagePath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = getCurrentThemePath() + sep + "images";
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath() + sep + "images";
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "/images";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data/images";
    }
    return path;
#endif
}

//*******************************
// GuiBase::getCurrentThemeSoundPath
//*******************************
string GuiBase::getCurrentThemeSoundPath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = getCurrentThemePath() + sep + "sounds";
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath() + sep + "sounds";
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "/sounds";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data/sounds";
    }
    return path;
#endif
}

//*******************************
// GuiBase::getCurrentThemeFontPath
//*******************************
string GuiBase::getCurrentThemeFontPath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = getCurrentThemePath() + sep + "font";
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath() + sep + "font";
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "/font";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data/font";
    }
    return path;
#endif
}

                                    //*******************************
                                    // Gui
                                    //*******************************

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
// Gui::getOpscreenRectOfTheme
//*******************************
SDL_Rect Gui::getOpscreenRectOfTheme() {
    SDL_Rect rect;
    rect.x = atoi(themeData.values["opscreenx"].c_str());
    rect.y = atoi(themeData.values["opscreeny"].c_str());
    rect.w = atoi(themeData.values["opscreenw"].c_str());
    rect.h = atoi(themeData.values["opscreenh"].c_str());

    return rect;
}

//*******************************
// Gui::getTextRectOfTheme
//*******************************
SDL_Rect Gui::getTextRectOfTheme() {
    SDL_Rect rect;
    rect.x = atoi(themeData.values["textx"].c_str());
    rect.y = atoi(themeData.values["texty"].c_str());
    rect.w = atoi(themeData.values["textw"].c_str());
    rect.h = atoi(themeData.values["texth"].c_str());

    return rect;
}

//*******************************
// Gui::FC_getFontRect
// set rect.h to font height, init rest to 0
//*******************************
FC_Rect Gui::FC_getFontRect(FC_Font_Shared font) {
    FC_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0;
    rect.h = FC_GetLineHeight(font);

    return rect;
}

//*******************************
// Gui::FC_getFontTextRect
// get Rect of font text
//*******************************
FC_Rect Gui::FC_getFontTextRect(FC_Font_Shared font, const char *text) {
    FC_Rect rect;
    rect.x = 0;
    rect.y = 0;
    if (text != nullptr)
        rect.w = FC_GetWidth(font, text);
    else
        rect.w = 0;
    rect.h = FC_GetLineHeight(font);

    return rect;

}

//*******************************
// Gui::getTextureAndRect
//*******************************
void Gui::getTextureAndRect(int x, int y, const char *text, FC_Font_Shared font,
                            SDL_Shared<SDL_Texture> *texture, FC_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Shared<SDL_Surface> surface;
    string fg = themeData.values["text_fg"];
    SDL_Color textColor = {getR(fg), getG(fg), getB(fg), 0};

    if (strlen(text) == 0) {
        Uint32 pixelFormat = SDL_GetWindowPixelFormat(window);
        *texture = SDL_CreateTexture(renderer, pixelFormat, SDL_TEXTUREACCESS_STATIC, 0, 0);
        rect->x = 0;
        rect->y = 0;
        rect->h = 0;
        rect->w = 0;
        return;
    }

    surface = TTF_RenderUTF8_Blended(get_ttf_source(font), text, textColor);
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
        rect.x = atoi(themeData.values["opscreenx"].c_str());
        rect.y = atoi(themeData.values["opscreeny"].c_str());
        rect.w = logoRect.w / 3;
        rect.h = logoRect.h / 3;
        SDL_RenderCopy(renderer, logo, nullptr, &rect);
        return rect.y + rect.h;
    }
}

//*******************************
// Gui::loadThemeTexture
//*******************************
SDL_Shared<SDL_Texture>
Gui::loadThemeTexture(const string& themePath, const string& defaultPath, const string& texname) {
    SDL_Shared<SDL_Texture> tex = nullptr;
    if (DirEntry::exists(themePath + themeData.values[texname])) {
        tex = IMG_LoadTexture(renderer, (themePath + themeData.values[texname]).c_str());
    } else {
        tex = IMG_LoadTexture(renderer, (defaultPath + defaultData.values[texname]).c_str());
    }
    return tex;
}

//*******************************
// Gui::loadAssets
//*******************************
void Gui::loadAssets(bool reloadMusic) {
    // check theme exists - otherwise back to aergb

    string defaultPath = Env::getPathToThemesDir() + sep + "default" + sep;
    themePath = getCurrentThemePath() + sep;

    cout << "Loading UI theme:" << themePath << endl;
    if (!DirEntry::exists(themePath + "theme.ini"))
    {
        themePath=defaultPath;
        cfg.inifile.values["theme"] = "default";
        cfg.save();
    }

    defaultData.load(defaultPath + "theme.ini");
    themeData.load(defaultPath + "theme.ini");
    themeData.OverwriteAndAppend(themePath + "theme.ini");    // adds to default/theme.ini values

    bool reloading = false;

    if (backgroundImg != nullptr) {
        Mix_FreeChunk(cursor);
        Mix_FreeChunk(cancel);
        Mix_FreeChunk(home_down);
        Mix_FreeChunk(home_up);
        reloading = true;
        backgroundImg = nullptr;
    }

    logoRect.x = atoi(themeData.values["lpositionx"].c_str());
    logoRect.y = atoi(themeData.values["lpositiony"].c_str());
    logoRect.w = atoi(themeData.values["lw"].c_str());
    logoRect.h = atoi(themeData.values["lh"].c_str());

    backgroundImg = loadThemeTexture(themePath, defaultPath, "background");
    logo = loadThemeTexture(themePath, defaultPath, "logo");
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

    buttonTextureMap["O"] = loadThemeTexture(themePath, defaultPath, "circle");
    buttonTextureMap["X"] = loadThemeTexture(themePath, defaultPath, "cross");
    buttonTextureMap["T"] = loadThemeTexture(themePath, defaultPath, "triangle");
    buttonTextureMap["S"] = loadThemeTexture(themePath, defaultPath, "square");
    buttonTextureMap["Select"] = loadThemeTexture(themePath, defaultPath, "select");
    buttonTextureMap["Start"] = loadThemeTexture(themePath, defaultPath, "start");
    buttonTextureMap["L1"] = loadThemeTexture(themePath, defaultPath, "l1");
    buttonTextureMap["R1"] = loadThemeTexture(themePath, defaultPath, "r1");
    buttonTextureMap["L2"] = loadThemeTexture(themePath, defaultPath, "l2");
    buttonTextureMap["R2"] = loadThemeTexture(themePath, defaultPath, "r2");
    buttonTextureMap["Check"] = loadThemeTexture(themePath, defaultPath, "check");
    buttonTextureMap["Uncheck"] = loadThemeTexture(themePath, defaultPath, "uncheck");
    buttonTextureMap["Esc"] = loadThemeTexture(themePath, defaultPath, "esc");
    buttonTextureMap["Enter"] = loadThemeTexture(themePath, defaultPath, "enter");
    buttonTextureMap["Tab"] = loadThemeTexture(themePath, defaultPath, "tab");

    string fontPath = (themePath + themeData.values["font"]);
    int fontSize = 0;
    string fontSizeString = themeData.values["fsize"];
    if (fontSizeString != "")
        fontSize = atoi(fontSizeString.c_str());
    themeFont = Fonts::openNewSharedCachedFont(fontPath, fontSize, renderer);

    if (reloadMusic) {
        if (music != nullptr) {

            Mix_FreeMusic(music);
            music = nullptr;
        }
    }
    bool customMusic = false;
    int freq = 32000;
    string musicPath = themeData.values["music"];
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
    cursor = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "cursor.wav").c_str());
    cancel = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "cancel.wav").c_str());
    home_up = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "home_up.wav").c_str());
    home_down = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "home_down.wav").c_str());
    resume = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "resume_new.wav").c_str());

    if (reloadMusic)
    if (cfg.inifile.values["nomusic"] != "true")
        if (themeData.values["loop"] != "-1") {


            if (!customMusic) {
                music = Mix_LoadMUS((themePath + themeData.values["music"]).c_str());
                if (music == nullptr) { printf("Unable to load Music file: %s\n", Mix_GetError()); }
                if (Mix_PlayMusic(music, themeData.values["loop"] == "1" ? -1 : 0) == -1) {
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
        while (SDL_PollEvent(&e)) {
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
    string RA_or_EA = _("RetroArch");
    string cfgPath = Env::getPathToRetroarchDir() + sep + "retroboot/retroboot.cfg";
    if (DirEntry::exists(cfgPath)) {
        Inifile RBcfg;
        RBcfg.load(cfgPath);
        if (RBcfg.values["use_emulationstation"] == "1")
            RA_or_EA = _("EmulationStation");
    }
    mainMenu += "|@S|  " + RA_or_EA + "   ";
    mainMenu += "|@T|  " + _("About") + "  |@Select|  " + _("Options") + " ";
    mainMenu += "|@L1| " + _("Advanced");
    mainMenu += " |@L2|+|@R2|" + _("Power Off");

    string forceScanMenu = _("Games changed. Press") + "  |@X|  " + _("to scan") + "|";
    string otherMenu;
    if (Env::autobleemKernel)
        otherMenu += "|@S|  " + _("Hardware Information") + "  ";
    otherMenu += "|@X|  " + _("Memory Cards") + "   |@O|  " + _("Game Manager");
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
        while (SDL_PollEvent(&e)) {

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

            if (e.type == SDL_JOYDEVICEADDED )
            {
                int joyid = e.jdevice.which;
                SDL_Joystick *joystick = SDL_JoystickOpen(joyid);
                if (!mapper.isKnownPad(SDL_JoystickInstanceID(joystick))) {
                    cout << "New pad type" << endl;
                    // new controller configuration
                    auto cfgPad = new GuiPadConfig(renderer);
                    cfgPad->joyid = SDL_JoystickInstanceID(joystick);
                    cfgPad->show();
                    delete cfgPad;
                    if (!forceScan) {
                        drawText(mainMenu);

                    } else {
                        drawText(forceScanMenu);
                    }
                }
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
                            if (Env::autobleemKernel) {
                                string cmd = Env::getPathToAppsDir() + sep + "pscbios/run.sh";
                                vector<const char *> argvNew { cmd.c_str(), nullptr };
                                Util::execFork(cmd.c_str(), argvNew);
                            }
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
// Gui::getAllTokenInfoForLineOfTextAndEmojis
// break up the text into tokens of pure text or an emoji icon marker
// return a vector of the text, emoji texture pointers, width and height of each token and the total width and height.
//*******************************
Gui::AllTextOrEmojiTokenInfo Gui::getAllTokenInfoForLineOfTextAndEmojis(FC_Font_Shared font, const std::string & _text) {
    //
    // break up the text into tokens of text and emoji markers
    //
    string text = _text;
    if (text.empty()) text = " ";
    if (text.back() != '|') {
        text = text + "|";  // in case a terminating | is needed
    }
    auto tokenStrings =  Util::getTokens(text, '|');

    //
    // fill the info structures
    //
    AllTextOrEmojiTokenInfo allInfo;

    for (const auto& tokenString : tokenStrings) {      // for each token string
        if (tokenString == "") continue;
        TextOrEmojiTokenInfo tokenInfo;
        tokenInfo.tokenString = tokenString;
        if (tokenString[0] == '@') {    // if emoji marker
            int w, h;
            auto it = buttonTextureMap.find(tokenString.c_str()+1);
            if (it != buttonTextureMap.end()) {
                tokenInfo.emoji = it->second;   // save the texture pointer
                SDL_QueryTexture(it->second, nullptr, nullptr, &w, &h);
                tokenInfo.size.w = w;
                tokenInfo.size.h = h;
                // update overall size
                allInfo.totalSize.w += w;
                if (h > allInfo.totalSize.h)
                    allInfo.totalSize.h = h;
                // add the token info
                allInfo.info.emplace_back(tokenInfo);
            } else {
                cout << "emoji not found for " << tokenString << endl;
            }
        } else {
            tokenInfo.size.w += FC_GetWidth(font, tokenString.c_str());
            tokenInfo.size.h = FC_GetLineHeight(font);
            // update overall size
            allInfo.totalSize.w += tokenInfo.size.w;
            if (tokenInfo.size.h > allInfo.totalSize.h)
                allInfo.totalSize.h = tokenInfo.size.h;
            // add the token info
            allInfo.info.emplace_back(tokenInfo);
        }
    }

    return allInfo;
}

//*******************************
// Gui::renderAllTokenInfo
// renders/draws the text and emoji icons at the chosen position on the screen
//*******************************
void Gui::renderAllTokenInfo(FC_Font_Shared font,
                             AllTextOrEmojiTokenInfo& allTokenInfo, int x, int y, XAlignment xAlign) {

    if (!font)
        font = themeFont;   // default to themeFont

    if (xAlign == XALIGN_CENTER) {
        x = (SCREEN_WIDTH / 2) - allTokenInfo.totalSize.w / 2;
    } else if (xAlign == XALIGN_RIGHT) {
        x = SCREEN_WIDTH - x - allTokenInfo.totalSize.w;
    }

    // adjust the text y position so the text is centered on the emoji centers
    int fontHeight = FC_GetLineHeight(font);
    int text_y = y + (allTokenInfo.totalSize.h - fontHeight)/2;

    for (auto& tokenInfo : allTokenInfo.info) {
        if (tokenInfo.emoji) {
            // the token is an emoji texture
            FC_Rect rect;
            rect.x = x;
            rect.y = y + (allTokenInfo.totalSize.h - tokenInfo.size.h); // center the emoji in y
            rect.w = tokenInfo.size.w;
            rect.h = tokenInfo.size.h;
            SDL_RenderCopy(renderer, tokenInfo.emoji, nullptr, &rect);
            x += tokenInfo.size.w;
        } else {
            // the token is text
            FC_DrawAlign(font, renderer, x, text_y, FC_ALIGN_LEFT, tokenInfo.tokenString.c_str());
            x += tokenInfo.size.w;
        }
    }
}

//*******************************
// Gui::renderText
// renders/draws the line of text and emoji icons at the chosen position on the screen.  returns the height.
//*******************************
int Gui::renderText(FC_Font_Shared font, const string & text, int x, int y, XAlignment xAlign) {
    if (!font)
        font = themeFont;   // default to themeFont
    AllTextOrEmojiTokenInfo allTokenInfo = getAllTokenInfoForLineOfTextAndEmojis(font, text);
    renderAllTokenInfo(themeFont, allTokenInfo, x, y, xAlign);

    return allTokenInfo.totalSize.h;    // return the height
}

//*******************************
// Gui::renderTextOnly_WithColor
// if background == true it draws a solid grey box around/behind the text
// this routine does not support emoji icons.  text only.
//*******************************
void Gui::renderTextOnly_WithColor(int x, int y, const std::string &text,
                                   SDL_Color textColor, FC_Font_Shared font,
                                   XAlignment xAlign, bool background) {
    auto gui = Gui::getInstance();
    int text_width = FC_GetWidth(font, text.c_str());
    int text_height = FC_GetLineHeight(font);

    if (xAlign == XALIGN_CENTER) {
        x = (SCREEN_WIDTH / 2) - (text_width / 2);
    } else if (xAlign == XALIGN_RIGHT) {
        x = SCREEN_WIDTH - x - text_width;
    }

    SDL_Rect rect{0, 0, 0, 0};

    auto renderer = Gui::getInstance()->renderer;

    if (text.size() == 0) {
        rect.x = 0;
        rect.y = 0;
        rect.h = 0;
        rect.w = 0;
    } else {
        rect.x = x;
        rect.y = y;
        rect.w = text_width;
        rect.h = text_height;
    }

    if (background) {
        // render a grey box behind the text
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 70);
        SDL_Rect backRect;
        backRect.x = rect.x - 10;
        backRect.y = rect.y - 2;
        backRect.w = rect.w + 20;
        backRect.h = rect.h + 4;

        SDL_RenderFillRect(renderer, &backRect);
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    if (textColor.a == 0)
        cout << "alpha in SDL_Color is 0 for text: " << text << endl;
    assert(textColor.a > 0);    // rendering the text will do no good if it's transparent
#else
    if (textColor.unused == 0)
        cout << "alpha in SDL_Color is 0 for text: " << text << endl;
    assert(textColor.unused > 0);
#endif
    FC_DrawColor(font, renderer, x, y, textColor, text.c_str());
};

//*******************************
// Gui::getCheckIconWidth
// returns the width of the check/uncheck icon textures
//*******************************
int Gui::getCheckIconWidth() {
    int checkIconWidth=0;
    int checkIconHeight=0;
    auto it = buttonTextureMap.find("Check");
    if (it != buttonTextureMap.end()) {
        SDL_QueryTexture(it->second, nullptr, nullptr, &checkIconWidth, &checkIconHeight);
    } else {
        cout << "missing check icon" << endl;
        assert(false);
    }

    return checkIconWidth;
}

//*******************************
// Gui::renderStatus
//*******************************
void Gui::renderStatus(const string &text, int posy) {
    string bg = themeData.values["text_bg"];

    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeData.values["textalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect = getTextRectOfTheme();
    SDL_RenderFillRect(renderer, &rect);

    int y = atoi(themeData.values["ttop"].c_str());
    if (posy!=-1)
        y=posy; // override the bottom status y position.  so far this has never been used.

    renderText(themeFont, text, 0, y, XALIGN_CENTER);
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
void Gui::renderLabelBox(int line, int yoffset) {
    string bg = themeData.values["label_bg"];
    Uint16 fontHeight = FC_GetLineHeight(themeFont);
    SDL_Rect opscreen = getOpscreenRectOfTheme();
    SDL_Rect rectSelection;
    rectSelection.x = opscreen.x + 5;
    rectSelection.y = yoffset + fontHeight * (line);
    rectSelection.w = opscreen.w - 10;
    rectSelection.h = fontHeight;

    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeData.values["keyalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &rectSelection);
}

//*******************************
// Gui::renderSelectionBox
//*******************************
void Gui::renderSelectionBox(int line, int yoffset, int xoffset, FC_Font_Shared font) {
    SDL_Shared<SDL_Texture> textTex;
    if (!font)
        font = themeFont;

    string fg = themeData.values["text_fg"];
    Uint16 fontHeight = FC_GetLineHeight(font);
    SDL_Rect opscreen = getOpscreenRectOfTheme();
    SDL_Rect rectSelection;
    rectSelection.x = opscreen.x + 5 + xoffset;
    rectSelection.y = yoffset + fontHeight * (line);
    rectSelection.w = opscreen.w - 10 - xoffset;
    rectSelection.h = fontHeight;

    SDL_SetRenderDrawColor(renderer, getR(fg), getG(fg), getB(fg), 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawRect(renderer, &rectSelection);
}

//*******************************
// Gui::renderTextLineOptions
//*******************************
int Gui::renderTextLineOptions(const string &_text, int line, int yoffset, XAlignment xAlign, int xoffset) {
    string text = _text;

    // if there is a check or uncheck icon, flag which one and remove the emoji toekn from the string
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

    // render the text string without the check/uncheck icon
    int h = renderTextLine(text, line, yoffset, xAlign, xoffset);

    if (button == -1) {
        return h;   // there is no check/uncheck emoji on this line
    }

    // render the check/uncheck icon on the right side of opscreen
    SDL_Rect opscreen = getOpscreenRectOfTheme();
    Uint16 fontHeight = FC_GetLineHeight(themeFont);

    int x = opscreen.x + opscreen.w - 10 - getCheckIconWidth();
    int y = (fontHeight * line) + yoffset;
    if (button == 1) {
        renderText(themeFont, "|@Check|", x, y);
    } else if (button == 0) {
        renderText(themeFont, "|@Uncheck|", x, y);
    }

    return h;
}

//*******************************
// Gui::renderTextLine
//*******************************
int Gui::renderTextLine(const string &text, int line, int yoffset, XAlignment xAlign, int xoffset, FC_Font_Shared font) {
    if (!font)
        font = themeFont;   // default to themeFont

    SDL_Rect opscreen = getOpscreenRectOfTheme();
    Uint16 fontHeight = FC_GetLineHeight(font);
    int x = opscreen.x + 10 + xoffset;
    int y = (fontHeight * line) + yoffset;

    if (line<0)
    {
        line=-line;
        y=line;
    }

    return renderText(font, text, x, y, xAlign);
}

//*******************************
// Gui::getTextRectangleOnScreen
//*******************************
// returns the SDL_Rect of the screen positions if your rendered this text with these args
SDL_Rect Gui::getTextRectangleOnScreen(const string &text, int line, int yoffset,  XAlignment xAlign, int xoffset, FC_Font_Shared font) {
    if (!font)
        font = themeFont;   // default to themeFont

    SDL_Rect opscreen = getOpscreenRectOfTheme();
    Uint16 fontHeight = FC_GetLineHeight(themeFont);
    FC_Rect textRec;
    textRec.x = opscreen.x + 10 + xoffset;
    textRec.y = (fontHeight * line) + yoffset;

    if (line<0)
    {
        line=-line;
        textRec.y=line;
    }

    AllTextOrEmojiTokenInfo allTokenInfo = getAllTokenInfoForLineOfTextAndEmojis(font, text);
    textRec.w = allTokenInfo.totalSize.w;
    textRec.h = allTokenInfo.totalSize.h;
    textRec.x = (SCREEN_WIDTH / 2) - allTokenInfo.totalSize.w / 2;

    return textRec;
}

//*******************************
// Gui::renderTextLineToColumns
//*******************************
int Gui::renderTextLineToColumns(const string &textLeft, const string &textRight,
                                 int xLeft, int xRight,
                                 int line, int yoffset, FC_Font_Shared font) {

            renderTextLine(textLeft,  line, yoffset, XALIGN_LEFT, xLeft, font);
    int h = renderTextLine(textRight, line, yoffset, XALIGN_LEFT, xRight, font);

    return h;   // rectangle height
}

//*******************************
// Gui::renderTextChar
//*******************************
void Gui::renderTextChar(const string &text, int line, int yoffset, int x) {
    Uint16 fontHeight = FC_GetLineHeight(themeFont);
    int y = (fontHeight * line) + yoffset;
    FC_DrawAlign(themeFont, renderer, x, y, FC_ALIGN_LEFT, text.c_str());
}

//*******************************
// Gui::renderTextBar
//*******************************
void Gui::renderTextBar() {
    string bg = themeData.values["main_bg"];
    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeData.values["mainalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2 = getOpscreenRectOfTheme();

    SDL_RenderFillRect(renderer, &rect2);
}

//*******************************
// Gui::renderFreeSpace
//*******************************
void Gui::renderFreeSpace() {
    int x = atoi(themeData.values["fsposx"].c_str());
    int y = atoi(themeData.values["fsposy"].c_str());
    renderText(themeFont, _("Free space") + " : " + Util::getAvailableSpace(), x, y);
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