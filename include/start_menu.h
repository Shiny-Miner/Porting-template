#pragma once

#define STARTMENU_POKEMON 1
#define STARTMENU_BAG 2
#define STARTMENU_SAVE 4

struct MenuAction
{
    const u8 *text;
    union {
        void (*void_u8)(u8);
        u8 (*u8_void)(void);
    } func;
};

extern u8 sStartMenuCursorPos;
extern u8 sNumStartMenuItems;
extern u8 sStartMenuOrder[];
extern bool8 (*sStartMenuCallback)(void);
extern const u8 *const sStartMenuDescPointers[];
extern const struct MenuAction sStartMenuActionTable[];

bool8 StartMenuPokedexSanityCheck(void);
void StartMenu_FadeScreenIfLeavingOverworld(void);
void DestroySafariZoneStatsWindow(void);
void CloseStartMenu(void);
