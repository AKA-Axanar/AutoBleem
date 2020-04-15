//
// Created by screemer on 2/12/19.
//

#include "ps_meta.h"
#include "ps_game.h"
#include "../util.h"
#include "../util_time.h"
#include <SDL2/SDL_image.h>
#include "../lang.h"
#include "../engine/inifile.h"
#include "../DirEntry.h"
#include "../environment.h"

using namespace std;

//*******************************
// PsMeta::updateTexts
//*******************************
void PsMeta::updateTexts(const string & gameNameTxt, const string & publisherTxt, const string & yearTxt,
                         const string & serial, const string & region, const string & playersTxt, bool internal,
                         bool hd, bool locked, int discs, bool favorite,  bool foreign, bool app,
                         const string& last_played,
                         int r,int g, int b) {
    this->discs = discs;
    this->internal = internal;
    this->hd = hd;
    this->locked = locked;
    this->favorite = favorite;
    this->gameName = gameNameTxt;
    this->publisher = publisherTxt;
    this->year = yearTxt;
    this->serial = serial;
    this->region = region;
    this->players = playersTxt;
    this->foreign = foreign;
    this->app = app;
    this->last_played = last_played;

    gameNameTex = createTextTex(gameName, r,g,b, fonts[FONT_28_BOLD]);
    publisherAndYearTex = createTextTex(publisher + ", " + year, r,g,b, fonts[FONT_15_BOLD]);
    if (serial != "")
        serialAndRegionTex = createTextTex(_("Serial:") + " " + serial + ", " + _("Region:") + " " + region, r,g,b, fonts[FONT_15_BOLD]);
	else
        serialAndRegionTex = createTextTex("", r,g,b, fonts[FONT_15_BOLD]);
    playersTex = createTextTex(playersTxt, r,g,b, fonts[FONT_15_BOLD]);
    discsTex = createTextTex(to_string(discs), r,g,b, fonts[FONT_15_BOLD]);
    datePlayedTex = createTextTex(_("Last Played:") + " " + last_played, r,g,b, fonts[FONT_15_BOLD]);

    if (foreign)
    {
        trim(publisher);
        if (publisher=="DETECT")
        {
            publisher = _("Unknown Core (AutoDetect)");
        }
        publisherAndYearTex = createTextTex(publisher, r,g,b, fonts[FONT_15_BOLD]);
    }
}

//*******************************
// PsMeta::updateTexts
//*******************************
void PsMeta::updateTexts(PsGamePtr & psGame, int r,int g, int b) {
    string appendText = psGame->players == 1 ? _("Player") : _("Players");
    if (!psGame->foreign) {
        if (psGame->serial == "") {
            Inifile iniFile;
            iniFile.load(psGame->folder + sep + "Game.ini");
            psGame->serial = iniFile.values["serial"];
            psGame->region = iniFile.values["region"];
        }
        updateTexts(psGame->title, psGame->publisher, to_string(psGame->year), psGame->serial, psGame->region,
                    to_string(psGame->players) + " " + appendText,
                    psGame->internal, psGame->hd, psGame->locked, psGame->cds, psGame->favorite,
                    psGame->foreign, psGame->app, UtilTime::timeToDisplayTimeString(psGame->last_played),
                    r, g, b);
    } else
    {
        if (psGame->app)
        {
            psGame->serial = "";
            psGame->region = "";

            updateTexts(psGame->title, psGame->publisher, to_string(psGame->year), psGame->serial, psGame->region,
                        to_string(psGame->players) + " " + appendText,
                        psGame->internal, psGame->hd, psGame->locked, psGame->cds, psGame->favorite,
                        psGame->foreign, psGame->app,  UtilTime::timeToDisplayTimeString(psGame->last_played),
                        r, g, b);
        } else {
            psGame->serial = "";
            psGame->region = "";

            updateTexts(psGame->title, psGame->core_name, to_string(psGame->year), psGame->serial, psGame->region,
                        to_string(psGame->players) + " " + appendText,
                        psGame->internal, psGame->hd, psGame->locked, psGame->cds, psGame->favorite,
                        psGame->foreign, psGame->app,  UtilTime::timeToDisplayTimeString(psGame->last_played),
                        r, g, b);
        }
    }
}

//*******************************
// PsMeta::destroy
//*******************************
void PsMeta::destroy() {
}

//*******************************
// PsMeta::render
//*******************************
void PsMeta::render() {
    if (gameName=="")
    {
        return;
    }

    if (internalOffTex == nullptr) {
        string curPath = Env::getWorkingPath() + sep;
        internalOnTex =  IMG_LoadTexture(renderer, (curPath + "evoimg/ps1.png").c_str());
        internalOffTex = IMG_LoadTexture(renderer, (curPath + "evoimg/usb.png").c_str());
        hdOnTex =        IMG_LoadTexture(renderer, (curPath + "evoimg/hd.png").c_str());
        hdOffTex =       IMG_LoadTexture(renderer, (curPath + "evoimg/sd.png").c_str());
        lockOnTex =      IMG_LoadTexture(renderer, (curPath + "evoimg/lock.png").c_str());
        lockOffTex =     IMG_LoadTexture(renderer, (curPath + "evoimg/unlock.png").c_str());
        cdTex =          IMG_LoadTexture(renderer, (curPath + "evoimg/cd.png").c_str());
        favoriteTex =    IMG_LoadTexture(renderer, (curPath + "evoimg/favorite.png").c_str());
        raTex =          IMG_LoadTexture(renderer, (curPath + "evoimg/ra.png").c_str());
    }

    if (visible) {
        Uint32 format;
        int access;
        int w, h;
        SDL_Rect rect;
        SDL_Rect fullRect;

        int yOffset = 0;
        // game name line
        {
            SDL_QueryTexture(gameNameTex, &format, &access, &w, &h);
            rect.x = x;
            rect.y = y + yOffset;
            rect.w = w;

            if (rect.w > 490) rect.w = 490;
            rect.h = h;
            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.w = w;
            fullRect.h = h;
            SDL_RenderCopy(renderer, gameNameTex, &fullRect, &rect);
        }

        yOffset += 35;
        // publisher line
        {
            SDL_QueryTexture(publisherAndYearTex, &format, &access, &w, &h);

            rect.x = x;
            rect.y = y + yOffset;
            rect.w = w;
            rect.h = h;

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.w = w;
            fullRect.h = h;
            SDL_RenderCopy(renderer, publisherAndYearTex, &fullRect, &rect);
        }

        yOffset += 21;
        // serial number line
        SDL_QueryTexture(serialAndRegionTex, &format, &access, &w, &h);

        rect.x = x;
        rect.y = y + yOffset;
        rect.w = w;
        rect.h = h;

        fullRect.x = 0;
        fullRect.y = 0;
        fullRect.w = w;
        fullRect.h = h;
        SDL_RenderCopy(renderer, serialAndRegionTex, &fullRect, &rect);


        yOffset += 21;
        // last played line
        {
            SDL_QueryTexture(datePlayedTex, &format, &access, &w, &h);

            rect.x = x;
            rect.y = y + yOffset;
            rect.w = w;
            rect.h = h;

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.w = w;
            fullRect.h = h;
            if (!foreign) {     // if not RA
#if defined(__x86_64__) || defined(_M_X64)
                // the devel system has time
                SDL_RenderCopy(renderer, datePlayedTex, &fullRect, &rect);
#else
                if (Env::autobleemKernel)
                    SDL_RenderCopy(renderer, datePlayedTex, &fullRect, &rect);
#endif
            }
        }

        yOffset += 22;
        if (!foreign) {
            // PS1 icons line
            SDL_QueryTexture(playersTex, &format, &access, &w, &h);
            rect.x = x + 35;
            rect.y = y + yOffset;
            rect.w = w;
            rect.h = h;

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.w = w;
            fullRect.h = h;
            SDL_RenderCopy(renderer, playersTex, &fullRect, &rect);

            SDL_QueryTexture(tex, &format, &access, &w, &h);
            rect.x = x;
            rect.y = y + yOffset - 2;
            rect.w = w;
            rect.h = h;

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.w = w;
            fullRect.h = h;
            SDL_RenderCopy(renderer, tex, &fullRect, &rect);

            int offset = 190, spread = 40;
            // render internal icon
            rect.x = x + 135;
            SDL_RenderCopy(renderer, cdTex, &fullRect, &rect);

            SDL_QueryTexture(discsTex, &format, &access, &w, &h);
            rect.x = x + 170;
            rect.y = y + yOffset;
            rect.w = w;
            rect.h = h;

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.w = w;
            fullRect.h = h;
            SDL_RenderCopy(renderer, discsTex, &fullRect, &rect);

            rect.x = x + offset;
            rect.y = y + yOffset - 2;
            rect.w = 30;
            rect.h = 30;

            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.w = 30;
            fullRect.h = 30;
            if (internal) {
                locked = true;
                hd = false;
                SDL_RenderCopy(renderer, internalOnTex, &fullRect, &rect);
            } else {
                SDL_RenderCopy(renderer, internalOffTex, &fullRect, &rect);
            }
            rect.x = x + offset + spread;
            if (hd) {
                SDL_RenderCopy(renderer, hdOnTex, &fullRect, &rect);
            } else {
                SDL_RenderCopy(renderer, hdOffTex, &fullRect, &rect);
            }
            rect.x = x + offset + spread * 2;
            if (locked) {
                SDL_RenderCopy(renderer, lockOnTex, &fullRect, &rect);
            } else {
                SDL_RenderCopy(renderer, lockOffTex, &fullRect, &rect);
            }
            rect.x = x + offset + spread * 3;
            if (favorite) {
                SDL_RenderCopy(renderer, favoriteTex, &fullRect, &rect);
            }
        } else
        {
            // retroarch icon
            if (!app) {
                SDL_QueryTexture(raTex, &format, &access, &w, &h);
                rect.x = x;
                rect.y = y + yOffset - 2;
                rect.w = w;
                rect.h = h;

                fullRect.x = 0;
                fullRect.y = 0;
                fullRect.w = w;
                fullRect.h = h;
                SDL_RenderCopy(renderer, raTex, &fullRect, &rect);
            }
        }

    }
}

//*******************************
// PsMeta::createTextTex
//*******************************
SDL_Shared<SDL_Texture> PsMeta::createTextTex(const string & text, Uint8 r, Uint8 g, Uint8 b, TTF_Font_Shared font) {

    SDL_Shared<SDL_Surface> surface;
    SDL_Shared<SDL_Texture> texture;
    SDL_Color textColor = {r, g, b, 0};

    if (text.size() == 0) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);

    } else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
    }

    return texture;
}

//*******************************
// PsMeta::update
//*******************************
void PsMeta::update(long time) {
    if (visible)
        if (animEndTime != 0) {
            if (animStarted == 0) {
                animStarted = time;
            }

            if (animStarted != 0) {
                // calculate length for point in time
                long currentAnim = time - animStarted;
                long totalAnimTime = animEndTime - animStarted;
                float position = currentAnim * 1.0f / totalAnimTime * 1.0f;
                int newPos = prevPos + ((nextPos - prevPos) * position);
                y = newPos;
            }

            if (time >= animEndTime) {
                animStarted = 0;
                animEndTime = 0;
                y = nextPos;
            }
        }
    lastTime = time;
}
