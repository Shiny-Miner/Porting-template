#include "../include/global.h"
#include "../include/gflib.h"
#include "../include/data.h"
#include "../include/keyboard_text.h"
#include "../include/event_scripts.h"
#include "../include/graphics.h" 
#include "../include/help_system.h"
#include "../include/naming_screen.h"
#include "../include/palette.h"
#include "../include/task.h"
#include "../include/sprite.h"
#include "../include/string_util.h"
#include "../include/window.h"
#include "../include/bg.h"
#include "../include/gpu_regs.h"
#include "../include/pokemon.h"
#include "../include/field_specials.h"
#include "../include/field_player_avatar.h"
#include "../include/event_object_movement.h"
#include "../include/event_data.h"
#include "../include/new_menu_helpers.h"
#include "../include/pokemon_icon.h"
#include "../include/pokemon_storage_system.h"
#include "../include/sound.h"
#include "../include/scanline_effect.h"
#include "../include/trig.h"
#include "../include/field_effect.h"
#include "../include/pokemon_icon.h"
#include "../include/strings.h"
#include "../include/menu.h"
#include "../include/text_window.h"
#include "../include/text_window_graphics.h"
#include "../include/overworld.h"
#include "../include/constants/vars.h"
#include "../include/constants/songs.h"
#include "../include/constants/help_system.h"
#include "../include/constants/flags.h"
#include "../include/constants/event_objects.h"

#define subsprite_table(ptr) {.subsprites = ptr, .subspriteCount = (sizeof ptr) / (sizeof(struct Subsprite))}

#define COLUMN_COUNT 12
#define FILE_COUNT 4
#define OT_NAME_LENGTH 10
#define KBEVENT_NONE 0
#define KBEVENT_PRESSED_A 5
#define KBEVENT_PRESSED_B 6
#define KBEVENT_PRESSED_SELECT 8
#define KBEVENT_PRESSED_START 9

#define KBROW_COUNT 4


enum
{
    KBPAGE_LETTERS_UPPER,
    KBPAGE_LETTERS_LOWER,
    KBPAGE_SYMBOLS,
    KBPAGE_COUNT,
};

enum
{
    MAIN_STATE_BEGIN_FADE_IN,
    MAIN_STATE_WAIT_FADE_IN,
    MAIN_STATE_HANDLE_INPUT,
    MAIN_STATE_MOVE_TO_OK_BUTTON,
    MAIN_STATE_START_PAGE_SWAP,
    MAIN_STATE_WAIT_PAGE_SWAP,
    MAIN_STATE_6,
    MAIN_STATE_UPDATE_SENT_TO_PC_MESSAGE,
    MAIN_STATE_BEGIN_FADE_OUT,
    MAIN_STATE_WAIT_FADE_OUT_AND_EXIT,
};

enum
{
    INPUT_STATE_DISABLED,
    INPUT_STATE_ENABLED,
};

enum
{
    KEY_ROLE_CHAR,
    KEY_ROLE_PAGE_UPPER,
    KEY_ROLE_PAGE_LOWER,
    KEY_ROLE_PAGE_OTHER,
    KEY_ROLE_BACKSPACE,
    KEY_ROLE_OK,
};


struct NamingScreenTemplate
{
    u8 copyExistingString;
    u8 maxChars;
    u8 iconFunction;
    u8 addGenderIcon;
    u8 initialPage;
    const u8 *title;
    const u8 *titleSpa;
};

struct NamingScreenData
{
	u8 textBuffer[0x10];
    u8 state;
    u8 windows[5];
    u16 inputCharBaseXPos;
    u16 bg2vOffset;
    u8 bldAlphaTg;
    u8 currentPage;
    u8 cursorSpriteId;
    u8 selectBtnFrameSpriteId;
    u8 keyRepeatStartDelayCopy;
    const struct NamingScreenTemplate *template;
    u8 templateNum;
    u8 *destBuffer;
    u16 monSpeciesOrPlayerGender;
    u16 monGender;
    u32 monPersonality;
    MainCallback returnCallback;
};

extern EWRAM_DATA struct NamingScreenData * sNamingScreenData;
void CB2_NamingScreen(void);
static void NamingScreen_Init(void);
static void NamingScreen_InitBGs(void);
static void NamingScreen_ResetObjectsAndTasks(void);
static void NamingScreen_LoadBGsTilesAndMap(void);
static void InitNamingScreen(void);
static void sub_809DD88(u8 taskId);
static bool8 MainState_BeginFadeIn(void);
static bool8 MainState_WaitFadeIn(void);
static bool8 MainState_HandleInput(void);
static bool8 MainState_MoveToOKButton(void);
static bool8 MainState_PokemonStore(void);
static bool8 MainState_BeginFadeInOut(void);
static bool8 MainState_WaitFadeOutAndExit(void);
static void NamingScreen_PkmnTransferToPCMsj(void);
static bool8 MainState_ExitOfPokemonStore(void);
static bool8 MainState_StartPageSwap(void);
static bool8 MainState_WaitPageSwap(void);
static void StartPageSwapAnim(void);
static void Task_HandlePageSwapAnim(u8 taskId);
static bool8 IsPageSwapAnimNotInProgress(void);
static bool8 PageSwapAnimState_Init(struct Task * task);
static bool8 PageSwapAnimState_1(struct Task * task);
static bool8 PageSwapAnimState_2(struct Task * task);
static bool8 PageSwapAnimState_3(struct Task *task);
static bool8 PageSwapAnimState_Done(struct Task * task);
static void Callback_Cursor(struct Sprite * sprite);
static void Callback_UnderscoreMain(struct Sprite * sprite);
static void Callback_Underscore(struct Sprite * sprite);
static void NamingScreen_CreateObjects(void);
static void CursorInit(void);
static void SetCursorPos(s16 x, s16 y);
static void GetCursorPos(s16 *xP, s16 *yP);
static void MoveCursorToOKButton(void);
static void MoveCursorToBackButton(void);
static void sub_809EA0C(u8 a0);
static void sub_809EA64(u8 a0);
static bool8 IsCursorAnimFinished(void);
static void CreateUnderscoreSprites(void);
static void CreateInputTargetIcon(void);
static void NamingScreen_NoCreateIcon(void);
static void NamingScreen_CreatePlayerIcon(void);
static void NamingScreen_CreatePCIcon(void);
static void NamingScreen_CreateMonIcon(void);
static void NamingScreen_CreateRivalIcon(void);
static bool8 HandleKeyboardEvent(void);
static bool8 TriggerKeyboardChange(void);
static u8 GetInputEvent(void);
static void SetInputState(u8 state);
static void Task_HandleInput(u8 taskId);
static void InputState_Disabled(struct Task * task);
static void InputState_Enabled(struct Task * task);
static void InputState_2(struct Task *);
static void HandleDpadMovement(struct Task * task);
static void PrintTitle(void);
static void AddGenderIconFunc_No(void);
static void AddGenderIconFunc_Yes(void);
static void DeleteTextCharacter(void);
static u8 GetTextCaretPosition(void);
static bool8 AppendCharToBuffer_CheckBufferFull(void);
static void AddTextCharacter(u8 character);
static void CopyStringToDestBuffer(void);
static void NamingScreen_LoadObjectsGfx(void);
static void NamingScreen_LoadBGsPal(void);
static void PrintBufferCharactersOnScreen(void);
static void NamingScreen_Callback(void);
static void NamingScreen_TurnOffScreen(void);
static void NamingScreen_InitDisplayMode(void);
static void VBlankCB_NamingScreen(void);
static void NamingScreen_ShowAllBgs(void);
static void InputInit(void);
static bool8 IsLetter(u8 character);
static void NamingScreen_RedrawOptions(void);
static bool8 KeyboardKeyHandler_Character(u8);
static bool8 KeyboardKeyHandler_Upper(u8);
static bool8 KeyboardKeyHandler_Lower(u8);
static bool8 KeyboardKeyHandler_Other(u8);
static bool8 KeyboardKeyHandler_Backspace(u8);
static bool8 KeyboardKeyHandler_OK(u8);
static void NamingScreen_DrawDialogueFrame(void);
static const u8 sNamingScreenTextColor[] = { 0, 1, 2 };

// Forward declarations

static const struct SubspriteTable SubspriteTable_PcIcon[];

static const struct SpriteTemplate gSpriteTemplate_Cursor;
static const struct SpriteTemplate gSpriteTemplate_UnderscoreMain;
static const struct SpriteTemplate sSpriteTemplate_Underscore;
static const struct SpriteTemplate gSpriteTemplate_PCIcon;

static const struct SpriteSheet sNamingScreenSpriteSheet[];
static const struct SpritePalette sNamingScreenSpritePalette[];

static const u8 *const sTransferredToPCMessages[] = {
    gText_MonSentToBoxInSomeonesPC,
    gText_MonSentToBoxInBillsPC,
    gText_MonSentToBoxSomeonesBoxFull,
    gText_MonSentToBoxBillsBoxFull,
   /* gText_MonSentToBoxInSomeonesPC,
    gText_MonSentToBoxInBillsPC,
    gText_MonSentToBoxSomeonesBoxFull,
    gText_MonSentToBoxBillsBoxFull*/
    
};

void ResetBg0(void)
{
    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    DeactivateAllTextPrinters();
    LoadStdWindowFrameGfx();
}

static const struct BgTemplate NamingScreenBgTemplate[] = {
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    }, {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    }, {
        .bg = 2,
        .charBaseIndex = 1,
        .mapBaseIndex = 27,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    }, {
        .bg = 3,
        .charBaseIndex = 1,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    }
};

static const struct WindowTemplate NamingScreenWindowTemplate[] = {
    {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 10,
        .width = 19,
        .height = 8,
        .paletteNum = 10,
        .baseBlock = 0
    }, {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 10,
        .width = 19,
        .height = 8,
        .paletteNum = 10,
        .baseBlock = 152
    }, {//Input
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 4,
        .width = 14,
        .height = 2,
        .paletteNum = 10,
        .baseBlock = 304
    }, {//Title
        .bg = 0,
        .tilemapLeft = 9,
        .tilemapTop = 2,
        .width = 16,
        .height = 2,
        .paletteNum = 10,
        .baseBlock = 332
    }, {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 11,
        .baseBlock = 364
    }, DUMMY_WIN_TEMPLATE
};

static const u8 gNamingScreenChars[][3][12] = {
    [KBPAGE_LETTERS_LOWER] = {
        { 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xAD,  },
        { 0xE0, 0xE1, 0xE2, 0x29, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xB8,  },
        { 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0x17, 0x1B, 0x6F, 0x23, 0x27, 0xF6, 0x00,  },
    },
    [KBPAGE_LETTERS_UPPER] = {
        { 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xAD,  },
        { 0xC6, 0xC7, 0xC8, 0x14, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xB8,  },
        { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x02, 0x06, 0x5A, 0x0E, 0x12, 0xF3, 0x00,  },
    },
    [KBPAGE_SYMBOLS] = {
        { 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0x2E, 0xAE,  },
        { 0xB0, 0xAF, 0x5B, 0x5C, 0x5D, 0xF0, 0x36, 0x35, 0x52, 0xAB, 0x51, 0xAC,  },
        { 0xB1, 0xB2, 0xB3, 0xB4, 0xBA, 0xB5, 0xB6, 0x00, 0x00, 0x00, 0x00, 0x00,  },
    }
};

static const struct NamingScreenTemplate *const sNamingScreenTemplates[];

void DoNamingScreen(u8 templateNum, u8 *destBuffer, u16 monSpeciesOrPlayerGender, u16 monGender, u32 monPersonality, MainCallback returnCallback)
{
    sNamingScreenData = Alloc(sizeof(struct NamingScreenData));
    if (!sNamingScreenData)
    {
        SetMainCallback2(returnCallback);
    }
    else
    {
        sNamingScreenData->templateNum = templateNum;
        sNamingScreenData->monSpeciesOrPlayerGender = monSpeciesOrPlayerGender;
        sNamingScreenData->monGender = monGender;
        sNamingScreenData->monPersonality = monPersonality;
        sNamingScreenData->destBuffer = destBuffer;
        sNamingScreenData->returnCallback = returnCallback;

        if (templateNum == 0)
            StartTimer1();

        SetMainCallback2(CB2_NamingScreen);
    }
}

void CB2_NamingScreen(void)
{
    switch (gMain.state)
    {
    case 0:
        NamingScreen_TurnOffScreen();
        NamingScreen_Init();
        gMain.state++;
        break;
    case 1:
        NamingScreen_InitBGs();
        gMain.state++;
        break;
    case 2:
		NamingScreen_ResetObjectsAndTasks();
        gMain.state++;
        break;
    case 3:
		NamingScreen_LoadBGsTilesAndMap();
        gMain.state++;
        break;
    case 4:
        NamingScreen_LoadBGsPal();
        gMain.state++;
        break;
    case 5:
        NamingScreen_LoadObjectsGfx();
        gMain.state++;
        break;
    case 6:
        NamingScreen_CreateObjects();
        UpdatePaletteFade();
        NamingScreen_ShowAllBgs();
        gMain.state++;
        break;
    default:
        InputInit();
        InitNamingScreen();
        break;
    }
}

static void NamingScreen_Init(void)
{
    sNamingScreenData->state = 0;
    sNamingScreenData->bg2vOffset = 0;
    sNamingScreenData->bldAlphaTg = 16;
    sNamingScreenData->template = sNamingScreenTemplates[sNamingScreenData->templateNum];
    sNamingScreenData->currentPage = sNamingScreenData->template->initialPage;
    sNamingScreenData->inputCharBaseXPos = (240 - sNamingScreenData->template->maxChars * 8) / 2 + 6;
    sNamingScreenData->keyRepeatStartDelayCopy = gKeyRepeatStartDelay;
    memset(sNamingScreenData->textBuffer, 0xFF, sizeof(sNamingScreenData->textBuffer));
    if (sNamingScreenData->template->copyExistingString != 0)
        StringCopy(sNamingScreenData->textBuffer, sNamingScreenData->destBuffer);
    gKeyRepeatStartDelay = 16;
}

static void sub_809DB70(void)
{
    u8 i;
    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse)
            gSprites[i].invisible = FALSE;
    }
    sub_809EA0C(0);
}

static void NamingScreen_InitBGs(void)
{
    u8 i;

    DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
    DmaClear32(3, (void *)OAM, OAM_SIZE);
    DmaClear16(3, (void *)PLTT, PLTT_SIZE);

    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
    ResetBgsAndClearDma3BusyFlags(FALSE);
    InitBgsFromTemplates(0, NamingScreenBgTemplate, NELEMS(NamingScreenBgTemplate));

    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    ChangeBgX(1, 0, 0);
    ChangeBgY(1, 0, 0);
    ChangeBgX(2, 0, 0);
    ChangeBgY(2, 0, 0);
    ChangeBgX(3, 0, 0);
    ChangeBgY(3, 0, 0);

    InitStandardTextBoxWindows();
    ResetBg0();

    for (i = 0; i < NELEMS(NamingScreenWindowTemplate) - 1; i++)
        sNamingScreenData->windows[i] = AddWindow(&NamingScreenWindowTemplate[i]);

    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_BG2 | BLDCNT_TGT2_BG3);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
}

static void NamingScreen_ResetObjectsAndTasks(void)
{
	ResetPaletteFade();
	ResetSpriteData();
	FreeAllSpritePalettes();
	ResetTasks();
}

static void NamingScreen_LoadBGsTilesAndMap(void)
{
    LZ77UnCompVram(gNamingScreenBGTiles,    	((void *)BG_VRAM + 0x4000));
    LZ77UnCompVram(gNamingScreenBGMap,			((void *)BG_VRAM + 0xE000));
    LZ77UnCompVram(gNamingScreenCharUpperMap,   ((void *)BG_VRAM + 0xD800 + 0x200));
	//if (gSaveBlock2Ptr->optionsLanguage == ENG)
	LZ77UnCompVram(gNamingScreenButtonsEng,	((void *)BG_VRAM + 0xE000 + 0x440));
	//if (gSaveBlock2Ptr->optionsLanguage == SPA)
		//LZ77UnCompVram(gNamingScreenButtonsSpa,	((void *)BG_VRAM + 0xE000 + 0x440));
}

static void InitNamingScreen(void)
{
    CreateTask(sub_809DD88, 2);
    SetMainCallback2(NamingScreen_Callback);
    BackupHelpContext();
    SetHelpContext(HELPCONTEXT_NAMING_SCREEN);
}

static void sub_809DD88(u8 taskId)
{
    switch (sNamingScreenData->state)
    {
    case MAIN_STATE_BEGIN_FADE_IN:
        MainState_BeginFadeIn();
        sub_809DB70();
        NamingScreen_InitDisplayMode();
        break;
    case MAIN_STATE_WAIT_FADE_IN:
        MainState_WaitFadeIn();
        break;
    case MAIN_STATE_HANDLE_INPUT:
        MainState_HandleInput();
        break;
    case MAIN_STATE_MOVE_TO_OK_BUTTON:
        MainState_MoveToOKButton();
        break;
    case MAIN_STATE_START_PAGE_SWAP:
        MainState_StartPageSwap();
        break;
    case MAIN_STATE_WAIT_PAGE_SWAP:
        MainState_WaitPageSwap();
        break;
    case MAIN_STATE_6:
        MainState_PokemonStore();
        break;
    case MAIN_STATE_UPDATE_SENT_TO_PC_MESSAGE:
        MainState_ExitOfPokemonStore();
        break;
    case MAIN_STATE_BEGIN_FADE_OUT:
        MainState_BeginFadeInOut();
        break;
    case MAIN_STATE_WAIT_FADE_OUT_AND_EXIT:
        MainState_WaitFadeOutAndExit();
        break;
    }
}

static bool8 MainState_BeginFadeIn(void)
{
    sNamingScreenData->currentPage = KBPAGE_LETTERS_UPPER;
	NamingScreen_RedrawOptions();
    PrintBufferCharactersOnScreen();
    PrintTitle();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
    sNamingScreenData->state++;
    return FALSE;
}

static bool8 MainState_WaitFadeIn(void)
{
    if (!gPaletteFade.active)
    {
        SetInputState(INPUT_STATE_ENABLED);
        sub_809EA64(1);
        sNamingScreenData->state++;
    }
    return FALSE;
}

static bool8 MainState_HandleInput(void)
{
    return HandleKeyboardEvent();
}

static bool8 MainState_MoveToOKButton(void)
{
    if (IsCursorAnimFinished())
    {
        SetInputState(INPUT_STATE_ENABLED);
        MoveCursorToOKButton();
        sNamingScreenData->state = MAIN_STATE_HANDLE_INPUT;
    }
    return FALSE;
}

static bool8 MainState_PokemonStore(void)
{
    CopyStringToDestBuffer();
    SetInputState(INPUT_STATE_DISABLED);
    sub_809EA64(0);
    if (sNamingScreenData->templateNum == NAMING_SCREEN_CAUGHT_MON &&
        CalculatePlayerPartyCount() >= 6)
    {
        NamingScreen_PkmnTransferToPCMsj();
        sNamingScreenData->state = MAIN_STATE_UPDATE_SENT_TO_PC_MESSAGE;
        return FALSE;
    }
    else
    {
        sNamingScreenData->state = MAIN_STATE_BEGIN_FADE_OUT;
        return TRUE;  //Exit the naming screen
    }
}

static bool8 MainState_BeginFadeInOut(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
    sNamingScreenData->state++;
    return FALSE;
}

static bool8 MainState_WaitFadeOutAndExit(void)
{
    if (!gPaletteFade.active)
    {
        if (sNamingScreenData->templateNum == NAMING_SCREEN_PLAYER)
            SeedRngAndSetTrainerId();
        SetMainCallback2(sNamingScreenData->returnCallback);
        DestroyTask(FindTaskIdByFunc(sub_809DD88));
        FreeAllWindowBuffers();
        FREE_AND_SET_NULL(sNamingScreenData);
        RestoreHelpContext();
    }
    return FALSE;
}

static void NamingScreen_PkmnTransferToPCMsj(void)
{
    u8 stringToDisplay = 0;

    if (!IsDestinationBoxFull())
    {
        StringCopy(gStringVar1, GetBoxNamePtr(VarGet(VAR_PC_BOX_TO_SEND_MON)));
        StringCopy(gStringVar2, sNamingScreenData->destBuffer);
    }
    else
    {
        StringCopy(gStringVar1, GetBoxNamePtr(VarGet(VAR_PC_BOX_TO_SEND_MON)));
        StringCopy(gStringVar2, sNamingScreenData->destBuffer);
        StringCopy(gStringVar3, GetBoxNamePtr(GetPCBoxToSendMon()));
        stringToDisplay = 2;
    }

    if (FlagGet(FLAG_SYS_NOT_SOMEONES_PC))
        stringToDisplay++;

    //if (gSaveBlock2Ptr->optionsLanguage == ENG)
	StringExpandPlaceholders(gStringVar4, sTransferredToPCMessages[stringToDisplay]);
    //if (gSaveBlock2Ptr->optionsLanguage == SPA)
	//	StringExpandPlaceholders(gStringVar4, sTransferredToPCMessages[stringToDisplay + 4]);
    NamingScreen_DrawDialogueFrame();
    //TextFlags.canABSpeedUpPrint = TRUE;
    AddTextPrinterParameterized2(0, 2, gStringVar4, GetTextSpeedSetting(), NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
    CopyWindowToVram(0, COPYWIN_BOTH);
}

static bool8 MainState_ExitOfPokemonStore(void)
{
    RunTextPrinters();

    if (!IsTextPrinterActive(0) && (JOY_NEW(A_BUTTON)))
        sNamingScreenData->state = MAIN_STATE_BEGIN_FADE_OUT;

    return FALSE;
}

static bool8 MainState_StartPageSwap(void)
{
    SetInputState(INPUT_STATE_DISABLED);
    StartPageSwapAnim();
    sub_809EA0C(1);
    PlaySE(SE_WIN_OPEN);
    sNamingScreenData->state = MAIN_STATE_WAIT_PAGE_SWAP;
    return FALSE;
}

static bool8 MainState_WaitPageSwap(void)
{
    s16 cursorX;
    s16 cursorY;

    if (IsPageSwapAnimNotInProgress())
    {
        sNamingScreenData->state = MAIN_STATE_HANDLE_INPUT;
        sNamingScreenData->currentPage++;
        sNamingScreenData->currentPage %= 3;
        SetInputState(INPUT_STATE_ENABLED);
        GetCursorPos(&cursorX, &cursorY);
        SetCursorPos(cursorX, cursorY);
        sub_809EA0C(0);
    }
    return FALSE;
}

//--------------------------------------------------
// Page Swap
//--------------------------------------------------

#define tState data[0]
#define tFrameCount data[1]

static bool8 (*const sPageSwapAnimStateFuncs[])(struct Task * task) = {
    PageSwapAnimState_Init,
    PageSwapAnimState_1,
    PageSwapAnimState_2,
    PageSwapAnimState_3,
    PageSwapAnimState_Done
};

static void StartPageSwapAnim(void)
{
    u8 taskId;

    taskId = CreateTask(Task_HandlePageSwapAnim, 0);
    Task_HandlePageSwapAnim(taskId);
}

static void Task_HandlePageSwapAnim(u8 taskId)
{
    while (sPageSwapAnimStateFuncs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 IsPageSwapAnimNotInProgress(void)
{
    if (FindTaskIdByFunc(Task_HandlePageSwapAnim) == 0xFF)
        return TRUE;
    else
        return FALSE;
}

static bool8 PageSwapAnimState_Init(struct Task *task)
{
    sNamingScreenData->bldAlphaTg = 16;
    sNamingScreenData->bg2vOffset = 0;
    task->tState++;
    return FALSE;
}

static bool8 PageSwapAnimState_1(struct Task *task)
{
    u8 a = sNamingScreenData->bldAlphaTg;
	u8 b = 16;
    u8 c = sNamingScreenData->bg2vOffset;
	task->tFrameCount += 1;
	SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(a, b)); 
	sNamingScreenData->bldAlphaTg = a - 1;
	sNamingScreenData->bg2vOffset = c - 2;
	if ((a - 1) == 0)
    {
		task->tState++;
    }
    return FALSE;
}

static bool8 PageSwapAnimState_2(struct Task *task)
{
	u8 prevPage = sNamingScreenData->currentPage;
	
	LoadPalette(gNamingScreenPal, 0, 0xE0);
	
	if (prevPage == KBPAGE_LETTERS_UPPER)
	{
		LoadPalette(gNamingScreenSelPal, 0x20, 0x20);
		LZ77UnCompVram(gNamingScreenCharLowerMap,   ((void *)BG_VRAM + 0xD800 + 0x200));
	}
	if (prevPage == KBPAGE_LETTERS_LOWER)
	{
		LoadPalette(gNamingScreenSelPal, 0x30, 0x20);
		LZ77UnCompVram(gNamingScreenCharOtherMap,   ((void *)BG_VRAM + 0xD800 + 0x200));
	}
	if (prevPage == KBPAGE_SYMBOLS)
	{
		LoadPalette(gNamingScreenSelPal, 0x10, 0x20);
		LZ77UnCompVram(gNamingScreenCharUpperMap,   ((void *)BG_VRAM + 0xD800 + 0x200));
	}	
	task->tState++;
    return FALSE;
}

static bool8 PageSwapAnimState_3(struct Task *task)
{
	u8 a = sNamingScreenData->bldAlphaTg;
	u8 b = 16;
	task->tFrameCount += 1;
	SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(a, b));
	++sNamingScreenData->bldAlphaTg;
	sNamingScreenData->bg2vOffset += 2;
	if ((a + 1) == 16)
    {
		SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
		task->tState++;
    }
    return FALSE;
}

static bool8 PageSwapAnimState_Done(struct Task *task)
{
    DestroyTask(FindTaskIdByFunc(Task_HandlePageSwapAnim));
    return 0;
}

#undef tState
#undef tFrameCount

//--------------------------------------------------
// Cursor blink
//--------------------------------------------------

#define tIdent data[0]


//--------------------------------------------------
// Cursor
//--------------------------------------------------

static void Callback_Cursor(struct Sprite *sprite)
{
	if (sprite->animEnded)
        StartSpriteAnim(sprite, 0);
    sprite->invisible = (sprite->data[4] & 0xFF);
    if (sprite->data[1] == FILE_COUNT - 1)
        sprite->invisible = TRUE;
    if (sprite->invisible || (sprite->data[4] & 0xFF00) == 0
     || sprite->data[0] != sprite->data[2] || sprite->data[1] != sprite->data[3])
    {
        sprite->data[5] = 0;
        sprite->data[6] = 1;
        sprite->data[7] = 2;
    }
    sprite->data[7]--;
    if (sprite->data[7] == 0)
    {
        sprite->data[5] += sprite->data[6];
        if (sprite->data[5] == 16 || sprite->data[5] == 0)
            sprite->data[6] = -sprite->data[6];
        sprite->data[7] = 2;
    }
    if ((sprite->data[4] & 0xFF00) != 0)
    {
        s8 gb = sprite->data[5];
        s8 r = sprite->data[5] >> 1;
        u16 index = IndexOfSpritePaletteTag(5) * 16 + 0x0101;

        MultiplyInvertedPaletteRGBComponents(index, r, gb, gb);
    }
}

static void Callback_UnderscoreMain(struct Sprite *sprite)
{
    const s16 arr[] = {0, -4, -2, -1};

    if (sprite->data[0] == 0 || --sprite->data[0] == 0)
    {
        sprite->data[0] = 8;
        sprite->data[1] = (sprite->data[1] + 1) & 3;
    }
    sprite->pos2.x = arr[sprite->data[1]];
}

static void Callback_Underscore(struct Sprite *sprite)
{
    const s16 arr[] = {2, 3, 2, 1};
    u8 var;

    var = GetTextCaretPosition();
    if (var != (u8)sprite->data[0])
    {
        sprite->pos2.y = 0;
        sprite->data[1] = 0;
        sprite->data[2] = 0;
    }
    else
    {
        sprite->pos2.y = arr[sprite->data[1]];
        sprite->data[2]++;
        if (sprite->data[2] > 8)
        {
            sprite->data[1] = (sprite->data[1] + 1) & 3;
            sprite->data[2] = 0;
        }
    }
}

static void NamingScreen_CreateObjects(void)
{
    CursorInit();
    CreateUnderscoreSprites();
    CreateInputTargetIcon();
}

static void CursorInit(void)
{
    sNamingScreenData->cursorSpriteId = CreateSprite(&gSpriteTemplate_Cursor, 38, 88, 1);
    sub_809EA0C(1);
    gSprites[sNamingScreenData->cursorSpriteId].oam.priority = 1;
    gSprites[sNamingScreenData->cursorSpriteId].oam.objMode = ST_OAM_OBJ_BLEND;
    gSprites[sNamingScreenData->cursorSpriteId].data[6] = 1;
    gSprites[sNamingScreenData->cursorSpriteId].data[6] = 2;
    SetCursorPos(0, 0);
}

static const u8 sKeyboardSymbolPositions[][COLUMN_COUNT] = { // Coordenadas de los caracteres
    {0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22},  //Upper page
    {0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22},  //Lower page
    {0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22},  //Others page
};

static u8 CursorColToKeyboardCol(s16 x)
{
    return sKeyboardSymbolPositions[sNamingScreenData->currentPage][x];
}

static void SetCursorPos(s16 x, s16 y)
{
    struct Sprite *cursorSprite = &gSprites[sNamingScreenData->cursorSpriteId];

    cursorSprite->pos1.x = CursorColToKeyboardCol(x) * 8 + 28;
    cursorSprite->pos1.y = y * 24 + 80;
    cursorSprite->data[2] = cursorSprite->data[0];
    cursorSprite->data[3] = cursorSprite->data[1];
    cursorSprite->data[0] = x;
    cursorSprite->data[1] = y;
}

static void GetCursorPos(s16 *x, s16 *y)
{
    struct Sprite *cursorSprite = &gSprites[sNamingScreenData->cursorSpriteId];

    *x = cursorSprite->data[0];
    *y = cursorSprite->data[1];
}

static void MoveCursorToOKButton(void)
{
	LoadPalette(gNamingScreenPal, 0, 0xE0);
	LoadPalette(gNamingScreenSelPal, 0x50, 0x20);
	SetCursorPos(4, FILE_COUNT - 1);
}

static void MoveCursorToBackButton(void)
{
	LoadPalette(gNamingScreenPal, 0, 0xE0);
	LoadPalette(gNamingScreenSelPal, 0x40, 0x20);
	SetCursorPos(3, FILE_COUNT - 1);
}

static void sub_809EA0C(u8 a)
{
    gSprites[sNamingScreenData->cursorSpriteId].data[4] &= ~0xFF;
    gSprites[sNamingScreenData->cursorSpriteId].data[4] |= a;
    StartSpriteAnim(&gSprites[sNamingScreenData->cursorSpriteId], 0);
}

static void sub_809EA64(u8 a)
{
    gSprites[sNamingScreenData->cursorSpriteId].data[4] &= 0xFF;
    gSprites[sNamingScreenData->cursorSpriteId].data[4] |= a << 8;
}

static void sub_809EAA8(void)
{
    StartSpriteAnim(&gSprites[sNamingScreenData->cursorSpriteId], 1);
}

static bool8 IsCursorAnimFinished(void)
{
    return gSprites[sNamingScreenData->cursorSpriteId].animEnded;
}

const u8 keyRoles[] = {KEY_ROLE_PAGE_UPPER, KEY_ROLE_PAGE_LOWER, KEY_ROLE_PAGE_OTHER, KEY_ROLE_BACKSPACE, KEY_ROLE_OK};

static u8 GetKeyRoleAtCursorPos(void)
{
    s16 cursorX;
    s16 cursorY;

    GetCursorPos(&cursorX, &cursorY);
    if (cursorY < FILE_COUNT - 1)
        return KEY_ROLE_CHAR;
    else
        return keyRoles[cursorX];
}

static void CreateUnderscoreSprites(void)
{
    u8 spriteId;
    s16 xPos;
    u8 i;

    xPos = sNamingScreenData->inputCharBaseXPos - 5;
    spriteId = CreateSprite(&gSpriteTemplate_UnderscoreMain, xPos, 0x28, 0);
    gSprites[spriteId].oam.priority = 3;
    gSprites[spriteId].invisible = TRUE;
    xPos = sNamingScreenData->inputCharBaseXPos;
    for (i = 0; i < sNamingScreenData->template->maxChars; i++, xPos += 8)
    {
        spriteId = CreateSprite(&sSpriteTemplate_Underscore, xPos + 3, 0x2C, 0);
        gSprites[spriteId].oam.priority = 3;
        gSprites[spriteId].data[0] = i;
        gSprites[spriteId].invisible = TRUE;
    }
}

//--------------------------------------------------
// Icon creation (the thing you're naming or giving input to)
//--------------------------------------------------

static void (*const sIconFunctions[])(void) = {
    NamingScreen_NoCreateIcon,
    NamingScreen_CreatePlayerIcon,
    NamingScreen_CreatePCIcon,
    NamingScreen_CreateMonIcon,
    NamingScreen_CreateRivalIcon
};

static void CreateInputTargetIcon(void)
{
    sIconFunctions[sNamingScreenData->template->iconFunction]();
}

static void NamingScreen_NoCreateIcon(void)
{

}

static void NamingScreen_CreatePlayerIcon(void)
{
    u8 rivalGfxId;
    u8 spriteId;

    rivalGfxId = GetRivalAvatarGraphicsIdByStateIdAndGender(0, sNamingScreenData->monSpeciesOrPlayerGender);
    spriteId = AddPseudoObjectEvent(rivalGfxId, SpriteCallbackDummy, 0x28, 0x1A, 0);
    gSprites[spriteId].oam.priority = 3;
    StartSpriteAnim(&gSprites[spriteId], 4);
}

static void NamingScreen_CreatePCIcon(void)
{
    u8 spriteId;

    spriteId = CreateSprite(&gSpriteTemplate_PCIcon, 0x28, 0x1A, 0);
    SetSubspriteTables(&gSprites[spriteId], SubspriteTable_PcIcon);
    gSprites[spriteId].oam.priority = 3;
}

static void NamingScreen_CreateMonIcon(void)
{
    u8 spriteId;

    LoadMonIconPalettes();
    spriteId = CreateMonIcon(sNamingScreenData->monSpeciesOrPlayerGender, SpriteCallbackDummy, 0x28, 0x1A, 0, sNamingScreenData->monPersonality, 1);
    gSprites[spriteId].oam.priority = 3;
}

static const union AnimCmd gUnknown_83E23A8[] = {
    ANIMCMD_FRAME( 0, 10),
    ANIMCMD_FRAME(24, 10),
    ANIMCMD_FRAME( 0, 10),
    ANIMCMD_FRAME(32, 10),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const gUnknown_83E23BC[] = {
    gUnknown_83E23A8
};

static void NamingScreen_CreateRivalIcon(void)
{
    const struct SpriteSheet sheet = {
        gRivalSpriteTiles, 0x900, 255
    };
    const struct SpritePalette palette = {
        gRivalSpritePalette, 255
    };
    struct SpriteTemplate template;
    const struct SubspriteTable * tables_p;
    u8 spriteId;

    MakeObjectTemplateFromObjectEventGraphicsInfo(OBJ_EVENT_GFX_RED_NORMAL, SpriteCallbackDummy, &template, &tables_p);

    template.tileTag = sheet.tag;
    template.paletteTag = palette.tag;
    template.anims = gUnknown_83E23BC;
    LoadSpriteSheet(&sheet);
    LoadSpritePalette(&palette);
    spriteId = CreateSprite(&template, 0x28, 0x1A, 0);
    gSprites[spriteId].oam.priority = 3;
}

static void NamingScreen_RedrawOptions(void)
{
	LoadPalette(gNamingScreenPal, 0x00, 0xD0);
	
	if (sNamingScreenData->currentPage == KBPAGE_LETTERS_LOWER)
		LoadPalette(gNamingScreenSelPal, 0x20, 0x20);
	if (sNamingScreenData->currentPage == KBPAGE_LETTERS_UPPER)
		LoadPalette(gNamingScreenSelPal, 0x10, 0x20);
	if (sNamingScreenData->currentPage == KBPAGE_SYMBOLS)
		LoadPalette(gNamingScreenSelPal, 0x30, 0x20);
}

static bool8 (*const sKeyboardKeyHandlers[])(u8) = {
    KeyboardKeyHandler_Character,
    KeyboardKeyHandler_Upper,
    KeyboardKeyHandler_Lower,
    KeyboardKeyHandler_Other,
    KeyboardKeyHandler_Backspace,
    KeyboardKeyHandler_OK,
};

static bool8 HandleKeyboardEvent(void)
{
    s16 cursorX;
    s16 cursor_X = 0;
    s16 cursorY;
    u8 event = GetInputEvent();
    u8 keyRole = GetKeyRoleAtCursorPos();

    if (event == KBEVENT_PRESSED_SELECT)
	{
		GetCursorPos(&cursorX, &cursorY);
	
		if (cursorY == FILE_COUNT - 1)
		{
			if (cursorX == 0)
				cursor_X = 1;
			else if (cursorX == 1)
				cursor_X = 2;
			else if (cursorX == 2)
				cursor_X = 0;
			else if (cursorX > 2)
				cursor_X = cursorX;
			SetCursorPos(cursor_X, cursorY);
		}
        return TriggerKeyboardChange();
	}
    else if (event == KBEVENT_PRESSED_B)
    {
		LoadPalette(gNamingScreenPal, 0, 0xE0);
		LoadPalette(gNamingScreenSelPal, 0x40, 0x20);
        DeleteTextCharacter();
        return FALSE;
    }
    else if (event == KBEVENT_PRESSED_START)
    {
        MoveCursorToOKButton();
        return FALSE;
    }
	
	return sKeyboardKeyHandlers[keyRole](event);
}

static bool8 KeyboardKeyHandler_Character(u8 event)
{
    if (event == KBEVENT_PRESSED_A)
    {
        bool8 var = AppendCharToBuffer_CheckBufferFull();

        sub_809EAA8();
        if (var)
        {
            SetInputState(INPUT_STATE_DISABLED);
            sNamingScreenData->state = MAIN_STATE_MOVE_TO_OK_BUTTON;
        }
		if (sNamingScreenData->currentPage == KBPAGE_LETTERS_UPPER && GetTextCaretPosition() == 1)
			sNamingScreenData->state = MAIN_STATE_START_PAGE_SWAP;
	}
    return FALSE;
}

static bool8 KeyboardKeyHandler_Upper(u8 event)
{
    NamingScreen_RedrawOptions();
	if (sNamingScreenData->currentPage == KBPAGE_LETTERS_UPPER)
		return FALSE;
	else
	{
		sNamingScreenData->currentPage = KBPAGE_SYMBOLS;
		TriggerKeyboardChange();
	}
    return FALSE;
}

static bool8 KeyboardKeyHandler_Lower(u8 event)
{
    NamingScreen_RedrawOptions();
	if (sNamingScreenData->currentPage == KBPAGE_LETTERS_LOWER)
		return FALSE;
	else
	{
		sNamingScreenData->currentPage = KBPAGE_LETTERS_UPPER;
		TriggerKeyboardChange();
	}
    return FALSE;
}

static bool8 KeyboardKeyHandler_Other(u8 event)
{
    NamingScreen_RedrawOptions();
	if (sNamingScreenData->currentPage == KBPAGE_SYMBOLS)
		return FALSE;
	else
	{
		sNamingScreenData->currentPage = KBPAGE_LETTERS_LOWER;
		TriggerKeyboardChange();
	}
    return FALSE;
}

static bool8 KeyboardKeyHandler_Backspace(u8 event)
{
	LoadPalette(gNamingScreenPal, 0, 0xE0);
	LoadPalette(gNamingScreenSelPal, 0x40, 0x20);
    if (event == KBEVENT_PRESSED_A)
        DeleteTextCharacter();
    return FALSE;
}

static bool8 KeyboardKeyHandler_OK(u8 event)
{
    MoveCursorToOKButton();
	if (event == KBEVENT_PRESSED_A)
    {
        PlaySE(SE_SELECT);
        sNamingScreenData->state = MAIN_STATE_6;
        return TRUE;
    }
    else
        return FALSE;
}

static bool8 TriggerKeyboardChange(void)
{
    sNamingScreenData->state = MAIN_STATE_START_PAGE_SWAP;
    return TRUE;
}

//--------------------------------------------------
// Input handling
//--------------------------------------------------

enum
{
    FNKEY_CASE_UPPER,
    FNKEY_CASE_LOWER,
    FNKEY_CASE_OTHER,
    FNKEY_BACK,
    FNKEY_OK,
};

#define tState data[0]
#define tKeyboardEvent data[1]
#define tKbFunctionKey data[2]

static void (*const sInputStateFuncs[])(struct Task *) = {
    InputState_Disabled,
    InputState_Enabled,
    InputState_2,
};

static void InputInit(void)
{
    CreateTask(Task_HandleInput, 1);
}

static u8 GetInputEvent(void)
{
    u8 taskId = FindTaskIdByFunc(Task_HandleInput);

    return gTasks[taskId].tKeyboardEvent;
}

static void SetInputState(u8 state)
{
    u8 taskId = FindTaskIdByFunc(Task_HandleInput);

    gTasks[taskId].tState = state;
}

static void Task_HandleInput(u8 taskId)
{
    sInputStateFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

static void InputState_Disabled(struct Task *task)
{
    task->tKeyboardEvent = 0;
}

static void InputState_Enabled(struct Task *task)
{
    task->tKeyboardEvent = 0;

    if (JOY_NEW(A_BUTTON))
        task->tKeyboardEvent = KBEVENT_PRESSED_A;
    else if (JOY_NEW(B_BUTTON))
	{
        task->tKeyboardEvent = KBEVENT_PRESSED_B;
		MoveCursorToBackButton();
	}
    else if (JOY_NEW(SELECT_BUTTON))
        task->tKeyboardEvent = KBEVENT_PRESSED_SELECT;
    else if (JOY_NEW(START_BUTTON))
	{
        task->tKeyboardEvent = KBEVENT_PRESSED_START;
		MoveCursorToOKButton();
	}
    else
        HandleDpadMovement(task);
}

static void InputState_2(struct Task *task)
{
    task->tKeyboardEvent = 0;
}

static void HandleDpadMovement(struct Task *task)
{
    const s16 sDpadDeltaX[] = {
         0,   //none
         0,   //up
         0,   //down
        -1,   //left
         1    //right
    };

    const s16 sDpadDeltaY[] = {
         0,   //none
        -1,   //up
         1,   //down
         0,   //left
         0    //right
    };

	const s16 s12RowTo5RowTableX[] = {0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
	const s16 s5RowTo12RowTableX[] = {0, 1, 3, 6, 10};

    s16 cursorX;
    s16 cursorY;
    u16 dpadDir;
    s16 prevCursorY;

    GetCursorPos(&cursorX, &cursorY);
    dpadDir = 0;
    if (gMain.newAndRepeatedKeys & DPAD_UP)
        dpadDir = 1;
    if (gMain.newAndRepeatedKeys & DPAD_DOWN)
        dpadDir = 2;
    if (gMain.newAndRepeatedKeys & DPAD_LEFT)
        dpadDir = 3;
    if (gMain.newAndRepeatedKeys & DPAD_RIGHT)
        dpadDir = 4;

    //Get new cursor position
    prevCursorY = cursorY;
    cursorX += sDpadDeltaX[dpadDir];
    cursorY += sDpadDeltaY[dpadDir];

    //Wrap cursor position in the Y direction
	if (cursorY < 0)
		cursorY = FILE_COUNT - 1;
	if (cursorY > FILE_COUNT - 1)
		cursorY = 0;
	
    //Handle cursor movement in Y direction
    if (sDpadDeltaY[dpadDir] != 0)
    {
		if (cursorY == FILE_COUNT - 1)
        {
            //We are now on the last file
            task->tKbFunctionKey = cursorX;
            cursorX = s12RowTo5RowTableX[cursorX];
        }
        else if (prevCursorY == FILE_COUNT - 1)
        {
            NamingScreen_RedrawOptions();
			cursorX = s5RowTo12RowTableX[cursorX];
        }
	}
    if (cursorY == FILE_COUNT - 1)
    {
        //There are only 5 keys on the last file,
        //so wrap Y accordingly
        if (cursorX < 0)
            cursorX = 4;
        if (cursorX > 4)
            cursorX = 0;
        if (cursorX == 0)
            task->tKbFunctionKey = FNKEY_CASE_UPPER;
        else if (cursorX == 1)
            task->tKbFunctionKey = FNKEY_CASE_LOWER;
        else if (cursorX == 2)
            task->tKbFunctionKey = FNKEY_CASE_OTHER;
        else if (cursorX == 3)
            task->tKbFunctionKey = FNKEY_BACK;
        else if (cursorX == 4)
            task->tKbFunctionKey = FNKEY_OK;
	}
	else
	{
		if (cursorX < 0)
			cursorX = COLUMN_COUNT - 1;
		if (cursorX > COLUMN_COUNT - 1)
			cursorX = 0;
	}	
    SetCursorPos(cursorX, cursorY);
}

#undef tState
#undef tKeyboardEvent
#undef tKbFunctionKey

static void PrintTitleFunction_NoMon(void)
{
    //if (gSaveBlock2Ptr->optionsLanguage == ENG)
	//{
		FillWindowPixelBuffer(sNamingScreenData->windows[3], PIXEL_FILL(0));
		AddTextPrinterParameterized3(sNamingScreenData->windows[3], 1, 1, 1, sNamingScreenTextColor, TEXT_SPEED_FF, sNamingScreenData->template->title);
	//}
	/*if (gSaveBlock2Ptr->optionsLanguage == SPA)
	{
		FillWindowPixelBuffer(sNamingScreenData->windows[3], PIXEL_FILL(0));
		AddTextPrinterParameterized3(sNamingScreenData->windows[3], 1, 1, 1, sNamingScreenTextColor, TEXT_SPEED_FF, sNamingScreenData->template->titleSpa);
	}*/
	CopyWindowToVram(sNamingScreenData->windows[3], COPYWIN_BOTH);
	PutWindowTilemap(sNamingScreenData->windows[3]);
}

static void PrintTitleFunction_WithMon(void)
{
    u8 buffer[0x20];

    //if (gSaveBlock2Ptr->optionsLanguage == ENG)
	//{
		StringCopy(buffer, gSpeciesNames[sNamingScreenData->monSpeciesOrPlayerGender]);
		StringAppendN(buffer, sNamingScreenData->template->title, 15);
	//}
    /*if (gSaveBlock2Ptr->optionsLanguage == SPA)
	{
		StringCopy(buffer, sNamingScreenData->template->titleSpa);
		StringAppendN(buffer, gSpeciesNames[sNamingScreenData->monSpeciesOrPlayerGender], 15);
		StringAppendN(buffer, Text_PkmnsNickname2Spa, 2);
	}*/	
	FillWindowPixelBuffer(sNamingScreenData->windows[3], PIXEL_FILL(0));
	AddTextPrinterParameterized3(sNamingScreenData->windows[3], 1, 1, 1, sNamingScreenTextColor, TEXT_SPEED_FF, buffer);
	CopyWindowToVram(sNamingScreenData->windows[3], COPYWIN_BOTH);
	PutWindowTilemap(sNamingScreenData->windows[3]);
}

static void (*const sPrintTitleFuncs[])(void) = {
    [NAMING_SCREEN_PLAYER]     = PrintTitleFunction_NoMon,
    [NAMING_SCREEN_BOX]        = PrintTitleFunction_NoMon,
    [NAMING_SCREEN_CAUGHT_MON] = PrintTitleFunction_WithMon,
    [NAMING_SCREEN_NAME_RATER] = PrintTitleFunction_WithMon,
    [NAMING_SCREEN_RIVAL]      = PrintTitleFunction_NoMon,
};

static void PrintTitle(void)
{
    sPrintTitleFuncs[sNamingScreenData->templateNum]();
}

static void (*const sAddGenderIconFuncs[])(void) = {
    AddGenderIconFunc_No,
    AddGenderIconFunc_Yes
};

static void CallAddGenderIconFunc(void)
{
    sAddGenderIconFuncs[sNamingScreenData->template->addGenderIcon]();
}

static void AddGenderIconFunc_No(void)
{

}

static const u8 sGenderColors[2][3] = {
    [MALE]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_BLUE, TEXT_COLOR_BLUE},
    [FEMALE] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED}
};

static void AddGenderIconFunc_Yes(void)
{
    u8 genderSymbol[2];
    bool8 gender = MALE;

    StringCopy(genderSymbol, gText_MaleSymbol);

    if (sNamingScreenData->monGender != MON_GENDERLESS)
    {
        if (sNamingScreenData->monGender == MON_FEMALE)
        {
            StringCopy(genderSymbol, gText_FemaleSymbol);
            gender = FEMALE;
        }
        AddTextPrinterParameterized3(sNamingScreenData->windows[2], 2, 104, 1, sGenderColors[gender], TEXT_SPEED_FF, genderSymbol);
    }
}

static u8 GetCharAtKeyboardPos(s16 x, s16 y)
{
    return gNamingScreenChars[sNamingScreenData->currentPage][y][x];
}

static u8 GetTextCaretPosition(void)
{
    u8 i;

    for (i = 0; i < sNamingScreenData->template->maxChars; i++)
    {
        if (sNamingScreenData->textBuffer[i] == EOS)
            return i;
    }
    return sNamingScreenData->template->maxChars - 1;
}

static u8 GetPreviousTextCaretPosition(void)
{
    s8 i;

    for (i = sNamingScreenData->template->maxChars - 1; i > 0; i--)
    {
        if (sNamingScreenData->textBuffer[i] != EOS)
            return i;
    }
    return 0;
}

static void DeleteTextCharacter(void)
{
    u8 index;
    u8 var2;

    index = GetPreviousTextCaretPosition();
    // Temporarily make this a space for redrawing purposes
    sNamingScreenData->textBuffer[index] = CHAR_SPACE;
    PrintBufferCharactersOnScreen();
    CopyBgTilemapBufferToVram(3);
    sNamingScreenData->textBuffer[index] = EOS;
    var2 = GetKeyRoleAtCursorPos();
    if (var2 == KEY_ROLE_CHAR || var2 == KEY_ROLE_BACKSPACE)
    PlaySE(SE_BALL);
}

static bool8 AppendCharToBuffer_CheckBufferFull(void)
{
    s16 x;
    s16 y;
	
    GetCursorPos(&x, &y);
    AddTextCharacter(GetCharAtKeyboardPos(x, y));
    PrintBufferCharactersOnScreen();
    CopyBgTilemapBufferToVram(3);
    PlaySE(SE_SELECT);

    if (GetPreviousTextCaretPosition() != sNamingScreenData->template->maxChars - 1)
        return FALSE;
    else
        return TRUE;
}

static void AddTextCharacter(u8 ch)
{
    u8 index = GetTextCaretPosition();

    sNamingScreenData->textBuffer[index] = ch;
}

static void CopyStringToDestBuffer(void)
{
    // Copy from the first non-whitespace character
    u8 i;

    for (i = 0; i < sNamingScreenData->template->maxChars; i++)
    {
        if (sNamingScreenData->textBuffer[i] != CHAR_SPACE && sNamingScreenData->textBuffer[i] != EOS)
        {
            StringCopyN(sNamingScreenData->destBuffer, sNamingScreenData->textBuffer, sNamingScreenData->template->maxChars + 1);
            break;
        }
    }
}

static void NamingScreen_LoadObjectsGfx(void)
{
    LoadSpriteSheets(sNamingScreenSpriteSheet);
    LoadSpritePalettes(sNamingScreenSpritePalette);
}

static void NamingScreen_LoadBGsPal(void)
{
    LoadPalette(gNamingScreenPal, 0, 0xE0);
    LoadPalette(gNamingScreenSelPal, 0x10, 0x20);
    LoadPalette(gNamingScreenTextPal, 0xA0, 0x20);
}

static void PrintBufferCharactersOnScreen(void)
{
    u8 i;
    u8 temp[2];
    u16 xoff;
    u8 maxChars = sNamingScreenData->template->maxChars;
    u16 xpos = sNamingScreenData->inputCharBaseXPos - 0x40;

    FillWindowPixelBuffer(sNamingScreenData->windows[2], PIXEL_FILL(0));

    for (i = 0; i < maxChars; i++)
    {
        temp[0] = sNamingScreenData->textBuffer[i];
        temp[1] = EOS;
        xoff = (IsLetter(temp[0]) == TRUE) ? 2 : 0;

		AddTextPrinterParameterized3(sNamingScreenData->windows[2], 2, i * 8 + xpos + xoff, 1, sNamingScreenTextColor, TEXT_SPEED_FF, temp);
    }
 
    CallAddGenderIconFunc();
    CopyWindowToVram(sNamingScreenData->windows[2], COPYWIN_GFX);
    PutWindowTilemap(sNamingScreenData->windows[2]);
}

static void NamingScreen_Callback(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void NamingScreen_TurnOffScreen(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
}

static void NamingScreen_InitDisplayMode(void)
{
    SetVBlankCallback(VBlankCB_NamingScreen);
}

static void VBlankCB_NamingScreen(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    SetGpuReg(REG_OFFSET_BG2VOFS, sNamingScreenData->bg2vOffset);
}

static void NamingScreen_ShowAllBgs(void)
{
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    ShowBg(3);
}

static bool8 IsLetter(u8 character)
{
    u8 i;

    for (i = 0; gText_AlphabetUpperLower[i] != EOS; i++)
    {
        if (character == gText_AlphabetUpperLower[i])
            return TRUE;
    }
    return FALSE;
}

static void NamingScreen_WindowFunc_DrawDialogueFrame(void)
{
	FillBgTilemapBufferRect(0, 0x30,    0,   14,   1,       1,  14);
	FillBgTilemapBufferRect(0, 0x31,    1,   14,   1,       1,  14);
	FillBgTilemapBufferRect(0, 0x32,    2,   14,  26,       1,  14);
    FillBgTilemapBufferRect(0, 0x33, 0x1C, 	 14, 	1, 		1, 	14);
    FillBgTilemapBufferRect(0, 0x34, 0x1D, 	 14, 	1, 		1, 	14);
	FillBgTilemapBufferRect(0, 0x35, 	0, 	 15, 	1, 		3, 	14);
    FillBgTilemapBufferRect(0, 0x36, 	1, 	 15, 	1, 		3, 	14);
    FillBgTilemapBufferRect(0, 0x37, 	2, 	 15,   26, 		4, 	14);
    FillBgTilemapBufferRect(0, 0x38, 0x1C, 	 15, 	1, 		3, 	14);
    FillBgTilemapBufferRect(0, 0x39, 0x1D, 	 15, 	1, 		3, 	14);
    FillBgTilemapBufferRect(0, 0x3A, 	0,   18, 	1, 		1, 	14);
    FillBgTilemapBufferRect(0, 0x3B, 	1,   18, 	1, 		1, 	14);
    FillBgTilemapBufferRect(0, 0x3D, 0x1C,   18, 	1, 		1, 	14);
    FillBgTilemapBufferRect(0, 0x3E, 0x1D,   18, 	1, 		1, 	14);
	FillBgTilemapBufferRect(0, 0x3F,    0,   19, 	1, 		1, 	14);
	FillBgTilemapBufferRect(0, 0x40,    1,   19, 	1, 		1, 	14);
	FillBgTilemapBufferRect(0, 0x41,    2,   19,   26, 		1, 	14);
	FillBgTilemapBufferRect(0, BG_TILE_H_FLIP(0x40), 0x1C, 19, 	1, 		1, 	14);
	FillBgTilemapBufferRect(0, BG_TILE_H_FLIP(0x3F), 0x1D, 19, 	1, 		1, 	14); 
}

static void NamingScreen_DrawDialogueFrame(void)
{
	sub_814FD38(0, 0x30, 0xE0);
	NamingScreen_WindowFunc_DrawDialogueFrame();
	FillWindowPixelBuffer(0, PIXEL_FILL(1));
	PutWindowTilemap(0);
	CopyWindowToVram(0, COPYWIN_BOTH);
}

//--------------------------------------------------
// Forward-declared variables
//--------------------------------------------------

static const struct NamingScreenTemplate sPlayerNamingScreenTemplate = {
    .copyExistingString = FALSE,
    .maxChars = PLAYER_NAME_LENGTH,
    .iconFunction = 1,
    .addGenderIcon = 0,
    .initialPage = KBPAGE_LETTERS_UPPER,
    .title = gText_YourName,
    //.titleSpa = Text_YourNameSpa,
};

static const struct NamingScreenTemplate sPcBoxNamingScreenTemplate = {
    .copyExistingString = FALSE,
    .maxChars = BOX_NAME_LENGTH,
    .iconFunction = 2,
    .addGenderIcon = 0,
    .initialPage = KBPAGE_LETTERS_UPPER,
    .title = gText_BoxName,
    //.titleSpa = Text_BoxNameSpa,
};

static const struct NamingScreenTemplate sMonNamingScreenTemplate = {
    .copyExistingString = FALSE,
    .maxChars = POKEMON_NAME_LENGTH,
    .iconFunction = 3,
    .addGenderIcon = 1,
    .initialPage = KBPAGE_LETTERS_UPPER,
    .title = gText_PkmnsNickname,
    //.titleSpa = Text_PkmnsNicknameSpa,
};

static const struct NamingScreenTemplate sRivalNamingScreenTemplate = {
    .copyExistingString = FALSE,
    .maxChars = OT_NAME_LENGTH,
    .iconFunction = 4,
    .addGenderIcon = 0,
    .initialPage = KBPAGE_LETTERS_UPPER,
    .title = gText_RivalsName,
    //.titleSpa = Text_RivalsNameSpa,
};

static const struct NamingScreenTemplate *const sNamingScreenTemplates[] = {
    &sPlayerNamingScreenTemplate,
    &sPcBoxNamingScreenTemplate,
    &sMonNamingScreenTemplate,
    &sMonNamingScreenTemplate,
    &sRivalNamingScreenTemplate,
};

static const struct OamData gOamData_858BFEC = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
};

static const struct OamData gOamData_Cursor = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x32),
    .x = 0,
    .size = SPRITE_SIZE(16x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
};

static const struct Subsprite Subsprite_PcIcon[] = {
    {
        .x = -8,
        .y = -12,
        .shape = SPRITE_SHAPE(16x8),
        .size = SPRITE_SIZE(16x8),
        .tileOffset = 0,
        .priority = 3
    }, {
        .x = -8,
        .y = -4,
        .shape = SPRITE_SHAPE(16x8),
        .size = SPRITE_SIZE(16x8),
        .tileOffset = 2,
        .priority = 3
    }, {
        .x = -8,
        .y =  4,
        .shape = SPRITE_SHAPE(16x8),
        .size = SPRITE_SIZE(16x8),
        .tileOffset = 4,
        .priority = 3
    }
};

static const struct SubspriteTable SubspriteTable_PcIcon[] = {
    subsprite_table(Subsprite_PcIcon)
};

static const struct SpriteFrameImage SpriteFrameImage_PcIcon[] = {
	{gSpriteImage_PcIcon0, 0xC0},
	{gSpriteImage_PcIcon1, 0xC0},
};

static const union AnimCmd gSpriteAnim_858C090[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd gSpriteAnim_858C098[] = {
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_END
};

static const union AnimCmd gSpriteAnim_858C0A4[] = {
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const gSpriteAnimTable_Underscore[] = {
    gSpriteAnim_858C090
};

static const union AnimCmd *const gSpriteAnimTable_Cursor[] = {
    gSpriteAnim_858C090,
    gSpriteAnim_858C098
};

static const union AnimCmd *const gSpriteAnimTable_PCIcon[] = {
    gSpriteAnim_858C0A4
};

static const struct SpriteTemplate gSpriteTemplate_Cursor = {
    .tileTag = 0x0007,
    .paletteTag = 0x0005,
    .oam = &gOamData_Cursor,
    .anims = gSpriteAnimTable_Cursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = Callback_Cursor
};

static const struct SpriteTemplate gSpriteTemplate_UnderscoreMain = {
    .tileTag = 0x000A,
    .paletteTag = 0x0003,
    .oam = &gOamData_858BFEC,
    .anims = gSpriteAnimTable_Underscore,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = Callback_UnderscoreMain
};

static const struct SpriteTemplate sSpriteTemplate_Underscore = {
    .tileTag = 0x000B,
    .paletteTag = 0x0003,
    .oam = &gOamData_858BFEC,
    .anims = gSpriteAnimTable_Underscore,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = Callback_Underscore
};

static const struct SpriteTemplate gSpriteTemplate_PCIcon = {
    .tileTag = 0xFFFF,
    .paletteTag = 0x0000,
    .oam = &gOamData_858BFEC,
    .anims = gSpriteAnimTable_PCIcon,
    .images = SpriteFrameImage_PcIcon,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct SpriteSheet sNamingScreenSpriteSheet[] = {
    { gNamingScreenCursorTiles, 			0x100, 7 },
    { gNamingScreenActiveCursorSmallTiles, 	0x100, 8 },
    { gNamingScreenActiveCursorBigTiles, 	0x100, 9 },
    { gNamingScreenUnderscoreTiles, 		 0x20, 11},
    {NULL},
};

static const struct SpritePalette sNamingScreenSpritePalette[] =
{
    {gNamingScreenPalettes[0], 0},
    {gNamingScreenPalettes[1], 3},
    {gNamingScreenPalettes[2], 5},
    {NULL},
};