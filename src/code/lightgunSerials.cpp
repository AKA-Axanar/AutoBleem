#include "lightgunSerials.h"
#include <vector>
#include <algorithm>

using namespace std;

// 
// List of PS1 Light Gun Games
//
static vector<string> lightgunSerials {
  "SLUS-00164", // Area 51 US (Justifier/Hyperblaster)
  "SLES-00578", // Area 51 PAL (Justifier/Hyperblaster)
  "SLES-03783", // Area 51 [MIDWAY CLASSICS] PAL (Justifier/Hyperblaster)
  "SLPS-00726", // Area 51 JAP (Justifier/Hyperblaster)
  "SLPS-00725", // Area 51 Special Pack JAP (Justifier/Hyperblaster)

  "SLUS-00335", // Crypt Killer US (Justifier/Hyperblaster)
  "SLES-00292", // Crypt Killer PAL (Justifier/Hyperblaster)

  "SLUS-00119", // Die Hard Trilogy US (Justifier/Hyperblaster)
  "SLUS-01015", // Die Hard Trilogy 2 - Viva Las Vegas US (Justifier/Hyperblaster)
  "SLES-00445", // Die Hard Trilogy PAL (Justifier/Hyperblaster or Guncon)
  "SLES-02746", // Die Hard Trilogy 2 - Viva Las Vegas PAL (Justifier/Hyperblaster or Guncon)
  "SLES-02747", // Die Hard Trilogy 2 - Viva Las Vegas PAL (Justifier/Hyperblaster or Guncon)
  "SLES-02748", // Die Hard Trilogy 2 - Viva Las Vegas PAL (Justifier/Hyperblaster or Guncon)
  "SLES-02749", // Die Hard Trilogy 2 - Viva Las Vegas PAL (Justifier/Hyperblaster or Guncon)
  "SLPS-00585", // Die Hard Trilogy JAP (Justifier/Hyperblaster or Guncon)

  "SLUS-00654", // Elemental Gearbolt US (Justifier/Hyperblaster or GunCon)
  "SCPS-10038", // Elemental Gearbolt JAP (Justifier/Hyperblaster or GunCon)

  "SLES-03990", // Extreme Ghostbusters: Ultimate Invasion PAL (GunCon)

  "SCES-02543", // Ghoul Panic PAL (uses GunCon)
  "SCPS-45491", // Ghoul Panic JAP (Released as "Oh! Bakyuuun" in Japan, uses GunCon)
  "SLPS-02680", // Ghoul Panic JAP (Released as "Oh! Bakyuuun" in Japan, uses GunCon)

  "SLPS-01322", // GunBare! Game Tengoku 2 JAP (JPN only)

  "SLUS-01398", // Gunfighter: The Legend of Jesse James US (GunCon)
  "SLES-03689", // Gunfighter: The Legend of Jesse James PAL (GunCon)

  "SLPS-01106", // Guntu - Western Front June, 1944 (JPN only, GunCon)

  "SLUS-00630", // Judge Dredd US (Justifier/Hyperblaster or GunCon)
  "SLES-00755", // Judge Dredd PAL (Justifier/Hyperblaster or GunCon)

  "SLUS-00293", // Lethal Enforcers 1&2 US (Justifier/Hyperblaster)
  "SLES-00542", // Lethal Enforcers 1&2 PAL (Justifier/Hyperblaster)
  "SLPM-86025", // Lethal Enforcers 1&2 JAP (Justifier/Hyperblaster)

  "SLUS-00503", // Maximum Force - Pull the Trigger US (Justifier/Hyperblaster or GunCon)
  "SLES-01001", // Maximum Force - Pull the Trigger PAL (Justifier/Hyperblaster or GunCon)

  "SLES-02244", // Mighty Hits Special PAL (EU/JPN only, Justifier/Hyperblaster)
  "SLPS-02165", // Mighty Hits Special JAP (EU/JPN only, Justifier/Hyperblaster)

  "SLES-03846", // Moorhuhn 3 - Chicken Chase PAL (GunCon)
  "SLES-03278", // Moorhuhn 2 - Die Jagd Geht Weiter PAL (GunCon)
  "SLES-04122", // Moorhuhn Kart PAL (GunCon)
  "SLES-04174", // Moorhuhn X PAL (GunCon)

  "SLUS-00481", // Point Blank US (GunCon)
  "SCES-00886", // Point Blank PAL (GunCon)
  "SLUS-00796", // Point Blank 2 US (GunCon)
  "SCES-02180", // Point Blank 2 PAL GunCon)
  "SLUS-01354", // Point Blank 3 US (GunCon)
  "SCES-03383", // Point Blank 3 PAL (GunCon)

  "SCUS-94408", // Project Horned Owl US (Justifier/Hyperblaster)

  "SCES-02569", // Rescue Shot PAL (GunCon)
  "SCPS-45477", // Rescue Shot Bubibo JAP (GunCon)
  "SLPS-02555", // Rescue Shot Bubibo JAP (GunCon)

  "SLUS-01087", // ?????  Resident Evil Survivor US (JPN/PAL versions) (GunCon)
  "SLES-02732", // Resident Evil Survivor PAL (JPN/PAL versions) (GunCon)
  "SLES-02744", // Resident Evil Survivor PAL (JPN/PAL versions) (GunCon)
//??? JAP version not found.    Resident Evil Survivor (JPN/PAL versions) (GunCon)

  "SLUS-00012", // Revolution X US (Justifier)
  "SLES-00129", // Revolution X PAL (Justifier)
  "SLES-00151", // Revolution X PAL (Justifier)
  "SLPS-00258", // Revolution X JAP (Justifier)

  "SLUS-00707", // Silent Hill US (Justifier/Hyperblaster, used for an easter egg)
  "SLES-01514", // Silent Hill PAL (Justifier/Hyperblaster, used for an easter egg)
  "SCPS-45380", // Silent Hill JAP (Justifier/Hyperblaster, used for an easter egg)
  "SLPM-86192", // Silent Hill JAP (Justifier/Hyperblaster, used for an easter egg)
  "SLPM-86498", // ???? Silent Hill [Konami The Best] JAP (Justifier/Hyperblaster, used for an easter egg)

  "SLPS-02474", // Simple 1500 Series Vol.024 - The Gun Shooting JAP (JPN only, GunCon)
  "SLPM-86816", // Simple 1500 Series Vol.063 - The Gun Shooting 2 JAP (JPN only, GunCon)

  "SLPS-00154", // Snatcher JAP (JPN only, Justifier/Hyperblaster, contains light-gun sequences)

  "SLUS-00381", // Star Wars: Rebel Assault II US (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLUS-00386", // Star Wars: Rebel Assault II US (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-00654", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-10654", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-00656", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-10656", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-00584", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-10584", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-00643", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-10643", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-00644", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLES-10644", // ?????? Star Wars: Rebel Assault II - The Hidden Empire PAL (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLPS-00638", // Star Wars: Rebel Assault II JAP (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)
  "SLPS-00639", // Star Wars: Rebel Assault II JAP (compatible with the Justifier/Hyperblaster, which is used for certain shooting sequences)

  "SLUS-00405", // Time Crisis US (GunCon)
  "SCES-00657", // Time Crisis PAL (GunCon)
  "SCES-00666", // Time Crisis JAP (GunCon)

  "SLUS-01336", // Time Crisis: Project Titan (GunCon)
  "SCES-02776", // ime Crisis: Project Titan (GunCon)
  "SCPS-45514", // Time Crisis: Project Titan (GunCon)
  "SLPS-03188", // Time Crisis: Project Titan (GunCon)
};

//
// isLightgunGame(const string& serial)
//
bool isLightgunGame(const string& serial) {
  if (serial == "")
    return false;

  auto it = find(lightgunSerials.begin(), lightgunSerials.end(), serial);
  return it != lightgunSerials.end();
}