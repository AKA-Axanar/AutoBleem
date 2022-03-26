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
#include "../LightgunGames.h"

using namespace std;

//*******************************
// PsMeta::updateTexts
//*******************************
void PsMeta::updateTexts(const string & gameNameTxt, const string & publisherTxt, const string & yearTxt,
                         const string & serial, const string & region, const string & playersTxt, bool internal,
                         bool hd, bool locked, int discs, bool favorite, bool play_using_ra, bool foreign, bool app,
                         const string& last_played, const std::string& _gamePathForLightgunGamesFile,
                         SDL_Color _textColor) {
    this->discs = discs;
    this->internal = internal;
    this->hd = hd;
    this->locked = locked;
    this->favorite = favorite;
    this->play_using_ra = play_using_ra;
    this->gameName = gameNameTxt;
    this->publisher = publisherTxt;
    this->year = yearTxt;
    this->serial = serial;
    this->region = region;
    this->players = playersTxt;
    this->foreign = foreign;
    this->app = app;
    this->last_played = last_played;
    this->gamePathForLightgunGamesFile = _gamePathForLightgunGamesFile;

    textColor = _textColor;
    textColor.a = SDL_ALPHA_OPAQUE; // if you're rendering with a different color you need this or it will be transparent

    if (foreign) {
        trim(publisher);
        if (publisher=="DETECT")
            publisher = _("Unknown Core (AutoDetect)");
    }
}

//*******************************
// PsMeta::updateTexts
//*******************************
void PsMeta::updateTexts(PsGamePtr & psGame, SDL_Color _textColor) {
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
                    psGame->internal, psGame->hd, psGame->locked, psGame->cds, psGame->favorite, psGame->play_using_ra,
                    psGame->foreign, psGame->app, UtilTime::timeToDisplayTimeString(psGame->last_played), 
                    psGame->folder, // ps1 game path in /Games
                    _textColor);
    } else
    {
        if (psGame->app)
        {
            psGame->serial = "";
            psGame->region = "";

            updateTexts(psGame->title, psGame->publisher, to_string(psGame->year), psGame->serial, psGame->region,
                        to_string(psGame->players) + " " + appendText,
                        psGame->internal, psGame->hd, psGame->locked, psGame->cds, psGame->favorite, psGame->play_using_ra,
                        psGame->foreign, psGame->app,  UtilTime::timeToDisplayTimeString(psGame->last_played), "", 
                        _textColor);
        } else {
            psGame->serial = "";
            psGame->region = "";

            updateTexts(psGame->title, psGame->core_name, to_string(psGame->year), psGame->serial, psGame->region,
                        to_string(psGame->players) + " " + appendText,
                        psGame->internal, psGame->hd, psGame->locked, psGame->cds, psGame->favorite, psGame->play_using_ra,
                        psGame->foreign, psGame->app,  UtilTime::timeToDisplayTimeString(psGame->last_played), 
                        psGame->image_path, // Retroarch roms image path
                        _textColor);
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
        lightgunTex =    IMG_LoadTexture(renderer, (curPath + "evoimg/lightgun.png").c_str());
        lightgun2Tex =   IMG_LoadTexture(renderer, (curPath + "evoimg/lightgun2.png").c_str());
    }

    if (visible) {
        Uint32 format;
        int access;
        int w, h;
        SDL_Rect rect;
        SDL_Rect fullRect;

        auto nameFont = fonts[FONT_28_BOLD];
        auto otherFont = fonts[FONT_15_BOLD];

        int yOffset = 0;
        // game name line
        // if the game name goes off the end of the screen use a smaller font
        int textWidth = FC_GetWidth(nameFont, gameName.c_str());
        if (x + textWidth > SCREEN_WIDTH) {
            int miniMe = 28.0 * ((float)(SCREEN_WIDTH - x) / (float)(textWidth)) + 0.5;
            nameFont = Fonts::openSpecificSharedCachedFont(FONT_BOLD, miniMe);

            // if it's still a bit over the right edge go down one more font size
            textWidth = FC_GetWidth(nameFont, gameName.c_str());
            if (x + textWidth > SCREEN_WIDTH) {
                --miniMe;
                nameFont = Fonts::openSpecificSharedCachedFont(FONT_BOLD, miniMe);
            }
        }
        gui->renderText(nameFont, gameName, x, y + yOffset);

        yOffset += 35;
        // publisher line
        gui->renderText(otherFont, publisher, x, y + yOffset);

        yOffset += 21;
        // serial number line
        gui->renderText(otherFont, _("Serial:") + " " + serial + ", " + _("Region:") + " " + region, x, y + yOffset);

        yOffset += 21;
        // last played line
#if defined(__x86_64__) || defined(_M_X64) || defined (PI_DEBUG)
        // the devel system has time
        gui->renderText(otherFont, _("Last Played:") + " " + last_played, x, y + yOffset);
#else
        if (Env::autobleemKernel)
            gui->renderText(otherFont, _("Last Played:") + " " + last_played, x, y + yOffset);
#endif

        yOffset += 22;
        if (!foreign) {
            // PS1 icons line
            gui->renderText(otherFont, players, x + 35, y + yOffset);

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

            int xoffset = 190, spread = 40;
            // render internal icon
            rect.x = x + 135;
            SDL_RenderCopy(renderer, cdTex, &fullRect, &rect);

            gui->renderText(otherFont, to_string(discs), x + 170, y + yOffset);

            rect.x = x + xoffset;
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

            int spreadCount = 1;
            rect.x = x + xoffset + (spread * spreadCount);
            if (hd) {
                SDL_RenderCopy(renderer, hdOnTex, &fullRect, &rect);
            } else {
                SDL_RenderCopy(renderer, hdOffTex, &fullRect, &rect);
            }
            ++spreadCount;
            rect.x = x + xoffset + (spread * spreadCount);
            if (locked) {
                SDL_RenderCopy(renderer, lockOnTex, &fullRect, &rect);
            } else {
                SDL_RenderCopy(renderer, lockOffTex, &fullRect, &rect);
            }
            if (favorite) {
                ++spreadCount;
                rect.x = x + xoffset + (spread * spreadCount);
                SDL_RenderCopy(renderer, favoriteTex, &fullRect, &rect);
            }
            if (play_using_ra) {
                ++spreadCount;
                rect.x = x + xoffset + (spread * spreadCount);
                SDL_RenderCopy(renderer, raTex, &fullRect, &rect);
            }
            if (Gui::getInstance()->lightgunGames.IsGameALightgunGame(gamePathForLightgunGamesFile)) {
                ++spreadCount;
                rect.x = x + xoffset + (spread * spreadCount);
                if (players.size() > 0 && players[0] >= '2')
                    SDL_RenderCopy(renderer, lightgun2Tex, &fullRect, &rect);
                else
                    SDL_RenderCopy(renderer, lightgunTex, &fullRect, &rect);
            }
        } else {
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
