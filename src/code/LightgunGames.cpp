//
// Created by steve on 3/24/22.
//

#include "LightgunGames.h"
#include "util.h"
#include "environment.h"
#include <algorithm>
#include "DirEntry.h"

using namespace std;

std::string LightgunGames::filename;
std::vector<std::string> LightgunGames::lightgunGamePaths;

LightgunGames::LightgunGames() {
    filename = Environment::getWorkingPath() + sep + string("lightguns.txt");
    lightgunGamePaths = Util::ReadTextFileAsAStringArray(filename, true);

    // there shouldn't be any blank lines unless someone hand edited it.  but just to be sure.
    lightgunGamePaths.erase(remove_if(begin(lightgunGamePaths), end(lightgunGamePaths), [] (const string& str) { return str == ""; }), end(lightgunGamePaths));
}

string LightgunGames::PathForLightgunFile(PsGamePtr game) {
    if (game == nullptr)
        return "";

    if (game->foreign)
        return game->image_path;   // Retroarch roms game
    else
        return game->folder;       // /Games PS1 game
   
}

void LightgunGames::UpdateFile() {
    Util::WriteStringsToTextFile(lightgunGamePaths, filename, true);
}

bool LightgunGames::IsGameALightgunGame(PsGamePtr game) {
    return IsGameALightgunGame(PathForLightgunFile(game));
}

bool LightgunGames::IsGameALightgunGame(const std::string& gamepath) {
    if (gamepath == "")
        return false;

    auto it = find(begin(lightgunGamePaths), end(lightgunGamePaths), gamepath);
    return it != end(lightgunGamePaths);
}

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

void LightgunGames::PurgeGamesNotFound() {
    lightgunGamePaths.erase(remove_if(begin(lightgunGamePaths), end(lightgunGamePaths),
                                      [] (const string& gamepath) { return !DirEntry::exists(gamepath); }),
                            end(lightgunGamePaths));
    UpdateFile();
}
