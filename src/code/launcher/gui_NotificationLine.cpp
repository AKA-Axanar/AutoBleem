#include "gui_NotificationLine.h"
#include "gui_launcher.h"
#include "../gui/gui.h"

using namespace std;

//*******************************
// NotificationLine::setText
//*******************************
void NotificationLine::setText(string _text, long _timeLimitInMilliSeconds, const SDL_Color & _textColor, FontEnum _fontEnum) {
    text = _text;
    timed = (_timeLimitInMilliSeconds != 0);
    notificationTime = SDL_GetTicks();  // tick count when setText called
    if (notificationTime == 0)  // if by chance it's 0.  0 flags that the timeLimit has been reached and to turn off the display
        ++notificationTime;
    timeLimit = _timeLimitInMilliSeconds;
    textColor = _textColor;
    fontEnum = _fontEnum;
};

//*******************************
// NotificationLine::setText
//*******************************
void NotificationLine::setText(string _text, long _timeLimitInMilliSeconds) {
    setText(_text, _timeLimitInMilliSeconds, textColor, fontEnum);
};

//*******************************
// NotificationLine::tickTock
//*******************************
void NotificationLine::tickTock() {
    auto gui = Gui::getInstance();
    if (timed) {
        if (notificationTime != 0) {
            long currentTimeTicks = SDL_GetTicks();
            if (currentTimeTicks - notificationTime > timeLimit) // if time limit reached
                notificationTime = 0;   // turn off the display
        }
        if (notificationTime != 0)
            gui->renderText_WithColor(gui->themeFonts[fontEnum], text, x, y, textColor, XALIGN_CENTER, true);
    } else // not timed - keep display on
        gui->renderText_WithColor(gui->themeFonts[fontEnum], text, x, y, textColor, XALIGN_CENTER, true);
}

//*******************************
// NotificationLines::createAndSetDefaults
//*******************************
void NotificationLines::createAndSetDefaults(int count, int x_start, int y_start, FontEnum fontEnum, int fontHeight, int separationBetweenLines) {
    for (int line=0; line < count; ++line) {
        NotificationLine notificationLine;
        notificationLine.fontEnum = fontEnum;
        notificationLine.textColor = brightWhite;
        notificationLine.x = x_start;
        notificationLine.y = y_start + (line * (fontHeight + separationBetweenLines));
        notificationLine.timed = true;
        notificationLine.timeLimit = DefaultShowingTimeout;

        lines.push_back(notificationLine);
    }
}

//*******************************
// NotificationLines::tickTock
//*******************************
void NotificationLines::tickTock() {
    for (auto & line : lines)
        line.tickTock();
}
