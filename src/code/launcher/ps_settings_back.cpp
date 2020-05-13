//
// Created by screemer on 2/11/19.
//

#include "ps_settings_back.h"
#include "../gui/gui.h"

//*******************************
// PsSettingsBack::setCurLen
//*******************************
void PsSettingsBack::setCurLen(int len) {
    y = 632 - len;
    h = len;
    x = 0;
    w = SCREEN_WIDTH;
    nextLen = len;
    prevLen = len;
    animEndTime = 0;
}

//*******************************
// PsSettingsBack::update
//*******************************
void PsSettingsBack::update(long time) {
    if (visible)
    if (animEndTime != 0) {
        if (animStarted == 0) {
            animStarted = time;
            prevLen = h;
        }

        if (animStarted != 0) {
            // calculate length for point in time
            long currentAnim = time - animStarted;
            long totalAnimTime = animEndTime - animStarted;
            float position = currentAnim * 1.0f / totalAnimTime * 1.0f;
            int newSize = prevLen + ((nextLen - prevLen) * position);
            y = 632 - newSize;
            h = newSize;
            x = 0;
            w = SCREEN_WIDTH;


        }

        if (time >= animEndTime) {
            animStarted = 0;
            animEndTime = 0;
            y = 632 - nextLen;
            h = nextLen;
            x = 0;
            w = SCREEN_WIDTH;
        }

    }
    lastTime = time;
}