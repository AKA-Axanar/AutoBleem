#pragma once

#include <string>

#define DISPLAY_NETWORK_MENU 0

struct Environment {
    static std::string getPathToUSBRoot();
    static std::string getPathToAutobleemDir();
    static std::string getPathToSharedThemeFiles();
    static std::string getPathToAppsDir();
    static std::string getPathToRCDir();
    static std::string getPathToGamesDir();
    static std::string getPathToMemCardsDir();
    static std::string getPathToSaveStatesDir();
    static std::string getPathToSystemDir();
    static std::string getPathToRetroarchDir();
    static std::string getPathToRetroarchPlaylistsDir();
    static std::string getPathToRetroarchCoreFile();
    static std::string getPathToRomsDir();
    static std::string getPathToRegionalDBFile();   // includes the "regional.db" filename
    static std::string getPathToInternalDBFile();   // includes the "internal.db" filename

#if DISPLAY_NETWORK_MENU
// for networking
    static std::string getPathToBleemsyncDir();
    static std::string getPathToBleemsyncCFGDir();
    static std::string getPathToBleemsyncWPADir();
    static std::string getPathToLogsDir();
#endif

    static std::string getWorkingPath();  // 1 arg: "usb:/Autobleem/bin/autobleem", 2 arg: autobleem-gui executable dir
    static std::string getSonyPath();  // 1 arg: "usb:/Autobleem/bin/autobleem/sony", 2 arg: "" + sep + "sony"
    static std::string getSonyFontPath();  // 1 arg: "usb:/Autobleem/bin/autobleem/sony", 2 arg: "" + sep + "sony"
//    static std::string getPathToWorkingPathFile(const std::string &filename);   // return path to file in working path

    static std::string getPathToThemesDir();        // "usb:/themes" or "./themes"
    static std::string getPathToCoversDBDir();   // "usb:/Autobleem/bin/db" or "../db"

    static bool autobleemKernel;        // true if the kernel is the AutoBleem Kernel
    static bool hiddenMenuEnabled;
};

using Env = Environment;
