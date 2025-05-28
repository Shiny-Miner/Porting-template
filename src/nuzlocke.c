#include "defines.h"
#include "defines_battle.h"
#include "../include/battle_anim.h"
#include "../include/event_data.h"
#include "../include/pokeball.h"
#include "../include/random.h"
#include "../include/constants/trainers.h"

#include "../include/new/ai_util.h"
#include "../include/new/ai_master.h"
#include "../include/new/ai_switching.h"
#include "../include/new/battle_controller_opponent.h"
#include "../include/new/battle_start_turn_start.h"
#include "../include/new/battle_util.h"
#include "../include/new/frontier.h"
#include "../include/new/mega.h"
#include "../include/new/move_menu.h"
#include "../include/new/multi.h"
#include "../include/new/switching.h"
#include "../include/naming_screen.h"
#include "../include/battle_setup.h"
#include "../include/bg.h"
#include "../include/daycare.h"
#include "../include/event_data.h"
#include "../include/event_object_movement.h"
#include "../include/field_control_avatar.h"
#include "../include/field_effect.h"
#include "../include/field_effect_helpers.h"
#include "../include/field_fadetransition.h"
#include "../include/field_message_box.h"
#include "../include/field_player_avatar.h"
#include "../include/field_poison.h"
#include "../include/field_screen_effect.h"
#include "../include/field_weather.h"
#include "../include/fieldmap.h"
#include "../include/fldeff_misc.h"
#include "../include/follower_mon.h"
#include "../include/item.h"
#include "../include/link.h"
#include "../include/list_menu.h"
#include "../include/m4a.h"
#include "../include/map_name_popup.h"
#include "../include/map_preview_screen.h"
#include "../include/map_scripts.h"
#include "../include/metatile_behavior.h"
#include "../include/overworld.h"
#include "../include/party_menu.h"
#include "../include/quest_log.h"
#include "../include/random.h"
#include "../include/rtc.h"
#include "../include/safari_zone.h"
#include "../include/script.h"
#include "../include/sprite.h"
#include "../include/script_menu.h"
#include "../include/sound.h"
#include "../include/string_util.h"

#include "../include/constants/flags.h"
#include "../include/constants/items.h"
#include "../include/constants/maps.h"
#include "../include/constants/metatile_behaviors.h"
#include "../include/constants/region_map_sections.h"
#include "../include/constants/songs.h"
#include "../include/constants/trainers.h"
#include "../include/constants/trainer_classes.h"

#include "../include/new/dexnav.h"
#include "../include/new/dynamic_ow_pals.h"
#include "../include/new/item.h"
#include "../include/new/follow_me.h"
#include "../include/new/frontier.h"
#include "../include/new/util.h"
#include "../include/new/multi.h"
#include "../include/new/overworld.h"
#include "../include/new/overworld_data.h"
#include "../include/new/party_menu.h"
#include "../include/new/read_keys.h"
#include "../include/new/wild_encounter.h"
#include "../include/pokemon_storage_system.h"

extern u8 gBattlerAttacker;
#define gText_BattleYesNoChoice (u8*) 0x83FE791
#define B_WIN_YESNO 14
#define SE_SELECT 5
void Cmd_trygivecaughtmonnick(void)
{
    if (FlagGet(FLAG_NUZLOCKE)) // ⭐ Nuzlocke Mode: Always nickname
    {
        switch (gBattleCommunication[MULTIUSE_STATE])
        {
        case 0:
            GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
            FreeAllWindowBuffers();

            DoNamingScreen(NAMING_SCREEN_CAUGHT_MON, gBattleStruct->caughtMonNick,
                           GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_SPECIES, NULL),
                           GetMonGender(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]]),
                           GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_PERSONALITY, NULL),
                           BattleMainCB2);

            gBattleCommunication[MULTIUSE_STATE]++;
            break;
        case 1:
            if (gMain.callback2 == BattleMainCB2 && !gPaletteFade->active)
            {
                SetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
                gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
            }
            break;
        }
        return;
    }

    // Default behavior when Nuzlocke is OFF (yes/no choice)
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        HandleBattleWindow(23, 8, 29, 13, 0);
        BattlePutTextOnWindow(gText_BattleYesNoChoice, B_WIN_YESNO);
        gBattleCommunication[MULTIUSE_STATE]++;
        gBattleCommunication[CURSOR_POSITION] = 0;
        BattleCreateYesNoCursorAt(0);
        break;
    case 1:
        if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION] != 0)
        {
            PlaySE(SE_SELECT);
            BattleDestroyYesNoCursorAt(0);
            gBattleCommunication[CURSOR_POSITION] = 0;
            BattleCreateYesNoCursorAt(0);
        }
        if (JOY_NEW(DPAD_DOWN) && gBattleCommunication[CURSOR_POSITION] == 0)
        {
            PlaySE(SE_SELECT);
            BattleDestroyYesNoCursorAt(0);
            gBattleCommunication[CURSOR_POSITION] = 1;
            BattleCreateYesNoCursorAt(1);
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (gBattleCommunication[CURSOR_POSITION] == 0)
            {
                gBattleCommunication[MULTIUSE_STATE]++;
                BeginFastPaletteFade(3);
            }
            else
            {
                gBattleCommunication[MULTIUSE_STATE] = 4;
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBattleCommunication[MULTIUSE_STATE] = 4;
        }
        break;
    case 2:
        if (!gPaletteFade->active)
        {
            GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
            FreeAllWindowBuffers();

            DoNamingScreen(NAMING_SCREEN_CAUGHT_MON, gBattleStruct->caughtMonNick,
                           GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_SPECIES, NULL),
                           GetMonGender(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]]),
                           GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_PERSONALITY, NULL),
                           BattleMainCB2);

            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 3:
        if (gMain.callback2 == BattleMainCB2 && !gPaletteFade->active)
        {
            SetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        }
        break;
    case 4:
        if (CalculatePlayerPartyCount() == PARTY_SIZE)
            gBattlescriptCurrInstr += 5;
        else
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        break;
    }
}
void CB2_EndTrainerBattle(void)
{
    if (sTrainerBattleMode == TRAINER_BATTLE_OAK_TUTORIAL)
    {
        if (IsPlayerDefeated(gBattleOutcome) == TRUE)
        {
            gSpecialVar_LastResult = TRUE;
            if (sRivalBattleFlags & RIVAL_BATTLE_HEAL_AFTER)
            {
                HealPlayerParty();
            }
            else
            {
                SetMainCallback2(CB2_WhiteOut);
                return;
            }
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
            SetBattledTrainerFlag();
            QuestLogEvents_HandleEndTrainerBattle();
        }
        else
        {
            gSpecialVar_LastResult = FALSE;
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
            SetBattledTrainerFlag();
            QuestLogEvents_HandleEndTrainerBattle();
        }

    }
    else
    {
        if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
        {
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        }
        else if (IsPlayerDefeated(gBattleOutcome) == TRUE)
        {
            SetMainCallback2(CB2_WhiteOut);
        }
        else
        {
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
			if (FlagGet(FLAG_NUZLOCKE))
			{
				Nuzlock_PokemonEraser();
			}
            SetBattledTrainerFlag();
            QuestLogEvents_HandleEndTrainerBattle();
        }
    }
}

void CB2_EndWildBattle(void)
{
    CpuFill16(0, (void *)BG_PLTT, BG_PLTT_SIZE);
    ResetOamRange(0, 128);
    if (IsPlayerDefeated(gBattleOutcome) == TRUE)
    {
        SetMainCallback2(CB2_WhiteOut);
    }
    else
    {	
        SetMainCallback2(CB2_ReturnToField);
        gFieldCallback = FieldCB_SafariZoneRanOutOfBalls;
		if (FlagGet(FLAG_NUZLOCKE))
		{
			Nuzlock_PokemonEraser();
			u16 count = VarGet(VAR_WILD_BATTLE_COUNT);
			count++;
			VarSet(VAR_WILD_BATTLE_COUNT, count);

			// Set flag on 2nd battle
			if (count >= 1)
			{
				FlagSet(FLAG_NO_CATCHING);
			}
		}
    }
}

void CB2_EndScriptedWildBattle_2(void)
{
    CpuFill16(0, (void *)BG_PLTT, BG_PLTT_SIZE);
    ResetOamRange(0, 128);
    if (IsPlayerDefeated(gBattleOutcome) == TRUE)
        SetMainCallback2(CB2_WhiteOut);
    else
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
		if (FlagGet(FLAG_NUZLOCKE))
		{
			Nuzlock_PokemonEraser();
		}
}

void CB2_WhiteOut(void)
{
    u8 val;

    if (++gMain.state >= 120)
    {
		if (FlagGet(FLAG_NUZLOCKE))
		{
			CB2_NewGameOld();
		}
        FieldClearVBlankHBlankCallbacks();
        StopMapMusic();
        ResetSafariZoneFlag_();
        DoWhiteOut();
        SetInitialPlayerAvatarStateWithDirection(DIR_NORTH);
        ScriptContext_Init();
        UnlockPlayerFieldControls();
        gFieldCallback = FieldCB_RushInjuredPokemonToCenter;
        val = 0;
        DoMapLoadLoop(&val);
        QuestLog_CutRecording();
        SetFieldVBlankCallback();
        SetMainCallback1(CB1_Overworld);
        SetMainCallback2(CB2_Overworld);
    }
}
void Nuzlock_PokemonEraser(void)
{
    u8 i;
    bool8 erased = FALSE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_HP, NULL) == 0)
        {
            ZeroMonData(&gPlayerParty[i]);
            erased = TRUE;
        }
    }

    if (erased)
    {
        CompactPartySlots();
        CalculatePlayerPartyCount();
    }
}
void RunOnTransitionMapScript(void)
{
	//Reset streaks upon moving to a new map
	gCurrentDexNavChain = 0;
	gFishingStreak = 0;
	gLastFishingSpecies = 0;
	gDontFadeWhite = FALSE;
	ForceClockUpdate();
	MapHeaderRunScriptByTag(3);
	if (FlagGet(FLAG_NUZLOCKE))
	{
		u16 sectionId = Overworld_GetMapHeaderByGroupAndId(
							gSaveBlock1->location.mapGroup,
							gSaveBlock1->location.mapNum)->regionMapSectionId;
	
		if (!FlagGet(FLAG_VISITED_AREA_START + sectionId)) // only clear if unvisited
		{
			FlagSet(FLAG_VISITED_AREA_START + sectionId);
			FlagClear(FLAG_NO_CATCHING);
		}
	}	
}