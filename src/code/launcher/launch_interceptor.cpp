//
// Created by screemer on 2019-12-01.
//

#include "launch_interceptor.h"
#include "../util.h"
#include "../util_time.h"
#include "../gui/gui.h"
#include <iostream>
#include <unistd.h>
#include "../environment.h"

#ifndef __APPLE__
#include <wait.h>
#endif
using namespace std;

bool LaunchInterceptor::execute(PsGamePtr &game, int resumepoint) {
    cout << "calling LaunchInterceptor::execute()" <<endl;

    shared_ptr<Gui> gui(Gui::getInstance());
    cout << "Starting External App" << endl;

    if (game->internal) {
        gui->internalDB->updateDatePlayed(game->gameId, UtilTime::getCurrentTime());
    } else {
        gui->db->updateDatePlayed(game->gameId, UtilTime::getCurrentTime());
    }

    if (game->foreign) {
        cout << "FOREIGN MODE" << endl;
    }

    gui->saveSelection();
    std::vector<const char *> argvNew;

    string link = game->base + sep+ game->startup;
    argvNew.push_back(link.c_str());
    argvNew.push_back(nullptr);

    cout << "CMD line to execute: ";
    for (const char *s:argvNew) {
        if (s != nullptr) {
            cout << s << " ";
        }
    }
    cout << endl;


#if defined(__x86_64__) || defined(_M_X64) || defined (PI_DEBUG)
    Gui::splash("I'm sorry Dave.  I'm afraid I can't do that.");
#else
    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }
    waitpid(pid, NULL, 0);
    usleep(3 * 1000);
#endif

    return true;
}

