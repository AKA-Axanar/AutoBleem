#include "lightgunSerials.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "util.h"
#include "environment.h"
#include "DirEntry.h"
#include <regex>

using namespace std;

static vector<string> lightgunSerials;

void loadLightgunSerialsFile() {
    static bool firstTime = true;
    if (firstTime) {
        ifstream file;
        file.open(Env::getWorkingPath() + sep + "lightgunSerials.txt");
        if (file.good()) {
            string line;
            while (Util::getlineRemoveCR(file, line)) {
                Util::removeComment(line);   // remove '#' to end of line
                trim(line); // remove leading a trailing whitespace
                if (line.length() == 0) continue;    // blank line

                regex expr("^[A-Za-z]{4}-[0-9]{5}");
                smatch match;
                if (regex_search(line, match, expr)) {
                    lightgunSerials.emplace_back(match.str());
                }

                if (file.eof())
                    break;
            }
            file.close();
        } else {
            cout << Env::getWorkingPath() + sep + "lightgunSerials.txt" << " not found" << endl;
        }

        firstTime = false;
    }
}

//
// isLightgunGame(const string& serial)
//
bool isLightgunGame(const string& serial) {
  if (serial == "")
    return false;   // the game doesn't have a serial

    if (lightgunSerials.size() == 0)
        loadLightgunSerialsFile();

  auto it = find(lightgunSerials.begin(), lightgunSerials.end(), serial);
  return it != lightgunSerials.end();
}