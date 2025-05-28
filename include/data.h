#ifndef GUARD_DATA_H
#define GUARD_DATA_H

#include "global.h"

#define SPECIES_SHINY_TAG 500
#define TRAINER_ENCOUNTER_MUSIC(trainer)((gTrainers[trainer].encounterMusic_gender & 0x7F))

struct MonCoords
{
    // This would use a bitfield, but some function
    // uses it as a u8 and casting won't match.
    u8 size; // u8 width:4, height:4;
    u8 y_offset;
};

extern const u8 gSpeciesNames[][POKEMON_NAME_LENGTH + 1];
extern const u8 gMoveNames[][13];
extern const u16 gUnknown_8251CB8[];
extern const u16 gUnknown_8251FEE[];
extern const u16 gUnknown_8252324[];
extern const u16 gUnknown_82539D4[];

extern const u8 gTrainerClassNames[][13];

#endif // GUARD_DATA_H
