#include "environment.h"
#include "DirEntry.h"
#include <dirent.h>
#include <unistd.h>
#include <climits>

using namespace std;

// these strings are initialized by main.cpp.  once initialized they should not be modified.
// they are not declared in the header to prevent outside code from accidentally modifying them.

// passing a single arg on the debug command line instead of passing two args is optional.
//
// in single arg mode you pass the path to the root of the flash USB drive.  in this mode, as much as possible, files
// from the USB drive are used instead of files in the debug build environment.  this allows you to debug the files
// on the USB drive.  this also allows someone to send you the contents of a USB drive to debug a
// problem.  you will use the UI theme, the menu themes, the cover db files, the regional db file, the .prev file,
// the retroarch playlists, the rom files, the lang files, and the config.ini file, etc.

bool private_singleArgPassed = false;
string private_pathToUSBDrive;
string private_pathToGamesDir;
string private_pathToRegionalDBFile;
string private_pathToInternalDBFile;
bool Env::autobleemKernel = false;
bool Env::hiddenMenuEnabled = false;

//*******************************
// Environment:: One Liners
//*******************************

string Environment::getPathToUSBRoot() {
  return private_pathToUSBDrive;
}

string Environment::getPathToAutobleemDir() {
    return private_pathToUSBDrive + sep + "Autobleem";
}

string Environment::getPathToAppsDir() {
    return private_pathToUSBDrive + sep + "Apps";
}

std::string Environment::getPathToRCDir() {
    return getPathToAutobleemDir() + sep + "rc";
}

string Environment::getPathToGamesDir() {
    return private_pathToGamesDir;
}

string Environment::getPathToMemCardsDir() {
    return private_pathToGamesDir + sep + "!MemCards";
}

string Environment::getPathToSaveStatesDir() {
    return private_pathToGamesDir + sep + "!SaveStates";
}

string Environment::getPathToSystemDir() {
    return private_pathToUSBDrive + sep + "System";
}

string Environment::getPathToRetroarchDir() {
    return private_pathToUSBDrive + sep + "retroarch";
}

string Environment::getPathToRetroarchPlaylistsDir() {
    return getPathToRetroarchDir() + sep + "playlists";
}

string Environment::getPathToRetroarchCoreFile() {
    return getPathToRetroarchDir() + sep + "cores/km_pcsx_rearmed_neon_libretro.so";
}

string Environment::getPathToRomsDir() {
    return private_pathToUSBDrive + sep + "roms";
}

// includes the "regional.db" filename
string Environment::getPathToRegionalDBFile() {
    return private_pathToRegionalDBFile;
}

// includes the "internal.db" filename
string Environment::getPathToInternalDBFile() {
    return private_pathToInternalDBFile;
}

#if DISPLAY_NETWORK_MENU
// for networking
string Environment::getPathToBleemsyncDir() {
    return getPathToUSBRoot() + sep + "bleemsync";
}

string Environment::getPathToBleemsyncCFGDir() {
    return getPathToBleemsyncDir() + sep + "etc/bleemsync/CFG";
}

string Environment::getPathToBleemsyncWPADir() {
    return getPathToBleemsyncDir() + sep + "network/etc/wpa_supplicant";
}

string Environment::getPathToLogsDir() {
    return private_pathToUSBDrive + sep + "logs";
}
#endif

//*******************************
// Environment::getWorkingPath
// 1 arg: "usb:/Autobleem/bin/autobleem"
// 2 arg: autobleem-gui executable dir
// PSC: autobleem-gui executable dir
//*******************************
string Environment::getWorkingPath() {
#if defined(__x86_64__) || defined(_M_X64)
    if (private_singleArgPassed) {
        string path = private_pathToUSBDrive + sep + "Autobleem/bin/autobleem";
        return path;
    } else {
        char temp[PATH_MAX];
        return (getcwd(temp, sizeof(temp)) ? string(temp) : string(""));
    }
#else
    char temp[PATH_MAX];
        return (getcwd(temp, sizeof(temp)) ? string(temp) : string(""));
#endif
}

//*******************************
// Environment::getSonyPath
// 1 arg: "usb:/Autobleem/bin/autobleem/sony"
// 2 arg: autobleem-gui executable dir + sep + "sony"
// PSC: autobleem-gui executable dir
//*******************************
string Environment::getSonyPath() {
#if defined(__x86_64__) || defined(_M_X64)
    return getWorkingPath() + sep + "sony";
#else
    return "/usr/sony/share/data";
#endif
}

//*******************************
// Environment::getSonyFontPath
// 1 arg: "usb:/Autobleem/bin/autobleem/sony/font"
// 2 arg: autobleem-gui executable dir + sep + "sony" + sep + "font"
// PSC: autobleem-gui executable dir
//*******************************
string Environment::getSonyFontPath() {
    return getSonyPath() + sep + "font";
}

#if 0
//*******************************
// Environment::getPathToWorkingPathFile
// return path to file in working path
// 1 arg: "usb:/Autobleem/bin/autobleem/filename"
// 2 arg: autobleem-gui executable dir/filename
// PSC: autobleem-gui executable dir/filename
//*******************************
string Environment::getPathToWorkingPathFile(const std::string &filename) {
    return getWorkingPath() + sep + filename;
}
#endif

//*******************************
// Environment::getPathToThemesDir
// 1 arg: "usb:/themes", 2 arg: "./themes"
// PSC: "/media/themes"
//*******************************
string Environment::getPathToThemesDir() {
#if defined(__x86_64__) || defined(_M_X64)
    if (private_singleArgPassed) {
        string path = private_pathToUSBDrive + sep + "themes";
        return path;
    } else {
        string path = getWorkingPath() + sep + "themes";
        return path;
    }
#else
    string path =  "/media/themes";
    return path;
#endif
}

//*******************************
// Environment::getPathToCoversDBDir
// 1 arg: "usb:/Autobleem/bin/db", 2 arg: "../db"
// PSC: "../db"
//*******************************
string Environment::getPathToCoversDBDir() {
#if defined(__x86_64__) || defined(_M_X64)
    if (private_singleArgPassed) {
        string path = private_pathToUSBDrive + sep + "Autobleem/bin/db";
        return path;
    } else {
        string path = "../db";
        return path;
    }
#else
    string path =  "../db";
    return path;
#endif
}
