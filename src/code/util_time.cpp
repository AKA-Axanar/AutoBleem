#include "util_time.h"
#include<time.h>
#include "gui/gui.h"

using namespace std;

//*******************************
// UtilTime::getCurrentTime
//*******************************
time_t UtilTime::getCurrentTime() {
    time_t t = time(nullptr);
    //cout << "Current Time = " << t << ", " << timeToDisplayTimeString(t) << endl;

    return t;
}

//*******************************
// UtilTime::usingWiFiUpdatedTime
// returns true if using AB kernel and it used WiFi to update the current time
//*******************************
bool UtilTime::usingWiFiUpdatedTime() {
    time_t t = time(nullptr);
    tm* local = localtime(&t);

    return (local != nullptr) && (local->tm_year + 1900 >= 2020);   // return true if the year >= 2020
}

//*******************************
// UtilTime::timeToDisplayTimeString
//*******************************
string UtilTime::timeToDisplayTimeString(time_t t, const string& _format) {
    string datetime;
    string format = _format;    // if you pass a format it uses that

    if (format == "") {
        // see if the use has a prefered format in config.ini
        string datetimeFormat = Gui::getInstance()->cfg.inifile.values["datetimeformat"];
        if (datetimeFormat != "")
            format = datetimeFormat;    // use the format in the config.ini
        else
            format = "%F %I:%M:%S %p";           // default: YYYY-MM-DD HH:MM:SS
    }

    if (t != 0) {
        tm* local = localtime(&t);
        if ((local != nullptr)  && (local->tm_year + 1900 >= 2020)) {  // if datetime is from a WiFi enabled datetime
            char buf[200];
            if (std::strftime(buf, sizeof(buf), format.c_str(), local))
                datetime = buf;
        }
    }

    return datetime;
}
