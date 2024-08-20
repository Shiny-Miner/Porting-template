#include "../include/global.h"
#include "../include/gflib.h"
#include "../include/battle_anim.h"
#include "../include/battle.h"
#include "../include/battle_message.h"
#include "../include/data.h"
#include "../include/decompress.h"
#include "../include/help_system.h"
#include "../include/evolution_scene.h"
#include "../include/evolution_graphics.h"
#include "../include/link.h"
#include "../include/link_rfu.h"
#include "../include/m4a.h"
#include "../include/event_data.h"
#include "../include/trade_scene.h"
#include "../include/new_menu_helpers.h"
#include "../include/menu.h"
#include "../include/overworld.h"
#include "../include/pokedex.h"
#include "../include/pokemon_summary_screen.h"
#include "../include/scanline_effect.h"
#include "../include/strings.h"
#include "../include/task.h"
#include "../include/text_window.h"
#include "../include/trig.h"
#include "../include/constants/moves.h"
#include "../include/constants/songs.h"
#include "../include/constants/pokemon.h"
#include "../include/constants/items.h"

void EvolutionScene(struct Pokemon* mon, u16 postEvoSpecies, bool8 canStopEvo, u8 partyId)
{
    u8 name[20];
    u16 currSpecies;
    u32 trainerId, personality;
    const struct CompressedSpritePalette* pokePal;
    u8 ID;
    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);
    CpuFill32(0, (void*)(VRAM), VRAM_SIZE);
    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    SetGpuReg(REG_OFFSET_WIN0H, 0);
    SetGpuReg(REG_OFFSET_WIN0V, 0);
    SetGpuReg(REG_OFFSET_WIN1H, 0);
    SetGpuReg(REG_OFFSET_WIN1V, 0);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    ResetPaletteFade();
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 256;
    gBattle_BG3_Y = 0;
    gBattleTerrain = BATTLE_TERRAIN_PLAIN;
    InitBattleBgsVideo();
    LoadBattleTextboxAndBackground();
    ResetSpriteData();
    ScanlineEffect_Stop();
    ResetTasks();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 4;

    sEvoStructPtr = AllocZeroed(sizeof(struct EvoInfo));
    if (!gMain.inBattle || gMonSpritesGfxPtr == NULL) AllocateMonSpritesGfx();     
    // If gMonSpritesGfxPtr has been freed (which can also happen at the end of the battle) then it needs to be reallocated

    GetMonData(mon, MON_DATA_NICKNAME, name);
    StringCopy_Nickname(gStringVar1, name);
    StringCopy(gStringVar2, gSpeciesNames[postEvoSpecies]);
    // preEvo sprite
    currSpecies = GetMonData(mon, MON_DATA_SPECIES);
    trainerId = GetMonData(mon, MON_DATA_OT_ID);
    personality = GetMonData(mon, MON_DATA_PERSONALITY);
    DecompressPicFromTable_2(&gMonFrontPicTable[currSpecies],
                             gMonSpritesGfxPtr->sprites.ptr[B_POSITION_OPPONENT_LEFT],
                             currSpecies);
    pokePal = GetMonSpritePalStructFromOtIdPersonality(currSpecies, trainerId, personality);
    LoadCompressedPalette(pokePal->data, 0x110, 0x20);
    SetMultiuseSpriteTemplateToPokemon(currSpecies, B_POSITION_OPPONENT_LEFT);
    gMultiuseSpriteTemplate.affineAnims = gDummySpriteAffineAnimTable;
    sEvoStructPtr->preEvoSpriteId = ID = CreateSprite(&gMultiuseSpriteTemplate, 120, 64, 30);
    gSprites[ID].callback = SpriteCallbackDummy_2;
    gSprites[ID].oam.paletteNum = 1;
    gSprites[ID].invisible = TRUE;
    // postEvo sprite
    DecompressPicFromTable_2(&gMonFrontPicTable[postEvoSpecies],
                             gMonSpritesGfxPtr->sprites.ptr[B_POSITION_OPPONENT_RIGHT],
                             postEvoSpecies);
    pokePal = GetMonSpritePalStructFromOtIdPersonality(postEvoSpecies, trainerId, personality);
    LoadCompressedPalette(pokePal->data, 0x120, 0x20);
    SetMultiuseSpriteTemplateToPokemon(postEvoSpecies, B_POSITION_OPPONENT_RIGHT);
    gMultiuseSpriteTemplate.affineAnims = gDummySpriteAffineAnimTable;
    sEvoStructPtr->postEvoSpriteId = ID = CreateSprite(&gMultiuseSpriteTemplate, 120, 64, 30);
    gSprites[ID].callback = SpriteCallbackDummy_2;
    gSprites[ID].oam.paletteNum = 2;
    gSprites[ID].invisible = TRUE;
    LoadEvoSparkleSpriteAndPal();
    sEvoStructPtr->evoTaskId = ID = CreateTask(Task_EvolutionScene, 0);
    gTasks[ID].tState = 0;
    gTasks[ID].tPreEvoSpecies = currSpecies;
    gTasks[ID].tPostEvoSpecies = postEvoSpecies;
    gTasks[ID].tCanStop = canStopEvo;
    gTasks[ID].tLearnsFirstMove = TRUE;
    gTasks[ID].tEvoWasStopped = FALSE;
    gTasks[ID].tPartyId = partyId;
    memcpy(&sEvoStructPtr->savedPalette, &gPlttBufferUnfaded[0x20], sizeof(sEvoStructPtr->savedPalette));
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_1D_MAP);
    SetHBlankCallback(EvoDummyFunc);
    SetVBlankCallback(VBlankCB_EvolutionScene);
    m4aMPlayAllStop();
    SetMainCallback2(CB2_EvolutionSceneUpdate);
}

#define LEFT_PKMN gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)]
#define RIGHT_PKMN gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)]

static void Task_EvolutionScene(u8 taskId)
{
    u32 var;
    struct Pokemon* mon = &gPlayerParty[gTasks[taskId].tPartyId];
    // check if B Button was held, so the evolution gets stopped
    if (gMain.heldKeys == B_BUTTON
        && gTasks[taskId].tState == EVOSTATE_WAIT_CYCLE_MON_SPRITE
        && gTasks[sEvoGraphicsTaskId].isActive
        && gTasks[taskId].tBits & TASK_BIT_CAN_STOP)
    {
        gTasks[taskId].tState = EVOSTATE_CANCEL;
        gTasks[sEvoGraphicsTaskId].tEvoStopped = TRUE;
        if (gMain.inBattle && gBattleOutcome == 0)
        {
            if (gTasks[taskId].tPartyId == LEFT_PKMN) 
            gPlayerDoesNotWantToEvolveLeft = TRUE; // Stop trying to make the left Pokémon evolve again in battle
            else if (gTasks[taskId].tPartyId == RIGHT_PKMN) 
            gPlayerDoesNotWantToEvolveRight = TRUE; // Stop trying to make the right Pokémon evolve again in battle
        }
        StopBgAnimation();
        return;
    }
    switch (gTasks[taskId].tState)
    {
    case EVOSTATE_FADE_IN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        gSprites[sEvoStructPtr->preEvoSpriteId].invisible = FALSE;
        gTasks[taskId].tState++;
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        break;
    case EVOSTATE_INTRO_MSG:
        if (!gPaletteFade.active)
        {
            StringExpandPlaceholders(gStringVar4, gText_PkmnIsEvolving);
            BattlePutTextOnWindow(gStringVar4, B_WIN_MSG);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_INTRO_MON_ANIM:
        if (!IsTextPrinterActive(0))
        {
            EvoScene_DoMonAnimAndCry(sEvoStructPtr->preEvoSpriteId, gTasks[taskId].tPreEvoSpecies);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_INTRO_SOUND:
        if (EvoScene_IsMonAnimFinished(sEvoStructPtr->preEvoSpriteId))
        {
            PlaySE(MUS_EVOLUTION_INTRO);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_START_MUSIC:
        if (!IsSEPlaying())
        {
            // Start music, fade background to black
            PlayNewMapMusic(MUS_EVOLUTION);
            gTasks[taskId].tState++;
            BeginNormalPaletteFade(0x1C, 4, 0, 0x10, RGB_BLACK);
        }
        break;
    case EVOSTATE_START_BG_AND_SPARKLE_SPIRAL:
        if (!gPaletteFade.active)
        {
            StartBgAnimation(FALSE);
            sEvoGraphicsTaskId = EvolutionSparkles_SpiralUpward(17);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_SPARKLE_ARC:
        if (!gTasks[sEvoGraphicsTaskId].isActive)
        {
            gTasks[taskId].tState++;
            sEvoStructPtr->delayTimer = 1;
            sEvoGraphicsTaskId = EvolutionSparkles_ArcDown();
        }
        break;
    case EVOSTATE_CYCLE_MON_SPRITE: // launch task that flashes pre evo with post evo sprites
        if (!gTasks[sEvoGraphicsTaskId].isActive)
        {
            sEvoGraphicsTaskId = CycleEvolutionMonSprite(sEvoStructPtr->preEvoSpriteId, sEvoStructPtr->postEvoSpriteId);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_WAIT_CYCLE_MON_SPRITE:
        if (--sEvoStructPtr->delayTimer == 0)
        {
            sEvoStructPtr->delayTimer = 3;
            if (!gTasks[sEvoGraphicsTaskId].isActive)
                gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_SPARKLE_CIRCLE:
        sEvoGraphicsTaskId = EvolutionSparkles_CircleInward();
        gTasks[taskId].tState++;
        break;
    case EVOSTATE_SPARKLE_SPRAY:
        if (!gTasks[sEvoGraphicsTaskId].isActive)
        {
            sEvoGraphicsTaskId = EvolutionSparkles_SprayAndFlash(gTasks[taskId].tPostEvoSpecies);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_EVO_SOUND:
        if (!gTasks[sEvoGraphicsTaskId].isActive)
        {
            PlaySE(SE_EXP);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_RESTORE_SCREEN: // stop music, return screen to pre-fade state
        if (IsSEPlaying())
        {
            m4aMPlayAllStop();
            memcpy(&gPlttBufferUnfaded[0x20], sEvoStructPtr->savedPalette, sizeof(sEvoStructPtr->savedPalette));
            RestoreBgAfterAnim();
            BeginNormalPaletteFade(0x1C, 0, 0x10, 0, RGB_BLACK);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_EVO_MON_ANIM:
        if (!gPaletteFade.active)
        {
            EvoScene_DoMonAnimAndCry(sEvoStructPtr->postEvoSpriteId, gTasks[taskId].tPostEvoSpecies);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_SET_MON_EVOLVED:
        if (IsCryFinished())
        {
            StringExpandPlaceholders(gStringVar4, gText_CongratsPkmnEvolved);
            BattlePutTextOnWindow(gStringVar4, B_WIN_MSG);
            PlayBGM(MUS_EVOLVED);
            gTasks[taskId].tState++;
            SetMonData(mon, MON_DATA_SPECIES, (void*)(&gTasks[taskId].tPostEvoSpecies));
            CalculateMonStats(mon);
            EvolutionRenameMon(mon, gTasks[taskId].tPreEvoSpecies, gTasks[taskId].tPostEvoSpecies);
            GetSetPokedexFlag(SpeciesToNationalPokedexNum(gTasks[taskId].tPostEvoSpecies), FLAG_SET_SEEN);
            GetSetPokedexFlag(SpeciesToNationalPokedexNum(gTasks[taskId].tPostEvoSpecies), FLAG_SET_CAUGHT);
            IncrementGameStat(GAME_STAT_EVOLVED_POKEMON);
            if (gMain.inBattle && gBattleOutcome == 0)
            { 
                // Update BattlePokemon stats if in battle
                u8 monId = gTasks[taskId].tPartyId;
                if (monId == LEFT_PKMN) 
                    CopyPlayerPartyMonToBattleData(0, monId, FALSE);
                else if (monId == RIGHT_PKMN) 
                {
                    CopyPlayerPartyMonToBattleData(2, monId, FALSE);
                }
            }
        }
        break;
    case EVOSTATE_TRY_LEARN_MOVE:
        if (!IsTextPrinterActive(0))
        {
            var = MonTryLearningNewMove(mon, gTasks[taskId].tLearnsFirstMove);
            if (var != MOVE_NONE && !gTasks[taskId].tEvoWasStopped)
            {
                u8 text[20];
                if (!(gTasks[taskId].tBits & TASK_BIT_LEARN_MOVE))
                {
                    StopMapMusic();
                    if (gMain.inBattle && gBattleOutcome == 0) PlayBattleBGM(); // If battle is still ongoing, replay battle music
                    else Overworld_PlaySpecialMapMusic();
                }

                gTasks[taskId].tBits |= TASK_BIT_LEARN_MOVE;
                gTasks[taskId].tLearnsFirstMove = FALSE;
                gTasks[taskId].tLearnMoveState = MVSTATE_INTRO_MSG_1;
                GetMonData(mon, MON_DATA_NICKNAME, text);
                StringCopy_Nickname(gBattleTextBuff1, text);
                if (var == MON_HAS_MAX_MOVES)
                    gTasks[taskId].tState = EVOSTATE_REPLACE_MOVE;
                else if (var == MON_ALREADY_KNOWS_MOVE)
                    break;
                else
                {
                    if (gMain.inBattle && gBattleOutcome == 0)
                    {
                        if (gTasks[taskId].tPartyId == LEFT_PKMN) 
                        {
                            GiveMoveToBattleMon(&gBattleMons[0], var); // Ensure the Pokémon can use the move in battle
                        }
                        else if (gTasks[taskId].tPartyId == RIGHT_PKMN) 
                        {
                            GiveMoveToBattleMon(&gBattleMons[2], var);  // Ensure the Pokémon can use the move in battle
                        }
                    }
                    gTasks[taskId].tState = EVOSTATE_LEARNED_MOVE;
                }
            }
            else // no move to learn, or evolution was canceled
            {
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
                gTasks[taskId].tState++;
            }
        }
        break;
    case EVOSTATE_END:
        if (!gPaletteFade.active)
        {
            if (!(gTasks[taskId].tBits & TASK_BIT_LEARN_MOVE))
            {
                StopMapMusic();
                if (gMain.inBattle && gBattleOutcome == 0) PlayBattleBGM(); // If battle is still ongoing, replay battle music
                else Overworld_PlaySpecialMapMusic();
            }
            if (!gTasks[taskId].tEvoWasStopped)
                CreateShedinja(gTasks[taskId].tPreEvoSpecies, mon);

            DestroyTask(taskId);
            if (!gMain.inBattle || gBattleOutcome != 0) FreeMonSpritesGfx(); // Free resources if battle is not ongoing
            Free(sEvoStructPtr);
            sEvoStructPtr = NULL;
            FreeAllWindowBuffers();
            SetMainCallback2(gCB2_AfterEvolution);
        }
        break;
    case EVOSTATE_CANCEL:
        if (!gTasks[sEvoGraphicsTaskId].isActive)
        {
            m4aMPlayAllStop();
            BeginNormalPaletteFade(0x6001C, 0, 0x10, 0, RGB_WHITE);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_CANCEL_MON_ANIM:
        if (!gPaletteFade.active)
        {
            EvoScene_DoMonAnimAndCry(sEvoStructPtr->preEvoSpriteId, gTasks[taskId].tPreEvoSpecies);
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_CANCEL_MSG:
        if (EvoScene_IsMonAnimFinished(sEvoStructPtr->preEvoSpriteId))
        {
            if (gTasks[taskId].tEvoWasStopped)
                StringExpandPlaceholders(gStringVar4, gText_EllipsisQuestionMark);
            else // Fire Red leftover probably
                StringExpandPlaceholders(gStringVar4, gText_PkmnStoppedEvolving);
            BattlePutTextOnWindow(gStringVar4, B_WIN_MSG);
            gTasks[taskId].tEvoWasStopped = TRUE;
            gTasks[taskId].tState = EVOSTATE_TRY_LEARN_MOVE;
        }
        break;
    case EVOSTATE_LEARNED_MOVE:
        if (!IsTextPrinterActive(0) && !IsSEPlaying())
        {
            BufferMoveToLearnIntoBattleTextBuff2();
            PlayFanfare(MUS_LEVEL_UP);
            BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_PKMNLEARNEDMOVE - BATTLESTRINGS_ID_ADDER]);
            BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
            gTasks[taskId].tLearnsFirstMove = 0x40; // re-used as a counter
            gTasks[taskId].tState++;
        }
        break;
    case EVOSTATE_TRY_LEARN_ANOTHER_MOVE:
        if (!IsTextPrinterActive(0) && !IsSEPlaying() && --gTasks[taskId].tLearnsFirstMove == 0)
            gTasks[taskId].tState = EVOSTATE_TRY_LEARN_MOVE;
        break;
    case EVOSTATE_REPLACE_MOVE:
        switch (gTasks[taskId].tLearnMoveState)
        {
        case MVSTATE_INTRO_MSG_1:
            if (!IsTextPrinterActive(0) && !IsSEPlaying())
            {
                // "{mon} is trying to learn {move}"
                BufferMoveToLearnIntoBattleTextBuff2();
                BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_TRYTOLEARNMOVE1 - BATTLESTRINGS_ID_ADDER]);
                BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
                gTasks[taskId].tLearnMoveState++;
            }
            break;
        case MVSTATE_INTRO_MSG_2:
            if (!IsTextPrinterActive(0) && !IsSEPlaying())
            {
                // "But, {mon} can't learn more than four moves"
                BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_TRYTOLEARNMOVE2 - BATTLESTRINGS_ID_ADDER]);
                BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
                gTasks[taskId].tLearnMoveState++;
            }
            break;
        case MVSTATE_INTRO_MSG_3:
            if (!IsTextPrinterActive(0) && !IsSEPlaying())
            {
                // "Delete a move to make room for {move}?"
                BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_TRYTOLEARNMOVE3 - BATTLESTRINGS_ID_ADDER]);
                BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
                gTasks[taskId].tLearnMoveYesState = MVSTATE_SHOW_MOVE_SELECT;
                gTasks[taskId].tLearnMoveNoState = MVSTATE_ASK_CANCEL;
                gTasks[taskId].tLearnMoveState++;
            }
        case MVSTATE_PRINT_YES_NO:
            if (!IsTextPrinterActive(0) && !IsSEPlaying())
            {
                HandleBattleWindow(0x18, 8, 0x1D, 0xD, 0);
                BattlePutTextOnWindow(gText_BattleYesNoChoice, B_WIN_YESNO);
                gTasks[taskId].tLearnMoveState++;
                sEvoCursorPos = 0;
                BattleCreateYesNoCursorAt(0);
            }
            break;
        case MVSTATE_HANDLE_YES_NO:
            // This Yes/No is used for both the initial "delete move?" prompt
            // and for the "stop learning move?" prompt
            // What Yes/No do next is determined by tLearnMoveYesState / tLearnMoveNoState
            if (JOY_NEW(DPAD_UP) && sEvoCursorPos != 0)
            {
                // Moved onto YES
                PlaySE(SE_SELECT);
                BattleDestroyYesNoCursorAt(sEvoCursorPos);
                sEvoCursorPos = 0;
                BattleCreateYesNoCursorAt(0);
            }
            if (JOY_NEW(DPAD_DOWN) && sEvoCursorPos == 0)
            {
                // Moved onto NO
                PlaySE(SE_SELECT);
                BattleDestroyYesNoCursorAt(sEvoCursorPos);
                sEvoCursorPos = 1;
                BattleCreateYesNoCursorAt(1);
            }
            if (JOY_NEW(A_BUTTON))
            {
                HandleBattleWindow(0x18, 8, 0x1D, 0xD, WINDOW_CLEAR);
                PlaySE(SE_SELECT);
                if (sEvoCursorPos != 0)
                {
                    // NO
                    gTasks[taskId].tLearnMoveState = gTasks[taskId].tLearnMoveNoState;
                }
                else
                {
                    // YES
                    gTasks[taskId].tLearnMoveState = gTasks[taskId].tLearnMoveYesState;
                    if (gTasks[taskId].tLearnMoveState == MVSTATE_SHOW_MOVE_SELECT)
                        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
                }
            }
            if (JOY_NEW(B_BUTTON))
            {
                // Equivalent to selecting NO
                HandleBattleWindow(0x18, 8, 0x1D, 0xD, WINDOW_CLEAR);
                PlaySE(SE_SELECT);
                gTasks[taskId].tLearnMoveState = gTasks[taskId].tLearnMoveNoState;
            }
            break;
        case MVSTATE_SHOW_MOVE_SELECT:
            if (!gPaletteFade.active)
            {
                FreeAllWindowBuffers();
                ShowSelectMovePokemonSummaryScreen(gPlayerParty, gTasks[taskId].tPartyId,
                            gPlayerPartyCount - 1, CB2_EvolutionSceneLoadGraphics,
                            gMoveToLearn);
                gTasks[taskId].tLearnMoveState++;
            }
            break;
        case MVSTATE_HANDLE_MOVE_SELECT:
            if (!gPaletteFade.active && gMain.callback2 == CB2_EvolutionSceneUpdate)
            {
                var = GetMoveSlotToReplace();
                if (var == MAX_MON_MOVES)
                {
                    // Didn't select move slot
                    gTasks[taskId].tLearnMoveState = MVSTATE_ASK_CANCEL;
                }
                else
                {
                    // Selected move to forget
                    u16 move = GetMonData(mon, var + MON_DATA_MOVE1);
                    if (IsHMMove2(move))
                    {
                        // Can't forget HMs
                        BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_HMMOVESCANTBEFORGOTTEN - BATTLESTRINGS_ID_ADDER]);
                        BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
                        gTasks[taskId].tLearnMoveState = MVSTATE_RETRY_AFTER_HM;
                    }
                    else
                    {
                        // Forget move
                        PREPARE_MOVE_BUFFER(gBattleTextBuff2, move)

                        if (gMain.inBattle && gBattleOutcome == 0)
                        {
                            if (gTasks[taskId].tPartyId == LEFT_PKMN) 
                            {
                                RemoveBattleMonPPBonus(&gBattleMons[0], var);
                                SetBattleMonMoveSlot(&gBattleMons[0], gMoveToLearn, var); // Replace in-battle Pokémon's move with the new move
                            }
                            else if (gTasks[taskId].tPartyId == RIGHT_PKMN) 
                            {
                                RemoveBattleMonPPBonus(&gBattleMons[2], var);
                                SetBattleMonMoveSlot(&gBattleMons[2], gMoveToLearn, var); // Replace in-battle Pokémon's move with the new move
                            }
                        }
                        RemoveMonPPBonus(mon, var);
                        SetMonMoveSlot(mon, gMoveToLearn, var);
                        gTasks[taskId].tLearnMoveState++;
                    }
                }
            }
            break;
        case MVSTATE_FORGET_MSG_1:
            BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_123POOF - BATTLESTRINGS_ID_ADDER]);
            BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
            gTasks[taskId].tLearnMoveState++;
            break;
        case MVSTATE_FORGET_MSG_2:
            if (!IsTextPrinterActive(0) && !IsSEPlaying())
            {
                BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_PKMNFORGOTMOVE - BATTLESTRINGS_ID_ADDER]);
                BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
                gTasks[taskId].tLearnMoveState++;
            }
            break;
        case MVSTATE_LEARNED_MOVE:
            if (!IsTextPrinterActive(0) && !IsSEPlaying())
            {
                BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_ANDELLIPSIS - BATTLESTRINGS_ID_ADDER]);
                BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
                gTasks[taskId].tState = EVOSTATE_LEARNED_MOVE;
            }
            break;
        case MVSTATE_ASK_CANCEL:
            BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_STOPLEARNINGMOVE - BATTLESTRINGS_ID_ADDER]);
            BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
            gTasks[taskId].tLearnMoveYesState = MVSTATE_CANCEL;
            gTasks[taskId].tLearnMoveNoState = MVSTATE_INTRO_MSG_1;
            gTasks[taskId].tLearnMoveState = MVSTATE_PRINT_YES_NO;
            break;
        case MVSTATE_CANCEL:
            BattleStringExpandPlaceholdersToDisplayedString(gBattleStringsTable[STRINGID_DIDNOTLEARNMOVE - BATTLESTRINGS_ID_ADDER]);
            BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MSG);
            gTasks[taskId].tState = EVOSTATE_TRY_LEARN_MOVE;
            break;
        case MVSTATE_RETRY_AFTER_HM:
            if (!IsTextPrinterActive(0) && !IsSEPlaying())
                gTasks[taskId].tLearnMoveState = MVSTATE_SHOW_MOVE_SELECT;
            break;
        }
        break;
    }
}