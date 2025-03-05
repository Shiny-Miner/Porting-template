#pragma once

#include <stddef.h>

struct Main
{
    u8 data1[0x2E];
    u16 newKeys;               // newly pressed keys with L=A remapping
    u8 data2[0x40C];
};

_Static_assert(sizeof(struct Main) == 0x43C, "");
_Static_assert(offsetof(struct Main, newKeys) == 0x2E, "");

extern struct Main gMain;
