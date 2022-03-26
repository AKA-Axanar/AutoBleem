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

void LightgunGames::UpdateFile() {
    Util::WriteStringsToTextFile(lightgunGamePaths, filename, true);
}

bool LightgunGames::IsGameALightgunGame(PsGamePtr game) {
    if (game->foreign)
        return IsGameALightgunGame(game->image_path);   // Retroarch roms game
    else
        return IsGameALightgunGame(game->folder);       // /Games PS1 game
}

bool LightgunGames::IsGameALightgunGame(const std::string& gamepath) {
    auto it = find(begin(lightgunGamePaths), end(lightgunGamePaths), gamepath);
    return it != end(lightgunGamePaths);
}

void LightgunGames::AddGame(PsGamePtr game) {
    if (game->foreign)
        AddGame(game->image_path);   // Retroarch roms game
    else
        AddGame(game->folder);       // /Games PS1 game
}

void LightgunGames::AddGame(const std::string& gamepath) {
    if (!IsGameALightgunGame(gamepath)) {
        lightgunGamePaths.emplace_back(gamepath);
        UpdateFile();
    }
}

void LightgunGames::RemoveGame(PsGamePtr game) {
    if (game->foreign)
        RemoveGame(game->image_path);   // Retroarch roms game
    else
        RemoveGame(game->folder);       // /Games PS1 game
}

void LightgunGames::RemoveGame(const std::string& gamepath) {
    lightgunGamePaths.erase(remove(begin(lightgunGamePaths), end(lightgunGamePaths), gamepath), end(lightgunGamePaths));
    UpdateFile();
}

void LightgunGames::PurgeGamesNotFound() {
    lightgunGamePaths.erase(remove_if(begin(lightgunGamePaths), end(lightgunGamePaths),
                                      [] (const string& gamepath) { return !DirEntry::exists(gamepath); }),
                            end(lightgunGamePaths));
    UpdateFile();
}
