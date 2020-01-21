//
// Created by screemer on 23.12.18.
//

#include "config.h"
#include "../util.h"
#include "../launcher/gui_NotificationLine.h"
#include "../DirEntry.h"
#include "../environment.h"

//*******************************
// Config::Config()
//*******************************
Config::Config()
{
    std::string path=Env::getWorkingPath() + sep + "config.ini";
    inifile.load(path);

    inifile.values.erase("stheme"); // stheme no longer used since themes were consolidated into /Themes
    inifile.values.erase("autoregion"); // autoregion no longer needed

    // the quick boot option has been removed.
    // if it exists and is false then set quickmenu to "menu" so it will boot to the normal menu at the bottom.
    // if it exists and is true then leave quickmenu at it's current setting (UI or Retroarch).
    if (inifile.values["quick"] != "") {
        if (inifile.values["quick"] == "false")
            inifile.values["quickmenu"] = "Menu";

        inifile.values.erase("quick");  // remove "quick"
        save(); // update the changes
    }

    inifile.values["adv"]="true";

    bool aDefaultWasSet {false};
    if (inifile.values["language"]=="")
    {
        inifile.values["language"]="English";
        aDefaultWasSet = true;
    }
    if (inifile.values["delay"] == "") {
        inifile.values["delay"] = "1";
        aDefaultWasSet = true;
    }
    if (inifile.values["ui"]=="")
    {
        inifile.values["ui"]="classic";
        aDefaultWasSet = true;
    }
    if (inifile.values["aspect"]=="")
    {
        inifile.values["aspect"]="false";
        aDefaultWasSet = true;
    }
    if (inifile.values["jewel"]=="")
    {
        inifile.values["jewel"]="default";
        aDefaultWasSet = true;
    }
    if (inifile.values["quickmenu"]=="")
    {
        inifile.values["quickmenu"]="UI";
        aDefaultWasSet = true;
    }
    if (inifile.values["music"]=="")
    {
        inifile.values["music"]="--";
        aDefaultWasSet = true;
    }
    if (inifile.values["showingtimeout"]=="")
    {
        inifile.values["showingtimeout"]=DefaultShowingTimeoutText;
        aDefaultWasSet = true;
    }

    if (inifile.values["raconfig"]=="")
    {
        inifile.values["raconfig"]="true";
        aDefaultWasSet = true;
    }

    inifile.values["pcsx"]="bleemsync";

    if (aDefaultWasSet)
        save();
}

//*******************************
// Config::save
//*******************************
void Config::save()
{
    inifile.values["pcsx"]="bleemsync";
    std::string path=Env::getWorkingPath() + sep + "config.ini";
    inifile.save(path);
}
