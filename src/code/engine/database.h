/* 
 * File:   database.h
 * Author: screemer
 *
 * Created on 11 dec 2018, 21:49
 */
#pragma once

#include "metadata.h"
#include <sqlite3ab.h>
#include "../launcher/ps_game.h"

//******************
// GameRowInfo
//******************
struct SubDirRowInfo {
    int subDirRowIndex = 0;
    std::string rowName = "";
    int indentLevel = 0;
    int numGames = 0;
};

//******************
// GameRowInfos
//******************
using GameRowInfos = std::vector<SubDirRowInfo>;

//******************
// GameRowGame
//******************
struct GameRowGame {
    int rowIndex = 0;
    int gameId = 0;
};

//******************
// GameRowGames
//******************
using GameRowGames = std::vector<GameRowGame>;

//******************
// Database
//******************
class Database {
public:

    bool connect(std::string fileName);
    void disconnect();
    bool createInitialDatabase();
    void addFavoriteColumn();
    void addHistoryColumn();
    void addDatePlayedColumn();
    bool truncate();

    bool beginTransaction();
    bool commit();

    bool insertGame(int id, std::string title, std::string publisher, int players, int year, std::string path, std::string sspath,
                    std::string memcard);
    bool insertDisc(int id, int discNum, std::string discName);

    bool subDirRowsTableIsEmpty();
    bool insertSubDirRow(int rowIndex, std::string rowName, int indentLevel, int numGames);
    bool insertSubDirGames(int rowIndex, int gameId);

    bool querySerial(std::string serial, Metadata *md);
    bool queryTitle(std::string title, Metadata *md);
    int getNumGames();
    bool updateYear(int id, int year);
    bool updateMemcard(int id, std::string memcard);

    bool getGames(PsGames *result);
    bool getInternalGames(PsGames *result);
    bool getGameRowInfos(GameRowInfos *gameRowInfos);
    bool getGameRowGameInfos(GameRowGames *gameRowGames);
    bool getGameIdsInRow(std::vector<int> *gameIdsInRow, int row);

    bool updateTitle(int id, std::string title);
    bool updateFavorite(int id, int fav);
    bool updateHistory(int id, int rank);   // 0 = not in history, 1-100 history from latest game played to oldest
    bool updateDatePlayed(int id, int date_in_seconds);   // seconds since 1970
    bool refreshGame(PsGamePtr & game);
    bool refreshGameInternal(PsGamePtr & game);

    bool deleteGameIdFromOneTable(int id, const std::string& cmd_str);
    bool deleteGameIdFromAllTables(int id);
private:
    sqlite3 *db;
    bool executeCreateStatement(char *sql, std::string name);
    bool executeStatement(char *sql, std::string outMsg, std::string errorMsg);
};
