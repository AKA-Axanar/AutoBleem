//
// Created by screemer on 2018-12-15.
//

#include "scanner.h"
#include "main.h"

static const char GAME_DATA[] = "GameData";
static const char GAME_INI[] = "Game.ini";
static const char PCSX_CFG[] = "pcsx.cfg";
static const char LABEL[] = ".png";
static const char LICENCE[] = ".lic";


bool wayToSort(Game i, Game j) {
    return i.folder_id < j.folder_id;
}

void Scanner::updateDB(Database *db) {
    if (complete)
        for (int i = 0; i < games.size(); i++) {
            Game data = games[i];
            cout << "Inserting game ID: " << data.folder_id << " - " << data.title << endl;
            db->insertGame(data.folder_id, data.title, data.publisher, data.players, data.year);
            for (int i = 0; i < data.discs.size(); i++) {
                db->insertDisc(data.folder_id, i + 1, data.discs[i].diskName);
            }
        }
}

void Scanner::scanDirectory(string path) {
    // clear games list
    games.clear();
    complete = false;

    vector<DirEntry> rootDir = Util::dir(path);
    for (std::vector<int>::size_type i = 0; i != rootDir.size(); i++) {
        DirEntry entry = rootDir[i];
        if (entry.name[0] == '.') continue;
        if (!entry.dir) continue;

        Game game;
        game.folder_id = atoi(entry.name.c_str());
        game.fullPath = path + entry.name + Util::separator();
        game.pathName = entry.name;

        string folderPath = path + entry.name + Util::separator() + GAME_DATA + Util::separator();

        bool gameDataExists = Util::exists(folderPath);
        if (!gameDataExists) {
            cerr << "Game: " << entry.name << " - GameData Not found" << endl;
            Util::createDir(folderPath);


        }
        game.gameDataFound = true;

        vector<DirEntry> gameRoot = Util::dir(path + entry.name + Util::separator());
        for (std::vector<int>::size_type j = 0; j != gameRoot.size(); j++) {
            DirEntry entryGame = gameRoot[j];
            if (entryGame.name[0] == '.') continue;
            if (entryGame.name == GAME_DATA) continue;

            string oldName = path + entry.name + Util::separator() + entryGame.name;
            string newName = folderPath + entryGame.name;
            cerr << "Moving: " << oldName << "  to: " << newName << endl;
            rename(oldName.c_str(), newName.c_str());
        }


        if (!Util::exists(folderPath + GAME_INI)) {
            game.readIni(folderPath + GAME_INI);
            game.gameIniFound = false;
        } else {
            game.gameIniFound = true;
        }


        vector<DirEntry> gameDir = Util::dir(folderPath);
        for (std::vector<int>::size_type j = 0; j != gameDir.size(); j++) {
            DirEntry entryGame = gameDir[j];
            if (entryGame.name[0] == '.') continue;

            if (Util::strcicmp(entryGame.name.c_str(), GAME_INI) == 0) {
                string gameIniPath = folderPath + GAME_INI;
                game.readIni(gameIniPath);
            }

            if (Util::strcicmp(entryGame.name.c_str(), PCSX_CFG) == 0) {
                game.pcsxCfgFound = true;
            }

            if (Util::strcicmp(entryGame.name.c_str(), PCSX_CFG) == 0) {
                game.pcsxCfgFound = true;
            }

            if (Util::strcicmp(entryGame.name.substr(entryGame.name.length() - 4).c_str(), LABEL) == 0) {
                game.imageFound = true;
            }
            if (Util::strcicmp(entryGame.name.substr(entryGame.name.length() - 4).c_str(), LICENCE) == 0) {
                game.licFound = true;
            }


        }
        game.recoverFiles();
        game.print();
        games.push_back(game);


    }
    sort(games.begin(), games.end(), wayToSort);

    complete = true;
}