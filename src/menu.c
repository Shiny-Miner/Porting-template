#include "../include/global.h"

#include "../include/help_message.h"
#include "../include/link.h"
#include "../include/main.h"
#include "../include/menu.h"
#include "../include/menu_helpers.h"
#include "../include/new_menu_helpers.h"
#include "../include/start_menu.h"
#include "../include/sound.h"

#include "../include/constants/songs.h"

bool8 StartMenuHasOption(u8 option);
bool8 StartCB_SelectOption(u8 option);

bool8 StartCB_HandleInput(void)
{
    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(-1);
        if (!MenuHelpers_IsLinkActive() && InUnionRoom() != TRUE && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_HELP)
        {
            PrintTextOnHelpMessageWindow(sStartMenuDescPointers[sStartMenuOrder[sStartMenuCursorPos]], 2);
        }
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(+1);
        if (!MenuHelpers_IsLinkActive() && InUnionRoom() != TRUE && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_HELP)
        {
            PrintTextOnHelpMessageWindow(sStartMenuDescPointers[sStartMenuOrder[sStartMenuCursorPos]], 2);
        }
    }
    if (JOY_NEW(A_BUTTON))
        return StartCB_SelectOption(sStartMenuOrder[sStartMenuCursorPos]);
    if (JOY_NEW(B_BUTTON | START_BUTTON))
    {
        DestroySafariZoneStatsWindow();
        DestroyHelpMessageWindow_();
        CloseStartMenu();
        return TRUE;
    }
    if (JOY_NEW(R_BUTTON) && StartMenuHasOption(STARTMENU_SAVE))
        return StartCB_SelectOption(STARTMENU_SAVE);
    // if (JOY_NEW(L_BUTTON) && StartMenuHasOption(STARTMENU_POKEMON))
        // return StartCB_SelectOption(STARTMENU_POKEMON);
    // if (JOY_NEW(SELECT_BUTTON) && StartMenuHasOption(STARTMENU_BAG))
        // return StartCB_SelectOption(STARTMENU_BAG);
    return FALSE;
}

bool8 StartMenuHasOption(u8 option)
{
    int i;

    for (i = 0; i < sNumStartMenuItems; ++i)
    {
        if (sStartMenuOrder[i] == option)
            return TRUE;
    }
    return FALSE;
}

bool8 StartCB_SelectOption(u8 option)
{
    PlaySE(SE_SELECT);
    if (!StartMenuPokedexSanityCheck())
        return FALSE;
    sStartMenuCallback = sStartMenuActionTable[option].func.u8_void;
    StartMenu_FadeScreenIfLeavingOverworld();
    return FALSE;
}
