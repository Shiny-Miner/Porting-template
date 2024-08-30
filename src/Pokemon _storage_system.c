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

