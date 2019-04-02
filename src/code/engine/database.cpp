#include "database.h"
#include "inifile.h"


using namespace std;


static const char SELECT_META[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS,  COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE SERIAL=? OR SERIAL LIKE ?";

static const char SELECT_TITLE[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS, COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE TITLE=?";

static const char UPDATE_YEAR[] = "UPDATE GAME SET RELEASE_YEAR=? WHERE GAME_ID=?";

static const char UPDATE_MEMCARD[] = "UPDATE GAME SET MEMCARD=? WHERE GAME_ID=?";
static const char UPDATE_TITLE[] = "UPDATE GAME SET GAME_TITLE_STRING=? WHERE GAME_ID=?";

static const char NUM_GAMES[] = "SELECT COUNT(*) as ctn FROM GAME";

static const char GAMES_DATA[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, PATH, SSPATH, MEMCARD, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

static const char GAMES_DATA_SINGLE[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, PATH, SSPATH, MEMCARD, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                    WHERE g.GAME_ID=?  \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

static const char GAMES_DATA_SINGLE_INTERNAL[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                    WHERE g.GAME_ID=?  \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

static const char GAMES_DATA_INTERNAL[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

static const char UPDATE_GAME_DB[] = "ALTER TABLE GAME ADD COLUMN FAV INT DEFAULT 0";

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
         PRIMARY KEY ( GAME_ID ) )";
static const char CREATE_DISC_SQL[] = " CREATE TABLE IF NOT EXISTS DISC \
     ( [GAME_ID] integer, \
       [DISC_NUMBER] integer, \
       [BASENAME] text, \
          UNIQUE ([GAME_ID], [DISC_NUMBER]) )";

static const char CREATE_LANGUAGE_SPECIFIC_SQL[] = "CREATE TABLE IF NOT EXISTS LANGUAGE_SPECIFIC \
      ( [DEFAULT_VALUE] text, \
        [LANGUAGE_ID] integer, \
        [VALUE] text, \
           UNIQUE ([DEFAULT_VALUE], [LANGUAGE_ID]) )";
static const char DELETE_DATA[] = "DELETE FROM GAME";
static const char DELETE_DATA2[] = "DELETE FROM DISC";
static const char DELETE_DATA3[] = "DELETE FROM LANGUAGE_SPECIFIC";


static const char INSERT_GAME[] = "INSERT INTO GAME ([GAME_ID],[GAME_TITLE_STRING],[PUBLISHER_NAME],[RELEASE_YEAR],[PLAYERS],[RATING_IMAGE],[GAME_MANUAL_QR_IMAGE],[LINK_GAME_ID],\
                [PATH],[SSPATH],[MEMCARD]) \
                values (?,?,?,?,?,'CERO_A','QR_Code_GM','',?,?,?)";

static const char INSERT_DISC[] = "INSERT INTO DISC ([GAME_ID],[DISC_NUMBER],[BASENAME]) \
                values (?,?,?)";

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
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return 0;
    }
    sqlite3_finalize(res);
    return 0;
}

bool Database::updateYear(int id, int year) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_YEAR, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_int(res, 1, year);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

bool Database::updateMemcard(int id, string memcard) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_MEMCARD, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_text(res, 1, memcard.c_str(), -1, nullptr);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

bool Database::updateTitle(int id, string title) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_TITLE, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_text(res, 1, title.c_str(), -1, nullptr);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

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
            md->title = std::string(reinterpret_cast<const char *>(title));
            md->publisher = std::string(reinterpret_cast<const char *>(publisher));
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

bool Database::getInternalGames(vector<PsGame *> *result) {
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

            PsGame *game = new PsGame();
            game->gameId = id;
            game->title = std::string(reinterpret_cast<const char *>(title));
            game->publisher = std::string(reinterpret_cast<const char *>(publisher));
            game->year = year;
            game->players = players;
            game->folder = "/gaadata/" + to_string(id) + "/";
            game->ssFolder = "/media/Games/!SaveStates/" + to_string(id) + "/";
            game->base = std::string(reinterpret_cast<const char *>(base));
            game->memcard = "SONY";
            game->internal = true;
            game->cds = discs;
            result->push_back(game);
        }
    } else {


        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

bool Database::refreshGameInternal(PsGame  *game) {

    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA_SINGLE_INTERNAL, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, game->gameId);
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *base = sqlite3_column_text(res, 5);
            int discs = sqlite3_column_int(res, 6);


            game->gameId = id;
            game->title = std::string(reinterpret_cast<const char *>(title));
            game->publisher = std::string(reinterpret_cast<const char *>(publisher));
            game->year = year;
            game->players = players;
            game->folder = "/gaadata/" + to_string(id) + "/";
            game->ssFolder = "/media/Games/!SaveStates/" + to_string(id) + "/";
            game->base = std::string(reinterpret_cast<const char *>(base));
            game->memcard = "SONY";
            game->internal = true;
            game->cds = discs;


            string gameIniPath = game->folder + "/Game.ini";
            if (Util::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                if (ini.values["automation"]=="1")
                {
                    game->locked = false;
                } else
                {
                    game->locked = true;
                }
                if (ini.values["highres"]=="1")
                {
                    game->hd=true;
                } else
                {
                    game->hd=false;
                }
            }

        }
    } else {


        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

bool Database::refreshGame(PsGame  *game) {

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
            int discs = sqlite3_column_int(res, 9);


            game->gameId = id;
            game->title = std::string(reinterpret_cast<const char *>(title));
            game->publisher = std::string(reinterpret_cast<const char *>(publisher));
            game->year = year;
            game->players = players;
            game->folder = std::string(reinterpret_cast<const char *>(path));
            game->ssFolder = std::string(reinterpret_cast<const char *>(sspath));
            game->base = std::string(reinterpret_cast<const char *>(base));
            game->memcard = std::string(reinterpret_cast<const char *>(memcard));
            game->cds = discs;

            string gameIniPath = game->folder + "/Game.ini";
            if (Util::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                if (ini.values["automation"]=="1")
                {
                    game->locked = false;
                } else
                {
                    game->locked = true;
                }
                if (ini.values["highres"]=="1")
                {
                    game->hd=true;
                } else
                {
                    game->hd=false;
                }
            }

        }
    } else {


        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

bool Database::getGames(vector<PsGame *> *result) {
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
            int discs = sqlite3_column_int(res, 9);

            PsGame *game = new PsGame();
            game->gameId = id;
            game->title = std::string(reinterpret_cast<const char *>(title));
            game->publisher = std::string(reinterpret_cast<const char *>(publisher));
            game->year = year;
            game->players = players;
            game->folder = std::string(reinterpret_cast<const char *>(path));
            game->ssFolder = std::string(reinterpret_cast<const char *>(sspath));
            game->base = std::string(reinterpret_cast<const char *>(base));
            game->memcard = std::string(reinterpret_cast<const char *>(memcard));
            game->cds = discs;

            string gameIniPath = game->folder + "/Game.ini";
            if (Util::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                if (ini.values["automation"]=="1")
                {
                    game->locked = false;
                } else
                {
                    game->locked = true;
                }
                if (ini.values["highres"]=="1")
                {
                    game->hd=true;
                } else
                {
                    game->hd=false;
                }
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
            md->title = std::string(reinterpret_cast<const char *>(title));
            md->publisher = std::string(reinterpret_cast<const char *>(publisher));
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

bool Database::insertGame(int id, string title, string publisher, int players, int year, string path, string sspath,
                          string memcard) {
    sqlite3_stmt *res = nullptr;
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

bool Database::executeCreateStatement(char *sql, string tableName) {
    char *errorReport = nullptr;
    cout << "Creating " << tableName << " table (if not exists)" << endl;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorReport);
    if (rc != SQLITE_OK) {
        cerr << "Failed to create " << tableName << "  table  " << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    return true;
}

bool Database::executeStatement(char *sql, string outMsg, string errorMsg) {
    char *errorReport = nullptr;
    cout << outMsg << endl;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorReport);
    if (rc != SQLITE_OK) {
        cerr << errorMsg << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    return true;
}

bool Database::connect(string fileName) {
    int rc = sqlite3_open(fileName.c_str(), &db);
    cout << "Connected to DB" << fileName << endl;
    if (rc != SQLITE_OK) {
        cout << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    return true;
}

void Database::disconnect() {
    if (db != nullptr) {
        cout << "Disconnecting DBs" << endl;
        sqlite3_db_cacheflush(db);
        sqlite3_close(db);
        db = nullptr;
    }
}
bool Database::truncate()
{
    executeStatement((char *) DELETE_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_DATA2, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_DATA3, "Truncating all data", "Error truncating data");
    return true;
}
bool Database::createInitialDatabase() {
    if (!executeCreateStatement((char *) CREATE_GAME_SQL, "GAME")) return false;
    if (!executeCreateStatement((char *) CREATE_DISC_SQL, "DISC")) return false;
    if (!executeCreateStatement((char *) CREATE_LANGUAGE_SPECIFIC_SQL, "LANGUAGE_SPECIFIC")) return false;
    return true;


}

void Database::createFavColumn()
{
 //   executeCreateStatement((char*) UPDATE_GAME_DB, "FAV column" );
}

