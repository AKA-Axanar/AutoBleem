/* 
 * File:   main.cpp
 * Author: screemer
 *
 * Created on 11 Dec 2018, 20:37
 */

#include <iostream>
#include "engine/database.h"
#include "engine/scanner.h"
#include "gui/gui.h"
#include "main.h"
#include "ver_migration.h"
#include "engine/coverdb.h"
#include "util.h"
#include <unistd.h>
#include "engine/GetGameDirHierarchy.h"
#include "engine/memcard.h"
#include "lang.h"
#include "launcher/emu_interceptor.h"
#include "launcher/pcsx_interceptor.h"
#include "launcher/retboot_interceptor.h"
#include "engine/GetGameDirHierarchy.h"
#include "environment.h"
#include "launcher/ra_integrator.h"
#include "launcher/launch_interceptor.h"
#include "launcher/gui_app_start.h"
#include "gui/abl.h"

using namespace std;

Database * db;

// these are defined in environment.h and are meant to not be modified once they are initialized here.
extern bool private_singleArgPassed;
extern string private_pathToUSBDrive;
extern string private_pathToGamesDir;
extern string private_pathToRegionalDBFile;
extern string private_pathToInternalDBFile;

//*******************************
// copyGameFilesInGamesDirToSubDirs
//*******************************
// Search for games with supported extension and move to sub-dir
// returns true is any files moved into sub-dirs
bool copyGameFilesInGamesDirToSubDirs(const string & path){
    bool ret = false;
    string fileExt;
    string filenameWE;
    vector<string> extensions;
    vector<string> binList;

    extensions.push_back("pbp");
    extensions.push_back("cue");

    //Getting all files in USBGames Dir
    DirEntries globalFileList = DirEntry::diru(path);
    DirEntries fileList = DirEntry::getFilesWithExtension(path, globalFileList, extensions);

    //On first run, we won't process bin/img files, as cue file may handle a part of them
    for (const auto &entry : fileList){
        Gui::splash(_("Moving :") + " " + entry.name);
        fileExt = DirEntry::getFileExtension(entry.name);
        filenameWE = DirEntry::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + sep + entry.name).c_str(),F_OK) != -1){
            if(fileExt == "cue"){
                binList = DirEntry::cueToBinList(path + sep + entry.name);
                if(!binList.empty()){
                    //Create directory for game
                    DirEntry::createDir(path + sep + filenameWE);
                    //Move cue file
                    DirEntry::renameFile(path + "/" + entry.name, path + sep + filenameWE + "/" + entry.name);
                    //Move bin files
                    for (const auto &bin : binList){
                        Gui::splash(_("Moving :") + " " + bin);
                        DirEntry::renameFile(path + sep + bin, path + sep + filenameWE + sep + bin);
                    }
                    ret = true;
                }
            }else{
                DirEntry::createDir(path + sep + filenameWE);

                DirEntry::renameFile(path + sep + entry.name, path + sep + filenameWE + sep + entry.name);
                ret = true;
            }
        }
    }

    //Next we will read only bin and img files
    extensions.clear();
    extensions.push_back("img");
    extensions.push_back("bin");
    fileList = DirEntry::getFilesWithExtension(path, globalFileList, extensions);
    for (const auto &entry : fileList){
        Gui::splash(_("Moving :") + " " + entry.name);
        fileExt = DirEntry::getFileExtension(entry.name);
        filenameWE = DirEntry::getFileNameWithoutExtension(entry.name);
        //Checking if file exists
        if(access((path + sep + entry.name).c_str(),F_OK) != -1){
            DirEntry::createDir(path + sep + filenameWE);
            DirEntry::renameFile(path + sep + entry.name, path + sep + filenameWE + sep + entry.name);
            ret = true;
        }
    }
    return ret; // true if any game files moved into a sub-dir
}

//*******************************
// scanGames
//*******************************
int scanGames(GamesHierarchy &gamesHierarchy) {
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Scanner> scanner(Scanner::getInstance());

    if (!db->createInitialDatabase()) {
        cout << "Error creating db structure" << endl;

        return EXIT_FAILURE;
    };

    if (!db->truncate())
    {
        gui->drawText("ERROR IN DB");
        sleep(1);
        return EXIT_FAILURE;
    }

    scanner->scanUSBGamesDirectory(gamesHierarchy);
    scanner->updateRegionalDB(gamesHierarchy, gui->db);

    gui->drawText(_("Total:") + " " + to_string(scanner->gamesToAddToDB.size()) + " " + _("games scanned") + ".");
    sleep(1);
    scanner->gamesToAddToDB.clear();
    return (EXIT_SUCCESS);
}

//*******************************
// rewriteGamelistXmlFile
// the /Games/gamelist.xml is for Emulation Station to find the PS1 cover files
//*******************************
void rewriteGamelistXml() {
    // this file was used during 0.9.0 testing.  it must be removed or ES will use it by mistake.
    DirEntry::removeFile(Env::getPathToGamesDir() + sep + "gamelist.xml");

    string path = Env::getPathToRetroarchDir() + sep + "retroboot/emulationstation/.emulationstation/gamelists/psx";
    DirEntry::createDir(path);
    string filePath = path + sep + "gamelist.xml";

    DirEntry::removeFile(filePath);

    PsGames currentGames;
    Gui::getInstance()->db->getGames(&currentGames);

    ofstream xml;
    xml.open(filePath.c_str(), ios::binary);

    xml << "<?xml version=\"1.0\"?>" << endl;
    xml << "<gameList>" << endl;

    auto makeGamesPathRelative = [] (const string& oldPath) -> string {
        string newPath=oldPath;
        size_t pos = newPath.find("/Games");
        if (pos != string::npos) {
            newPath.erase(0, pos-1 + sizeof("/Games"));
            newPath = "." + newPath;
        }
        return newPath;
    };

    for (const auto& game : currentGames) {
        xml << "\t<game>" << endl;

        xml << "\t\t<path>" << makeGamesPathRelative(game->folder) << "</path>" << endl;
        xml << "\t\t<name>" << game->title << "</name>" << endl;
        xml << "\t\t<desc>" << game->title << "</desc>" << endl;
        string imagePath = game->folder + sep + game->base + ".png";
        xml << "\t\t<image>" << makeGamesPathRelative(imagePath) << "</image>" << endl;

        xml << "\t</game>" << endl;
    }
    xml << "</gameList>" << endl;

    xml.close();
}

//*******************************
// main
//*******************************
int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    Env::autobleemKernel = DirEntry::exists("/autobleem");
    shared_ptr<Lang> lang(Lang::getInstance());


    if (argc == 1 + 1) {
        // the single arg is the path to the usb drive
        private_singleArgPassed = true;
        private_pathToUSBDrive = argv[1];
        private_pathToRegionalDBFile = private_pathToUSBDrive + sep + "System/Databases/regional.db";
        private_pathToInternalDBFile = private_pathToUSBDrive + sep + "System/Databases/internal.db";
        private_pathToGamesDir = private_pathToUSBDrive + sep + "Games";
    } else if (argc == 1 + 2) {
        // the two args are the path to the regional.db file and the path to the /Games dir on the usb drive
        private_singleArgPassed = false;
        private_pathToRegionalDBFile = argv[1];
#if defined(__x86_64__) || defined(_M_X64) || defined (PI_DEBUG)
        private_pathToInternalDBFile = "internal.db";   // it's in the same dir as the autobleem-gui app you are debugging
#else
        private_pathToInternalDBFile = "/media/System/Databases/internal.db";
#endif
        private_pathToGamesDir = argv[2];
        private_pathToUSBDrive = DirEntry::getDirNameFromPath(private_pathToGamesDir);
    }
    else {
        cout << "USAGE: autobleem-gui /path/dbfilename.db /path/to/games" << endl;
        return EXIT_FAILURE;
    }

    // now that Environment is setup, routines that need the paths can be called
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Scanner> scanner(Scanner::getInstance());
    gui->mapper.init();
    lang->load(gui->cfg.inifile.values["language"]);

    Coverdb *coverdb = new Coverdb();
    gui->coverdb = coverdb;

    db = new Database();
    if (!db->connect(Env::getPathToRegionalDBFile())) {
        delete db;
        return EXIT_FAILURE;
    }
    gui->db = db;
    db->createInitialDatabase();

    // if the /System/Databases/internal.db doesn't exist make a copy from the PSC
    cout << "Importing internal games from PSC to USB" << endl;
    Util::execUnixCommand("/media/Autobleem/rc/backup_internal.sh");

    // add favorites and history columns to internal.db if the column doesn't exist
    Database *internalDB = new Database();
    if (!internalDB->connect(Env::getPathToInternalDBFile())) {
        delete internalDB;
        return EXIT_FAILURE;
    }
    gui->internalDB = internalDB;
    gui->internalDB->addFavoriteColumn(); // add the favorites column if it doesn't exist
    gui->internalDB->addHistoryColumn();  // add the history column if it doesn't exist
    gui->internalDB->addLastPlayedColumn();  // add the last played column if it doesn't exist
    gui->internalDB->addPlayUsingRAColumn(); // add the favorites column if it doesn't exist

    string dbpath = Env::getPathToRegionalDBFile();
    string pathToGamesDir = Env::getPathToGamesDir();

    Memcard *memcardOperation = new Memcard(pathToGamesDir);
    memcardOperation->restoreAll(Env::getPathToSaveStatesDir());
    delete memcardOperation;

    string prevPath = Env::getWorkingPath() + sep + "autobleem.prev";
    bool prevFileExists = DirEntry::exists(prevPath);
    bool gamelistXmlExists = DirEntry::exists(Env::getPathToRetroarchDir() + sep + "retroboot/emulationstation/.emulationstation/gamelists/psx/gamelist.xml");

    GamesHierarchy gamesHierarchy;
    gamesHierarchy.getHierarchy(pathToGamesDir);

    USBGames allGames = gamesHierarchy.getAllGames();
    USBGame::sortByFullPath(allGames);

    bool autobleemPrevOutOfDate = gamesHierarchy.gamesDoNotMatchAutobleemPrev(prevPath);
    bool thereAreRawGameFilesInGamesDir = scanner->areThereGameFilesInDir(pathToGamesDir);

    if (!prevFileExists || !gamelistXmlExists || thereAreRawGameFilesInGamesDir || autobleemPrevOutOfDate) {
        scanner->forceScan = true;
    }

    gui->display(scanner->forceScan, pathToGamesDir, db, false);

    if (thereAreRawGameFilesInGamesDir)
        copyGameFilesInGamesDirToSubDirs(pathToGamesDir);   // the gui->display needs to be up first

    while (gui->menuOption == MENU_OPTION_SCAN || gui->menuOption == MENU_OPTION_START) {

        gui->menuSelection();
        gui->saveSelection();
        if (gui->menuOption == MENU_OPTION_SCAN) {
            gamesHierarchy.getHierarchy(pathToGamesDir);
            // write the prev file now.  if you call it after scanGames the games that failed to verify will have been
            // removed from the hierarchy and it force you to rescan on every boot.
            gamesHierarchy.writeAutobleemPrev(prevPath);
            scanGames(gamesHierarchy);
            rewriteGamelistXml();

            if (gui->forceScan) {
                gui->forceScan = false;
            } else {
                //break;
            }
        }

        if (gui->menuOption == MENU_OPTION_START) {
            cout << "Starting game" << endl;
            gui->finish();

            int numtimesopened, frequency, channels;
            Uint16 format;
            numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);
            for (int i=0;i<numtimesopened;i++)
            {
                Mix_CloseAudio();
            }
            while(Mix_QuerySpec(&frequency, &format, &channels))
            {
                Mix_CloseAudio();
            }


            gui->mapper.flushPads();

            gui->saveSelection();
            EmuInterceptor *interceptor;
            if (gui->runningGame->foreign)
            {
                if (!gui->runningGame->app)
                {
                    interceptor = new RetroArchInterceptor();
                } else {
                     interceptor =  new LaunchInterceptor();
                }
            } else {
                if (gui->emuMode == EMU_PCSX) {
                    interceptor = new PcsxInterceptor();
                } else {
                    interceptor = new RetroArchInterceptor();
                }
            }

            interceptor->memcardIn(gui->runningGame);
            interceptor->prepareResumePoint(gui->runningGame, gui->resumepoint);
            interceptor->execute(gui->runningGame, gui->resumepoint );
            interceptor->memcardOut(gui->runningGame);
            delete (interceptor);

            bool reloadFavHist {false};
            if (gui->runningGame->foreign)
                reloadFavHist = true;
            else if (gui->emuMode != EMU_PCSX)
                reloadFavHist = true;

            if (reloadFavHist) {
                auto ra = RAIntegrator::getInstance();
                ra->reloadFavorites();  // they could have changed
                ra->reloadHistory();  // they could have changed
            }

            usleep(300*1000);


            gui->mapper.probePads();
            gui->runningGame.reset();    // replace with shared_ptr pointing to nullptr
            gui->startingGame = false;
            // remove all events if something left
            SDL_PumpEvents();
            SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);

            gui->display(false, pathToGamesDir, db, true);
        }
    }
    db->disconnect();
    delete db;
	db = nullptr;

    internalDB->disconnect();
    delete internalDB;
	internalDB = nullptr;

    Gui::splash(_("Loading ... Please Wait ..."));
    gui->finish();
    SDL_Quit();
    delete coverdb;

    exit(0);
}
