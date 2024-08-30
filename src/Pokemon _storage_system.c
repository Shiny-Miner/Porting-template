#include "../include/script.h"
#include "../include/sound.h"
#include "../include/string_util.h"
#include "../include/strings.h"
#include "../include/text.h"
#include "../include/text_window.h"
#include "../include/trig.h"
#include "../include/walda_phrase.h"
#include "../include/window.h"
#include "../include/constants/items.h"
#include "../include/constants/moves.h"
#include "../include/constants/rgb.h"
#include "../include/constants/songs.h"
#include "../include/constants/pokemon_icon.h"

gKeyRepeatContinueDelay = 3;
gKeyRepeatStartDelay = 30;

#define INSTANT_BOX 0

#define CURSOR_MOVE_BASE_STEPS_WRAP 3
#define CURSOR_MOVE_BASE_STEPS_NO_WRAP 3

#define BOX_SCROLL_SPEED_FACTOR 4
#define BOX_SCROLL_ARROWS_START_X_DELTA 12

#define BOX_SCROLL_SPEED(input) ((input) * 6 * BOX_SCROLL_SPEED_FACTOR)

#define PARTY_MENU_SCROLL_DELTA 2
#define PARTY_MENU_ADJUSTED_SCROLL_TIMER (20/PARTY_MENU_SCROLL_DELTA)

#define COMPACT_PARTY_SPRITES_DELTA 2

#define MON_HAND_DELTA 2

const u16 ALIGNED(4) *palettes;
u16 ALIGNED(4) wallpaperTilemap[360];
s16 iconScrollDistance;
static void CreateInstantIncomingBoxTitle(u8 boxId, s8 direction);
static void SpriteCB_InstantIncomingBoxTitle(struct Sprite *);
static void SpriteCB_InstantOutgoingBoxTitle(struct Sprite *);

static bool8 ShowPartyMenu(void)
{
  if (sStorage->partyMenuMoveTimer == PARTY_MENU_ADJUSTED_SCROLL_TIMER)
        return FALSE;

    sStorage->partyMenuUnused1--;
sStorage->partyMenuY += PARTY_MENU_SCROLL_DELTA;
    TilemapUtil_Move(TILEMAPID_PARTY_MENU, 3, PARTY_MENU_SCROLL_DELTA);
TilemapUtil_Update(TILEMAPID_PARTY_MENU);
    ScheduleBgCopyTilemapToVram(1);
MovePartySprites(8 * PARTY_MENU_SCROLL_DELTA);
    if (++sStorage->partyMenuMoveTimer == PARTY_MENU_ADJUSTED_SCROLL_TIMER)
      {
        sInPartyMenu = TRUE;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static bool8 HidePartyMenu(void)
{
  
    if (sStorage->partyMenuMoveTimer != PARTY_MENU_ADJUSTED_SCROLL_TIMER)
    {
        sStorage->partyMenuUnused1++;
       sStorage->partyMenuY -= PARTY_MENU_SCROLL_DELTA;
        TilemapUtil_Move(TILEMAPID_PARTY_MENU, 3, -PARTY_MENU_SCROLL_DELTA);
        TilemapUtil_Update(TILEMAPID_PARTY_MENU);
FillBgTilemapBufferRect_Palette0(1, 0x100, 10, sStorage->partyMenuY, 12, PARTY_MENU_SCROLL_DELTA);
        MovePartySprites(-8 * PARTY_MENU_SCROLL_DELTA);
        if (++sStorage->partyMenuMoveTimer != PARTY_MENU_ADJUSTED_SCROLL_TIMER)
          {
            ScheduleBgCopyTilemapToVram(1);
            return TRUE;
        }
        else
        {
            sInPartyMenu = FALSE;
            DestroyAllPartyMonIcons();
            CompactPartySlots();
            // The close box button gets partially covered by
            // the party menu, restore it
            TilemapUtil_SetRect(TILEMAPID_CLOSE_BUTTON, 0, 0, 9, 2);
            TilemapUtil_Update(TILEMAPID_CLOSE_BUTTON);
            ScheduleBgCopyTilemapToVram(1);
            return FALSE;
        }
    }
    return FALSE;
}
