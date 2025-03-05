#pragma once

#include <stddef.h>

#include "gba/gba.h"

#include "constants/global.h"

#define TEST_BUTTON(field, button) ({(field) & (button);})
#define JOY_NEW(button)      TEST_BUTTON(gMain.newKeys,  button)

#define PACKED __attribute__((packed))
#define NAKED __attribute__((naked))

struct SaveBlock2
{
    u8 data1[0x13];
    u8 optionsButtonMode;  // OPTIONS_BUTTON_MODE_[NORMAL/LR/L_EQUALS_A]
    u8 data2[0xF10];
}; // size: 0xF24

_Static_assert(sizeof(struct SaveBlock2) == 0xF24, "");
_Static_assert(offsetof(struct SaveBlock2, optionsButtonMode) == 0x13, "");

extern struct SaveBlock2 *gSaveBlock2Ptr;
