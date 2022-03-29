//
// Created by steve on 3/24/22.
//

#include "LightgunGames.h"
#include "util.h"
#include "environment.h"
#include <algorithm>
#include "DirEntry.h"
#include "engine/database.h"
#include "gui/gui.h"
#include "launcher/ra_integrator.h"
#include <iostream>

using namespace std;

std::string LightgunGames::filename;
std::vector<std::string> LightgunGames::lightgunGamePaths;

//********************
// LightgunGames
//********************
LightgunGames::LightgunGames() {
    filename = Environment::getWorkingPath() + sep + string("lightguns.txt");
    lightgunGamePaths = Util::ReadTextFileAsAStringArray(filename, true);

    // there shouldn't be any blank lines unless someone hand edited it.  but just to be sure.
    lightgunGamePaths.erase(remove_if(begin(lightgunGamePaths), end(lightgunGamePaths), [] (const string& str) { return str == ""; }), end(lightgunGamePaths));
}

//********************
// PathForLightgunFile
//********************
string LightgunGames::PathForLightgunFile(PsGamePtr game) {
    if (game == nullptr)
        return "";

    if (game->foreign)
        return game->image_path;   // Retroarch roms game
    else
        return game->folder;       // /Games PS1 game
   
}

//********************
// UpdateFile
//********************
void LightgunGames::UpdateFile() {
    Util::WriteStringsToTextFile(lightgunGamePaths, filename, true);
}

//********************
// IsGameALightgunGame
//********************
bool LightgunGames::IsGameALightgunGame(PsGamePtr game) {
    return IsGameALightgunGame(PathForLightgunFile(game));
}

bool LightgunGames::IsGameALightgunGame(const std::string& gamepath) {
    if (gamepath == "")
        return false;

    auto it = find(begin(lightgunGamePaths), end(lightgunGamePaths), gamepath);
    return it != end(lightgunGamePaths);
}

//********************
// AddGame
//********************
void LightgunGames::AddGame(PsGamePtr game) {
    if (game)
        AddGame(PathForLightgunFile(game));
}

void LightgunGames::AddGame(const std::string& gamepath) {
    if (gamepath == "")
        return;

    if (!IsGameALightgunGame(gamepath)) {
        lightgunGamePaths.emplace_back(gamepath);
        UpdateFile();
    }
}

//********************
// RemoveGame
//********************
void LightgunGames::RemoveGame(PsGamePtr game) {
    if (game)
        RemoveGame(PathForLightgunFile(game));
}

void LightgunGames::RemoveGame(const std::string& gamepath) {
    if (gamepath == "")
        return;

    lightgunGamePaths.erase(remove(begin(lightgunGamePaths), end(lightgunGamePaths), gamepath), end(lightgunGamePaths));
    UpdateFile();
}

//********************
// PurgeGamesNotFound
//********************
void LightgunGames::PurgeGamesNotFound() {
    lightgunGamePaths.erase(remove_if(begin(lightgunGamePaths), end(lightgunGamePaths),
                                      [] (const string& gamepath) { return !DirEntry::exists(gamepath); }),
                            end(lightgunGamePaths));
    UpdateFile();
}

//********************
// GetAllLightgunGames
//********************
PsGames LightgunGames::GetAllLightgunGames() {
    PsGames psgames;
    Gui::getInstance()->db->getGames(&psgames);
    //sort(games.begin(), games.end(), sortByTitle);

    PsGames lightgunGames;
    copy_if(begin(psgames), end(psgames), back_inserter(lightgunGames), [&] (PsGamePtr game) { return IsGameALightgunGame(game); });

    shared_ptr<RAIntegrator> integrator = RAIntegrator::getInstance();
    PsGames raGames = integrator->getAllRAGames();
    copy_if(begin(raGames), end(raGames), back_inserter(lightgunGames), [&] (PsGamePtr game) { return IsGameALightgunGame(game); });

    sort(begin(lightgunGames), end(lightgunGames), [] (PsGamePtr game1, PsGamePtr game2) { return game1->title < game2->title; });

    return lightgunGames;
}

