#ifndef FOLLOWER_MON_SPRITES_H
#define FOLLOWER_MON_SPRITES_H

#include "global.h"
#include "constants/event_objects.h"

#define EVENT_OBJ_PAL_TAG_NONE 0x1102
#define gEventObjectBaseOam_16x16 ((const struct OamData*) 0x83A36F8)
#define gEventObjectBaseOam_16x32 ((const struct OamData*) 0x83A3710)
#define gEventObjectBaseOam_32x32 ((const struct OamData*) 0x83A3718)
#define gEventObjectBaseOam_64x64 ((const struct OamData*) 0x83A3720)
#define gEventObjectSpriteOamTables_16x16 ((const struct SubspriteTable*) 0x83A3748)
#define gEventObjectSpriteOamTables_16x32 ((const struct SubspriteTable*) 0x83A379C)
#define gEventObjectSpriteOamTables_32x32 ((const struct SubspriteTable*) 0x83A37F0)
#define gEventObjectSpriteOamTables_64x64 ((const struct SubspriteTable*) 0x83A38D0)
#define gEventObjectImageAnimTable_PlayerNormal ((const union AnimCmd* const*) 0x83A3470)
#define gEventObjectImageAnimTable_Standard ((const union AnimCmd* const*) 0x83A3368)
#define gEventObjectImageAnimTable_Surfing ((const union AnimCmd* const*) 0x83A3584)
#define gEventObjectImageAnimTable_FieldMove ((const union AnimCmd* const*) 0x83A3638)
#define gEventObjectImageAnimTable_Fishing ((const union AnimCmd* const*) 0x83A3668)
#define gEventObjectImageAnimTable_VsSeekerBike ((const union AnimCmd* const*) 0x83A3640)

#define ANIM_STD_FACE_SOUTH       0
#define ANIM_STD_FACE_NORTH       1
#define ANIM_STD_FACE_WEST        2
#define ANIM_STD_FACE_EAST        3
#define ANIM_STD_GO_SOUTH         4
#define ANIM_STD_GO_NORTH         5
#define ANIM_STD_GO_WEST          6
#define ANIM_STD_GO_EAST          7
#define ANIM_STD_GO_FAST_SOUTH    8
#define ANIM_STD_GO_FAST_NORTH    9
#define ANIM_STD_GO_FAST_WEST     10
#define ANIM_STD_GO_FAST_EAST     11
#define ANIM_STD_GO_FASTER_SOUTH  12
#define ANIM_STD_GO_FASTER_NORTH  13
#define ANIM_STD_GO_FASTER_WEST   14
#define ANIM_STD_GO_FASTER_EAST   15
#define ANIM_STD_GO_FASTEST_SOUTH 16
#define ANIM_STD_GO_FASTEST_NORTH 17
#define ANIM_STD_GO_FASTEST_WEST  18
#define ANIM_STD_GO_FASTEST_EAST  19
#define ANIM_RAISE_HAND           20

#define MON_OW_TEMPLATE_32x32_FRAMES(Id, name)                        \
static const struct SpriteFrameImage sMonPicTable_##name[] =          \
{                                                                     \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 0),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 1),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 2),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 3),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 4),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 5),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 6),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 7),    \
    overworld_frame(gFollowerMonPic##Id##_##name##Tiles, 4, 4, 8),    \
};

#define MON_OW_OBJECT_GRAPHICS(PalId, name)                 \
{                                                           \
    .tileTag = 0xFFFF,                                      \
    .paletteTag1 = 0x##PalId,                               \
    .paletteTag2 = EVENT_OBJ_PAL_TAG_NONE,                  \
    .size = (32 * 32) / 2,                                  \
    .width = 32,                                            \
    .height = 32,                                           \
    .shadowSize = SHADOW_SIZE_M,                            \
    .inanimate = FALSE,                                     \
    .disableReflectionPaletteLoad = FALSE,                  \
    .tracks = TRACKS_FOOT,                                  \
    .gender = MALE,                                         \
    .oam = gEventObjectBaseOam_32x32,                       \
    .subspriteTables = gEventObjectSpriteOamTables_32x32,   \
    .anims = gFollowerMonAnimTable,                         \
    .images = sMonPicTable_##name,                          \
    .affineAnims = gDummySpriteAffineAnimTable,             \
},

extern const u16 gFollowerMonSpriteIdTable[];
extern const u16 gFollowerMonShinySpriteIdTable[];
extern const union AnimCmd *const gFollowerMonAnimTable[];

// Overworld Tables
extern const struct EventObjectGraphicsInfo gFollowerMonGfxTable0[];
extern const struct EventObjectGraphicsInfo gFollowerMonGfxTable1[];

// Images declarations
//Gen 1
extern const u8 gFollowerMonPic0001_BulbasaurTiles[];
extern const u8 gFollowerMonPic0002_IvysaurTiles[];
extern const u8 gFollowerMonPic0003_VenusaurTiles[];
extern const u8 gFollowerMonPic0004_CharmanderTiles[];
extern const u8 gFollowerMonPic0005_CharmeleonTiles[];
extern const u8 gFollowerMonPic0006_CharizardTiles[];
extern const u8 gFollowerMonPic0007_SquirtleTiles[];
extern const u8 gFollowerMonPic0008_WartortleTiles[];
extern const u8 gFollowerMonPic0009_BlastoiseTiles[];

//Shiny
extern const u8 gFollowerMonPic2001_BulbasaurShinyTiles[];
extern const u8 gFollowerMonPic2002_IvysaurShinyTiles[];
extern const u8 gFollowerMonPic2003_VenusaurShinyTiles[];
extern const u8 gFollowerMonPic2004_CharmanderShinyTiles[];
extern const u8 gFollowerMonPic2005_CharmeleonShinyTiles[];
extern const u8 gFollowerMonPic2006_CharizardShinyTiles[];
extern const u8 gFollowerMonPic2007_SquirtleShinyTiles[];
extern const u8 gFollowerMonPic2008_WartortleShinyTiles[];
extern const u8 gFollowerMonPic2009_BlastoiseShinyTiles[];


// Palettes declarations
// Gen 1
extern const u16 gFollowerMonPic0001_BulbasaurPal[];
extern const u16 gFollowerMonPic0002_IvysaurPal[];
extern const u16 gFollowerMonPic0003_VenusaurPal[];
extern const u16 gFollowerMonPic0004_CharmanderPal[];
extern const u16 gFollowerMonPic0005_CharmeleonPal[];
extern const u16 gFollowerMonPic0006_CharizardPal[];
extern const u16 gFollowerMonPic0007_SquirtlePal[];
extern const u16 gFollowerMonPic0008_WartortlePal[];
extern const u16 gFollowerMonPic0009_BlastoisePal[];

//Shiny
extern const u16 gFollowerMonPic2001_BulbasaurShinyPal[];
extern const u16 gFollowerMonPic2002_IvysaurShinyPal[];
extern const u16 gFollowerMonPic2003_VenusaurShinyPal[];
extern const u16 gFollowerMonPic2004_CharmanderShinyPal[];
extern const u16 gFollowerMonPic2005_CharmeleonShinyPal[];
extern const u16 gFollowerMonPic2006_CharizardShinyPal[];
extern const u16 gFollowerMonPic2007_SquirtleShinyPal[];
extern const u16 gFollowerMonPic2008_WartortleShinyPal[];
extern const u16 gFollowerMonPic2009_BlastoiseShinyPal[];


#endif