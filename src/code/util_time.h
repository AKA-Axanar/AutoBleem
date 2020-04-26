
#pragma once

#include <string>
#include <ctime>

//******************
// UtilTime
//******************
class UtilTime {
public:
    static time_t getCurrentTime();
    static bool usingWiFiUpdatedTime(); // returns true if using AB kernel and it used WiFi to update the current time
    static std::string timeToDisplayTimeString(time_t t, const std::string& format = "");
};

