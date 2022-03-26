//
// Created by steve on 3/24/22.
//
#pragma once

#include <string>
#include <vector>
#include "launcher/ps_game.h"

struct LightgunGames {
    static std::string filename;
    static std::vector<std::string> lightgunGamePaths;

    LightgunGames();
    void UpdateFile();

    bool IsGameALightgunGame(PsGamePtr game);
    bool IsGameALightgunGame(const std::string& gamepath);

    void AddGame(PsGamePtr game);
    void AddGame(const std::string& gamepath);

    void RemoveGame(PsGamePtr game);
    void RemoveGame(const std::string& gamepath);

    void PurgeGamesNotFound();
};
