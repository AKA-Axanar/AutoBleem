/* 
 * File:   database.h
 * Author: screemer
 *
 * Created on 11 dec 2018, 21:49
 */


#ifndef DATABASE_H
#define DATABASE_H

#include "../main.h"
#include "metadata.h"
#include "../sqlite3.h"

using namespace std;


class Database {
public:

    bool connect(string fileName);
    void disconnect();
    bool createInitialDatabase();
    bool insertGame(int id, string title, string publisher, int players, int year);
    bool insertDisc(int id, int discNum, string discName);
    bool querySerial(string serial, Metadata *md);
    bool queryTitle(string title, Metadata *md);
    int getNumGames();
    bool updateYear(int id, int year);
private:
    sqlite3 *db;
    bool executeCreateStatement(char *sql, string tableName);
    bool executeStatement(char *sql, string outMsg, string errorMsg);
};






#endif /* DATABASE_H */

