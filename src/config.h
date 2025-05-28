#pragma once

#define FLAG_NO_CATCHING 0x902 //If using HUBOL or other build verison of CFRU then do not mess this up
#define FLAG_NUZLOCKE 0xA06 //If set nuzlock mode will be turned on
#define VAR_WILD_BATTLE_COUNT 0x5155 //Counts number of wild battles, if >1 then NO_CATCHING is enabled.
#define FLAG_VISITED_AREA_START 0x1300 //Flag that is set in a nuzlocke for every visited area (Helpful in tracking)
