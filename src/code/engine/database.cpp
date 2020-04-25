#include "database.h"
#include "inifile.h"
#include "../util.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include "serialscanner.h"
#include "../DirEntry.h"
#include "../environment.h"

using namespace std;

                                  //*******************************
                                  // DATABASE SQL
                                  //*******************************

//*******************************
// covers?.db
//*******************************

// used by: querySerial
static const char SELECT_META[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS,  COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE SERIAL=? OR SERIAL LIKE ?";

// used by: queryTitle
static const char SELECT_TITLE[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS, COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE TITLE=?";

//*******************************
// RELEASE_YEAR is found in both internal.db and regional.db
// used by Database::updateYear() which is only called by VerMigration::migrate04_05()
// VerMigration appears to be no longer used
//*******************************
// used by: updateYear
static const char UPDATE_YEAR[] = "UPDATE GAME SET RELEASE_YEAR=? WHERE GAME_ID=?";

//*******************************
// regional.db
//*******************************

// used by: updateMemcard
static const char UPDATE_MEMCARD[] = "UPDATE GAME SET MEMCARD=? WHERE GAME_ID=?";

// used by: getGames
static const char GAMES_DATA[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, \
                                  PLAYERS, PATH, SSPATH, MEMCARD, d.BASENAME, HISTORY, LAST_PLAYED, \
                                  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                  GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                  ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

// used by: refreshGameInternal
// used by: refreshGame
static const char GAMES_DATA_SINGLE[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, \
                                        PLAYERS, PATH, SSPATH, MEMCARD, d.BASENAME, HISTORY, LAST_PLAYED, \
                                        COUNT(d.GAME_ID) as NUMD \
                                        FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                        WHERE g.GAME_ID=?  \
                                        GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                        ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

// used by: insertGame
static const char INSERT_GAME[] = "INSERT INTO GAME ([GAME_ID],[GAME_TITLE_STRING],[PUBLISHER_NAME],[RELEASE_YEAR],\
                                   [PLAYERS],[RATING_IMAGE],[GAME_MANUAL_QR_IMAGE],[LINK_GAME_ID],\
                                   [PATH],[SSPATH],[MEMCARD]) \
                                   values (?,?,?,?,?,'CERO_A','QR_Code_GM','',?,?,?)";

// used by: createInitialDatabase
static const char CREATE_GAME_SQL[] = " CREATE TABLE IF NOT EXISTS GAME  \
     ( GAME_ID integer NOT NULL UNIQUE, \
       GAME_TITLE_STRING text, \
       PUBLISHER_NAME text, \
       RELEASE_YEAR integer,\
       PLAYERS integer,     \
       RATING_IMAGE text,   \
       GAME_MANUAL_QR_IMAGE text, \
       LINK_GAME_ID integer,\
       PATH    text null,   \
       SSPATH  text null,   \
       MEMCARD text null,   \
       HISTORY integer,     \
       LAST_PLAYED integer, \
       PRIMARY KEY ( GAME_ID ) )";

// used by: createInitialDatabase
static const char CREATE_DISC_SQL[] = " CREATE TABLE IF NOT EXISTS DISC \
     ( [GAME_ID] integer, \
       [DISC_NUMBER] integer, \
       [BASENAME] text, \
          UNIQUE ([GAME_ID], [DISC_NUMBER]) )";

// used by: createInitialDatabase
static const char CREATE_SUBDIR_ROW_SQL[] = " CREATE TABLE IF NOT EXISTS SUBDIR_ROWS  \
     ( SUBDIR_ROW_INDEX integer NOT NULL UNIQUE, \
       SUBDIR_ROW_NAME text, \
       INDENT_LEVEL integer,    \
       NUM_GAMES integer,    \
       PRIMARY KEY ( SUBDIR_ROW_INDEX ) )";

// used by: createInitialDatabase
static const char CREATE_SUBDIR_GAMES_TO_DISPLAY_ON_ROW_SQL[] = " CREATE TABLE IF NOT EXISTS SUBDIR_GAMES_TO_DISPLAY_ON_ROW  \
     ( SUBDIR_ROW_INDEX integer, GAME_ID integer )";

// used by: subDirRowsTableIsEmpty
static const char IS_SUBDIR_ROWS_TABLE_EMPTY[] = "SELECT count(*) FROM SUBDIR_ROWS";

// used by: insertSubDirRow
static const char INSERT_SUBDIR_ROW[] = "INSERT INTO SUBDIR_ROWS \
        ([SUBDIR_ROW_INDEX],[SUBDIR_ROW_NAME],[INDENT_LEVEL],[NUM_GAMES]) \
        values (?,?,?,?)";

// used by: getGameRowInfos
static const char GET_SUBDIR_ROW[] = "SELECT SUBDIR_ROW_INDEX, SUBDIR_ROW_NAME, INDENT_LEVEL, NUM_GAMES FROM \
        SUBDIR_ROWS ORDER BY SUBDIR_ROW_INDEX";

// used by: insertSubDirGames
static const char INSERT_SUBDIR_GAME[] = "INSERT INTO SUBDIR_GAMES_TO_DISPLAY_ON_ROW \
        ([SUBDIR_ROW_INDEX],[GAME_ID]) values (?,?)";

// used by: getGameRowGameInfos
static const char GET_SUBDIR_GAME[] = "SELECT SUBDIR_ROW_INDEX, GAME_ID FROM \
        SUBDIR_GAMES_TO_DISPLAY_ON_ROW ORDER BY SUBDIR_ROW_INDEX";

// used by: getGameIdsInRow
static const char GET_SUBDIR_GAME_ON_ROW[] = "SELECT GAME_ID FROM \
        SUBDIR_GAMES_TO_DISPLAY_ON_ROW WHERE SUBDIR_ROW_INDEX =?";

// used by: deleteGameIdInAllTables
static const char DELETE_GAME_ID_FROM_DISC[] = "DELETE FROM DISC WHERE GAME_ID =?";
static const char DELETE_GAME_ID_FROM_GAME[] = "DELETE FROM GAME WHERE GAME_ID =?";
static const char DELETE_GAME_ID_FROM_SUBDIR_GAMES_TO_DISPLAY_ON_ROW[] = "DELETE FROM SUBDIR_GAMES_TO_DISPLAY_ON_ROW WHERE GAME_ID =?";

//*******************************
// internal.db
//*******************************

// used by: refreshGameInternal
static const char GAMES_DATA_SINGLE_INTERNAL[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, d.BASENAME,  COUNT(d.GAME_ID) as NUMD, \
                                     FAVORITE, HISTORY, LAST_PLAYED FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                     WHERE g.GAME_ID=?  \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

// used by: getInternalGames
static const char GAMES_DATA_INTERNAL[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, d.BASENAME,  COUNT(d.GAME_ID) as NUMD, \
                                     FAVORITE, HISTORY, LAST_PLAYED FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

// used by: addFavoriteColumn for the internal.db (USB games don't need it as they use the game.ini to flag favorites)
static const char ADD_FAVORITE_COLUMN[] = "ALTER TABLE GAME ADD COLUMN FAVORITE INT DEFAULT 0";

// used by: updateFavorite for the internal.db (USB games don't need it as they use the game.ini to flag favorites)
static const char UPDATE_FAVORITE[] = "UPDATE GAME SET FAVORITE=? WHERE GAME_ID=?";

// used by: addHistoryColumn for the internal.db and regional.db
static const char ADD_HISTORY_COLUMN[] = "ALTER TABLE GAME ADD COLUMN HISTORY INT DEFAULT 0";

// used by: updateHistory for the internal.db and regional.db
static const char UPDATE_HISTORY[] = "UPDATE GAME SET HISTORY=? WHERE GAME_ID=?";

// used by: addLastPlayedColumn for the internal.db and regional.db
static const char ADD_LAST_PLAYED_COLUMN[] = "ALTER TABLE GAME ADD COLUMN LAST_PLAYED INT DEFAULT 0";

// used by: updateDatePlayed for the internal.db and regional.db
static const char UPDATE_LAST_PLAYED[] = "UPDATE GAME SET LAST_PLAYED=? WHERE GAME_ID=?";

//*******************************
// ????.db
//*******************************

// used by: updateTitle
static const char UPDATE_TITLE[] = "UPDATE GAME SET GAME_TITLE_STRING=? WHERE GAME_ID=?";

// used by: getNumGames
static const char NUM_GAMES[] = "SELECT COUNT(*) as ctn FROM GAME";

// used by: createInitialDatabase
static const char CREATE_LANGUAGE_SPECIFIC_SQL[] = "CREATE TABLE IF NOT EXISTS LANGUAGE_SPECIFIC \
      ( [DEFAULT_VALUE] text, \
        [LANGUAGE_ID] integer, \
        [VALUE] text, \
           UNIQUE ([DEFAULT_VALUE], [LANGUAGE_ID]) )";

// used by: beginTransaction
static const char BEGIN_TRANSACTION[] = "BEGIN TRANSACTION";
// used by: commit
static const char COMMIT[] = "COMMIT";

// used by: truncate
static const char DELETE_GAME_DATA[] = "DELETE FROM GAME";
static const char DELETE_DISC_DATA[] = "DELETE FROM DISC";
static const char DELETE_LANGUAGE_DATA[] = "DELETE FROM LANGUAGE_SPECIFIC";
static const char DELETE_SUBDIR_ROW_DATA[] = "DELETE FROM SUBDIR_ROWS";
static const char DELETE_SUBDIR_GAME_DATA[] = "DELETE FROM SUBDIR_GAMES_TO_DISPLAY_ON_ROW";

// used by: insertDisc
static const char INSERT_DISC[] = "INSERT INTO DISC ([GAME_ID],[DISC_NUMBER],[BASENAME]) \
                values (?,?,?)";

//*******************************
// DATABASE code
//*******************************

//*******************************
// Database::getNumGames
//*******************************
int Database::getNumGames() {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, NUM_GAMES, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const int number = sqlite3_column_int(res, 0);
            sqlite3_finalize(res);
            return number;
        }
    } else {
        cerr << "Failed: db:: getNumGames" << endl;
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return 0;
    }
    sqlite3_finalize(res);
    return 0;
}

//*******************************
// Database::updateYear
// called by VerMigration::migrate04_05()
//*******************************
bool Database::updateYear(int id, int year) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_YEAR, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: updateYear, " << id << ", " << year << endl;
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    sqlite3_bind_int(res, 1, year);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::updateMemcard
//*******************************
bool Database::updateMemcard(int id, string memcard) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_MEMCARD, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db::updateMemcard, " << id << ", " << memcard << endl;
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    sqlite3_bind_text(res, 1, memcard.c_str(), -1, nullptr);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::updateTitle
//*******************************
bool Database::updateTitle(int id, string title) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_TITLE, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: updateTitle, " << id << ", " << title << endl;
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    sqlite3_bind_text(res, 1, title.c_str(), -1, nullptr);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::updateFavorite
//*******************************
bool Database::updateFavorite(int id, int favorite) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_FAVORITE, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: updateFavorite, " << id << ", " << favorite << endl;
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    sqlite3_bind_int(res, 1, favorite);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::updateHistory
// 0 = not in history, 1-100 history from latest game played to oldest
//*******************************
bool Database::updateHistory(int id, int rank) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_HISTORY, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: updateHistory, " << id << ", " << rank << endl;
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    sqlite3_bind_int(res, 1, rank);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::updateDatePlayed
// seconds since 1970
//*******************************
bool Database::updateDatePlayed(int id, int date_in_seconds) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_LAST_PLAYED, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: updateDatePlayed, " << id << ", " << date_in_seconds << endl;
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    sqlite3_bind_int(res, 1, date_in_seconds);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::queryTitle
//*******************************
bool Database::queryTitle(string title, Metadata *md) {

    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, SELECT_TITLE, -1, &res, nullptr);
    if (rc == SQLITE_OK) {

        sqlite3_bind_text(res, 1, title.c_str(), -1, nullptr);
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            const int year = sqlite3_column_int(res, 3);
            const int players = sqlite3_column_int(res, 4);
            const void *bytes = sqlite3_column_blob(res, 5);
            size_t size = sqlite3_column_bytes(res, 5);
            if (size != 0) {
                md->dataSize = size;
                md->bytes = new char[size];
                memcpy(md->bytes, bytes, size);
            }
            md->title = string(reinterpret_cast<const char *>(title));
            md->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(md->publisher);
            md->year = year;
            md->players = players;
            md->valid = true;
            return true;

        }
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return false;
}

//*******************************
// Database::getInternalGames
//*******************************
bool Database::getInternalGames(PsGames *result) {
    result->clear();
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA_INTERNAL, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *base = sqlite3_column_text(res, 5);
            int discs = sqlite3_column_int(res, 6);
            int fav = sqlite3_column_int(res, 7);
            int history = sqlite3_column_int(res, 8);
            int last_played = sqlite3_column_int(res, 9);

            PsGamePtr psGame{new PsGame};
            psGame->gameId = id;
            psGame->title = string(reinterpret_cast<const char *>(title));
            psGame->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(psGame->publisher);
            psGame->year = year;
            psGame->players = players;
            psGame->folder = "/gaadata/" + to_string(id) + "/";
            psGame->ssFolder = Env::getPathToSaveStatesDir() + sep + to_string(id) + "/";
            psGame->base = string(reinterpret_cast<const char *>(base));
            psGame->serial = psGame->base;
            psGame->region = SerialScanner::serialToRegion(psGame->serial);
            psGame->memcard = "SONY";
            psGame->internal = true;
            psGame->cds = discs;
            psGame->favorite = (fav != 0);
            psGame->history = history;
            psGame->last_played = last_played;
            result->push_back(psGame);
            //cout << "getInternalGames: " << game->serial << ", " << game->title << endl;
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::refreshGameInternal
//*******************************
bool Database::refreshGameInternal(PsGamePtr &psGame) {

    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA_SINGLE_INTERNAL, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, psGame->gameId);
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *base = sqlite3_column_text(res, 5);
            int discs = sqlite3_column_int(res, 6);
            int fav = sqlite3_column_int(res, 7);
            int history = sqlite3_column_int(res, 8);
            int last_played = sqlite3_column_int(res, 9);

            psGame->gameId = id;
            psGame->title = string(reinterpret_cast<const char *>(title));
            psGame->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(psGame->publisher);
            psGame->year = year;
            psGame->players = players;
            psGame->folder = "/gaadata/" + to_string(id) + "/";
            psGame->ssFolder = Env::getPathToSaveStatesDir() + sep + to_string(id) + "/";
            psGame->base = string(reinterpret_cast<const char *>(base));
            psGame->serial = psGame->base;
            psGame->region = SerialScanner::serialToRegion(psGame->serial);
            psGame->memcard = "SONY";
            psGame->internal = true;
            psGame->cds = discs;
            psGame->favorite = (fav != 0);
            psGame->history = history;
            psGame->last_played = last_played;

            string gameIniPath = psGame->folder + sep + GAME_INI;
            if (DirEntry::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                psGame->locked =  !(ini.values["automation"]=="1");
                psGame->hd =       (ini.values["highres"]=="1");
                psGame->favorite = (ini.values["favorite"] == "1");
            }
        }
    } else {

        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::refreshGame
//*******************************
bool Database::refreshGame(PsGamePtr &game) {

    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA_SINGLE, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, game->gameId);
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *path = sqlite3_column_text(res, 5);
            const unsigned char *sspath = sqlite3_column_text(res, 6);
            const unsigned char *memcard = sqlite3_column_text(res, 7);
            const unsigned char *base = sqlite3_column_text(res, 8);
            int history = sqlite3_column_int(res, 9);
            int last_played = sqlite3_column_int(res, 10);
            int discs = sqlite3_column_int(res, 11);

            game->gameId = id;
            game->title = string(reinterpret_cast<const char *>(title));
            game->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(game->publisher);
            game->year = year;
            game->players = players;
            game->folder = string(reinterpret_cast<const char *>(path));
            game->ssFolder = string(reinterpret_cast<const char *>(sspath));
            game->base = string(reinterpret_cast<const char *>(base));
            game->history = history;
            game->last_played = last_played;
            game->memcard = string(reinterpret_cast<const char *>(memcard));
            game->cds = discs;

            string gameIniPath = game->folder + sep + GAME_INI;
            if (DirEntry::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                game->locked =  !(ini.values["automation"]=="1");
                game->hd =       (ini.values["highres"]=="1");
                game->favorite = (ini.values["favorite"] == "1");
            }
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getGames
//*******************************
bool Database::getGames(PsGames *result) {
    result->clear();
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *path = sqlite3_column_text(res, 5);
            const unsigned char *sspath = sqlite3_column_text(res, 6);
            const unsigned char *memcard = sqlite3_column_text(res, 7);
            const unsigned char *base = sqlite3_column_text(res, 8);
            int history = sqlite3_column_int(res, 9);
            int last_played = sqlite3_column_int(res, 10);
            int discs = sqlite3_column_int(res, 11);

            PsGamePtr game{new PsGame};
            game->gameId = id;
            game->title = string(reinterpret_cast<const char *>(title));
            game->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(game->publisher);
            game->year = year;
            game->players = players;
            game->folder = string(reinterpret_cast<const char *>(path));
            game->ssFolder = string(reinterpret_cast<const char *>(sspath));
            game->base = string(reinterpret_cast<const char *>(base));
            game->history = history;
            game->last_played = last_played;
            game->memcard = string(reinterpret_cast<const char *>(memcard));
            game->cds = discs;
            //cout << "getGames: " << game->serial << ", " << game->title << endl;

            string gameIniPath = game->folder + sep + GAME_INI;
            if (DirEntry::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                game->locked =  !(ini.values["automation"]=="1");
                game->hd =       (ini.values["highres"]=="1");
                game->favorite = (ini.values["favorite"] == "1");
            }
            result->push_back(game);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getGameRowInfos
//*******************************
bool Database::getGameRowInfos(GameRowInfos *gameRowInfos) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GET_SUBDIR_ROW, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            SubDirRowInfo subDirRowInfo;
            subDirRowInfo.subDirRowIndex = sqlite3_column_int(res, 0);
            const unsigned char *name = sqlite3_column_text(res, 1);
            subDirRowInfo.rowName = (const char*) name;
            subDirRowInfo.indentLevel = sqlite3_column_int(res, 2);
            subDirRowInfo.numGames = sqlite3_column_int(res, 3);

            cout << "SubDirRowInfo: " << string(subDirRowInfo.indentLevel * 2, ' ') << subDirRowInfo.rowName
                    << ", index: " << subDirRowInfo.subDirRowIndex
                    << ", indent: " << subDirRowInfo.indentLevel
                    << ", numGames: " << subDirRowInfo.numGames << endl;

            gameRowInfos->emplace_back(subDirRowInfo);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getSubDirGames
//*******************************
bool Database::getGameRowGameInfos(GameRowGames *gameRowGames) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GET_SUBDIR_GAME, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            GameRowGame gameRowGame;
            gameRowGame.rowIndex = sqlite3_column_int(res, 0);
            gameRowGame.gameId = sqlite3_column_int(res, 1);

            cout << "GameRowGame: " << gameRowGame.rowIndex << ", " << gameRowGame.gameId << endl;

            gameRowGames->emplace_back(gameRowGame);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getSubDirGames
//*******************************
bool Database::getGameIdsInRow(vector<int> *gameIdsInRow, int row) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GET_SUBDIR_GAME_ON_ROW, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, row);
        while (sqlite3_step(res) == SQLITE_ROW) {
            int gameId = sqlite3_column_int(res, 0);
            //cout << "GameId in Row: " << row << ", " << gameId << endl;
            gameIdsInRow->emplace_back(gameId);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::querySerial
//*******************************
bool Database::querySerial(string serial, Metadata *md) {
    string serialLike = serial + "-%";
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, SELECT_META, -1, &res, nullptr);
    if (rc == SQLITE_OK) {

        sqlite3_bind_text(res, 1, serial.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 2, serialLike.c_str(), -1, nullptr);
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            const int year = sqlite3_column_int(res, 3);
            const int players = sqlite3_column_int(res, 4);
            const void *bytes = sqlite3_column_blob(res, 5);
            size_t size = sqlite3_column_bytes(res, 5);
            if (size != 0) {
                md->dataSize = size;
                md->bytes = new char[size];
                memcpy(md->bytes, bytes, size);
            }

            const unsigned char *path = sqlite3_column_text(res, 6);
            md->title = string(reinterpret_cast<const char *>(title));
            md->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(md->publisher);
            md->year = year;
            md->serial = serial;
            md->region = SerialScanner::serialToRegion(md->serial);
            md->players = players;
            md->valid = true;
            //cout << "querySerial: " << "serial " << serial << ", " << md->title << endl;

            return true;

        }
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return false;
}

//*******************************
// Database::insertDisc
//*******************************
bool Database::insertDisc(int id, int discNum, string discName) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, INSERT_DISC, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, id);
        sqlite3_bind_int(res, 2, discNum);
        sqlite3_bind_text(res, 3, discName.c_str(), -1, nullptr);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::insertGame
//*******************************
bool Database::insertGame(int id, string title, string publisher, int players, int year, string path, string sspath,
                          string memcard) {
    sqlite3_stmt *res = nullptr;
    Util::cleanPublisherString(publisher);
    int rc = sqlite3_prepare_v2(db, INSERT_GAME, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, id);
        sqlite3_bind_text(res, 2, title.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 3, publisher.c_str(), -1, nullptr);
        sqlite3_bind_int(res, 4, year);
        sqlite3_bind_int(res, 5, players);
        sqlite3_bind_text(res, 6, path.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 7, sspath.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 8, memcard.c_str(), -1, nullptr);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::subDirRowsTableIsEmpty
// returns true if no rows in table or failure
// *******************************
bool Database::subDirRowsTableIsEmpty() {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, IS_SUBDIR_ROWS_TABLE_EMPTY, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const int number = sqlite3_column_int(res, 0);
            sqlite3_finalize(res);
            return (number == 0);   // true if no rows in table
        }
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return true;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::insertSubDirRow
//*******************************
bool Database::insertSubDirRow(int rowIndex, string rowName, int indentLevel, int numGames) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, INSERT_SUBDIR_ROW, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, rowIndex);
        sqlite3_bind_text(res, 2, rowName.c_str(), -1, nullptr);
        sqlite3_bind_int(res, 3, indentLevel);
        sqlite3_bind_int(res, 4, numGames);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);

    return true;
}

//*******************************
// Database::insertSubDirGames
//*******************************
bool Database::insertSubDirGames(int rowIndex, int gameId) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, INSERT_SUBDIR_GAME, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, rowIndex);
        sqlite3_bind_int(res, 2, gameId);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);

    return true;
}

//*******************************
// Database::executeCreateStatement
//*******************************
bool Database::executeCreateStatement(char *sql, string name) {
    char *errorReport = nullptr;
    cout << "Creating " << name << " (if not exists)" << endl;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorReport);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: executeCreateStatement, " << sql << ", " << name << endl;
        cerr << "Failed to create " << name << "  table/column  " << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    return true;
}

//*******************************
// Database::executeStatement
//*******************************
bool Database::executeStatement(char *sql, string outMsg, string errorMsg) {
    char *errorReport = nullptr;
    cout << outMsg << endl;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorReport);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: executeStatement, " << sql << ", " << outMsg<< ", " << errorMsg << endl;
        cerr << errorMsg << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    return true;
}

//*******************************
// Database::connect
//*******************************
bool Database::connect(string fileName) {
    int rc = sqlite3_open(fileName.c_str(), &db);
    cout << "Connected to DB " << fileName << endl;
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: connect, " << fileName << endl;
        cout << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    return true;
}

//*******************************
// Database::disconnect
//*******************************
void Database::disconnect() {
    if (db != nullptr) {
        cout << "Disconnecting DBs" << endl;
        sqlite3_db_cacheflush(db);
        sqlite3_close(db);
        db = nullptr;
    }
}

//*******************************
// Database::beginTransaction
//*******************************
bool Database::beginTransaction() {
    executeStatement((char *) BEGIN_TRANSACTION, "Begin Transaction", "Error beginning  transaction");
    return true;
}

//*******************************
// Database::commit
//*******************************
bool Database::commit() {
    executeStatement((char *) COMMIT, "Commit", "Error on commit");
    return true;
}

//*******************************
// Database::truncate
//*******************************
bool Database::truncate() {
    executeStatement((char *) DELETE_GAME_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_DISC_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_LANGUAGE_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_SUBDIR_ROW_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_SUBDIR_GAME_DATA, "Truncating all data", "Error truncating data");
    return true;
}

//*******************************
// Database::createInitialDatabase
//*******************************
bool Database::createInitialDatabase() {
    if (!executeCreateStatement((char *) CREATE_GAME_SQL, "GAME")) return false;
    executeCreateStatement((char*) ADD_HISTORY_COLUMN, "History column" ); // add column to existing table
    executeCreateStatement((char*) ADD_LAST_PLAYED_COLUMN, "Last_Played column" ); // add column to existing table
    if (!executeCreateStatement((char *) CREATE_DISC_SQL, "DISC")) return false;
    if (!executeCreateStatement((char *) CREATE_LANGUAGE_SPECIFIC_SQL, "LANGUAGE_SPECIFIC")) return false;
    if (!executeCreateStatement((char *) CREATE_SUBDIR_ROW_SQL, "SUBDIR_ROWS")) return false;
    if (!executeCreateStatement((char *) CREATE_SUBDIR_GAMES_TO_DISPLAY_ON_ROW_SQL, "SUBDIR_GAMES_TO_DISPLAY_ON_ROW")) return false;

    return true;
}

//*******************************
// Database::addFavoriteColumn
//*******************************
void Database::addFavoriteColumn() {
    executeCreateStatement((char*) ADD_FAVORITE_COLUMN, "Favorite column" );
}

//*******************************
// Database::addHistoryColumn
//*******************************
void Database::addHistoryColumn() {
    executeCreateStatement((char*) ADD_HISTORY_COLUMN, "History column" );
}

//*******************************
// Database::addLastPlayedColumn
//*******************************
void Database::addLastPlayedColumn() {
    executeCreateStatement((char*) ADD_LAST_PLAYED_COLUMN, "Last_Played column" );
}

//*******************************
// Database::deleteGameIdFromOneTable
//*******************************
bool Database::deleteGameIdFromOneTable(int id, const string& cmd_str) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, cmd_str.c_str(), -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed: db:: delete game_id from table, " << id << ", " << cmd_str << endl;
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        return false;
    }
    sqlite3_bind_int(res, 1, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}


//*******************************
// Database::deleteGameIdFromAllTables
//*******************************
bool Database::deleteGameIdFromAllTables(int id) {
    beginTransaction(); // all the statements must succeed or the DB won't be modified

    bool success = true;
    success = deleteGameIdFromOneTable(id, DELETE_GAME_ID_FROM_DISC);
    if (success)
        success = deleteGameIdFromOneTable(id, DELETE_GAME_ID_FROM_GAME);
    if (success)
        success = deleteGameIdFromOneTable(id, DELETE_GAME_ID_FROM_SUBDIR_GAMES_TO_DISPLAY_ON_ROW);

    if (success)
        commit();

    return success;
}

#if 0
// from: https://forums.coronalabs.com/topic/29425-check-if-column-exists-in-sqlite-database/
function dbColumnExists(dbase, tbl, col)
  local sql = "select * from "..tbl.." limit 1;"
  local stmt = dbase:prepare(sql)
  local tb = stmt:get_names()
  local found = false
  for v = 1, stmt:columns() do
        print(tb[v]);
    if tb[v] == col then
        found = true;
        print("FOUND "..tb[v]);
    end
  end
  return found
end

// from: https://stackoverflow.com/questions/18920136/check-if-a-column-exists-in-sqlite
// link has multiple code snippets
// This method will check if column exists in your table
public boolean isFieldExist(String tableName, String fieldName)
{
     boolean isExist = false;
     SQLiteDatabase db = this.getWritableDatabase();
     Cursor res = db.rawQuery("PRAGMA table_info("+tableName+")",null);
    res.moveToFirst();
    do {
        String currentColumn = res.getString(1);
        if (currentColumn.equals(fieldName)) {
            isExist = true;
        }
    } while (res.moveToNext());
     return isExist;
}
#endif
