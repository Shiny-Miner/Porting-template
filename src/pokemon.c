#include "../include/global.h"
#include "../include/malloc.h"
#include "../include/battle.h"
#include "../include/battle_anim.h"
#include "../include/battle_controllers.h"
#include "../include/battle_message.h"
#include "../include/battle_setup.h"
#include "../include/battle_tower.h"
#include "../include/data.h"
#include "../include/event_data.h"
#include "../include/evolution_scene.h"
#include "../include/field_specials.h"
#include "../include/item.h"
#include "../include/link.h"
#include "../include/main.h"
#include "../include/overworld.h"
#include "../include/m4a.h"
#include "../include/party_menu.h"
#include "../include/pokedex.h"
#include "../include/pokemon.h"
#include "../include/pokemon_summary_screen.h"
#include "../include/pokemon_storage_system.h"
#include "../include/random.h"
#include "../include/sound.h"
#include "../include/string_util.h"
#include "../include/strings.h"
#include "../include/task.h"
#include "../include/text.h"
#include "../include/util.h"
#include "../include/constants/abilities.h"
#include "../include/constants/battle_move_effects.h"
#include "../include/constants/battle_script_commands.h"
#include "../include/constants/hold_effects.h"
#include "../include/constants/item_effects.h"
#include "../include/constants/items.h"
#include "../include/constants/layouts.h"
#include "../include/constants/moves.h"
#include "../include/constants/songs.h"
#include "../include/constants/trainers.h"

bool8 HealStatusConditions(struct Pokemon *mon, u32 unused, u32 healMask, u8 battleId);

#define PP_UP_SHIFTS(val) val, (val) << 2, (val) << 4, (val) << 6
#define PP_UP_SHIFTS_INV(val) (u8)~(val), (u8)~((val) << 2), (u8)~((val) << 4), (u8)~((val) << 6)

const u8 gPPUpAddValues[MAX_MON_MOVES] = {PP_UP_SHIFTS(1)};
const u8 gPPUpClearMask[MAX_MON_MOVES] = {PP_UP_SHIFTS_INV(3)};

static const u8 sGetMonDataEVConstants[] = 
{
    MON_DATA_HP_EV,
    MON_DATA_ATK_EV,
    MON_DATA_DEF_EV,
    MON_DATA_SPEED_EV,
    MON_DATA_SPDEF_EV,
    MON_DATA_SPATK_EV
};

#define UPDATE_FRIENDSHIP_FROM_ITEM()                                                                   \
{                                                                                                       \
    if (retVal == 0 && friendshipChange == 0)                                                           \
    {                                                                                                   \
        friendshipChange = itemEffect[idx];                                                             \
        friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, NULL);                                        \
        if (friendshipChange > 0 && holdEffect == HOLD_EFFECT_FRIENDSHIP_UP)                            \
            friendship += 150 * friendshipChange / 100;                                                 \
        else                                                                                            \
            friendship += friendshipChange;                                                             \
        if (friendshipChange > 0)                                                                       \
        {                                                                                               \
            if (GetMonData(mon, MON_DATA_POKEBALL, NULL) == ITEM_LUXURY_BALL)                           \
                friendship++;                                                                           \
            if (GetMonData(mon, MON_DATA_MET_LOCATION, NULL) == GetCurrentRegionMapSectionId())         \
                friendship++;                                                                           \
        }                                                                                               \
        if (friendship < 0)                                                                             \
            friendship = 0;                                                                             \
        if (friendship > MAX_FRIENDSHIP)                                                                \
            friendship = MAX_FRIENDSHIP;                                                                \
        SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);                                              \
    }                                                                                                   \
}

// Rewrited function
void CopyPlayerPartyMonToBattleData(u8 battlerId, u8 partyIndex, bool8 resetStats)
{
    u16 *hpSwitchout;
    s32 i;
    u8 nickname[POKEMON_NAME_LENGTH * 2]; // Why is the nickname array here longer in FR/LG?

    gBattleMons[battlerId].species = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPECIES, NULL);
    gBattleMons[battlerId].item = GetMonData(&gPlayerParty[partyIndex], MON_DATA_HELD_ITEM, NULL);

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        gBattleMons[battlerId].moves[i] = GetMonData(&gPlayerParty[partyIndex], MON_DATA_MOVE1 + i, NULL);
        gBattleMons[battlerId].pp[i] = GetMonData(&gPlayerParty[partyIndex], MON_DATA_PP1 + i, NULL);
    }

    gBattleMons[battlerId].ppBonuses = GetMonData(&gPlayerParty[partyIndex], MON_DATA_PP_BONUSES, NULL);
    gBattleMons[battlerId].friendship = GetMonData(&gPlayerParty[partyIndex], MON_DATA_FRIENDSHIP, NULL);
    gBattleMons[battlerId].experience = GetMonData(&gPlayerParty[partyIndex], MON_DATA_EXP, NULL);
    gBattleMons[battlerId].hpIV = GetMonData(&gPlayerParty[partyIndex], MON_DATA_HP_IV, NULL);
    gBattleMons[battlerId].attackIV = GetMonData(&gPlayerParty[partyIndex], MON_DATA_ATK_IV, NULL);
    gBattleMons[battlerId].defenseIV = GetMonData(&gPlayerParty[partyIndex], MON_DATA_DEF_IV, NULL);
    gBattleMons[battlerId].speedIV = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPEED_IV, NULL);
    gBattleMons[battlerId].spAttackIV = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPATK_IV, NULL);
    gBattleMons[battlerId].spDefenseIV = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPDEF_IV, NULL);
    gBattleMons[battlerId].personality = GetMonData(&gPlayerParty[partyIndex], MON_DATA_PERSONALITY, NULL);
    gBattleMons[battlerId].status1 = GetMonData(&gPlayerParty[partyIndex], MON_DATA_STATUS, NULL);
    gBattleMons[battlerId].level = GetMonData(&gPlayerParty[partyIndex], MON_DATA_LEVEL, NULL);
    gBattleMons[battlerId].hp = GetMonData(&gPlayerParty[partyIndex], MON_DATA_HP, NULL);
    gBattleMons[battlerId].maxHP = GetMonData(&gPlayerParty[partyIndex], MON_DATA_MAX_HP, NULL);
    gBattleMons[battlerId].attack = GetMonData(&gPlayerParty[partyIndex], MON_DATA_ATK, NULL);
    gBattleMons[battlerId].defense = GetMonData(&gPlayerParty[partyIndex], MON_DATA_DEF, NULL);
    gBattleMons[battlerId].speed = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPEED, NULL);
    gBattleMons[battlerId].spAttack = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPATK, NULL);
    gBattleMons[battlerId].spDefense = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPDEF, NULL);
    gBattleMons[battlerId].isEgg = GetMonData(&gPlayerParty[partyIndex], MON_DATA_IS_EGG, NULL);
    gBattleMons[battlerId].abilityNum = GetMonData(&gPlayerParty[partyIndex], MON_DATA_ABILITY_NUM, NULL);
    gBattleMons[battlerId].otId = GetMonData(&gPlayerParty[partyIndex], MON_DATA_OT_ID, NULL);
    gBattleMons[battlerId].type1 = gSpeciesInfo[gBattleMons[battlerId].species].types[0];
    gBattleMons[battlerId].type2 = gSpeciesInfo[gBattleMons[battlerId].species].types[1];
    gBattleMons[battlerId].ability = GetAbilityBySpecies(gBattleMons[battlerId].species, gBattleMons[battlerId].abilityNum);
    GetMonData(&gPlayerParty[partyIndex], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gBattleMons[battlerId].nickname, nickname);
    GetMonData(&gPlayerParty[partyIndex], MON_DATA_OT_NAME, gBattleMons[battlerId].otName);

    hpSwitchout = &gBattleStruct->hpOnSwitchout[GetBattlerSide(battlerId)];
    *hpSwitchout = gBattleMons[battlerId].hp;

    if (resetStats)
    {
        for (i = 0; i < NUM_BATTLE_STATS; i++)
            gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGE;

        gBattleMons[battlerId].status2 = 0;
    }

    UpdateSentPokesToOpponentValue(battlerId);
    ClearTemporarySpeciesSpriteData(battlerId, FALSE);
}

// Rewrited function
bool8 PokemonUseItemEffects(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex, bool8 usedByAI)
{
    u32 data;
    s32 friendship;
    s32 cmdIndex;
    bool8 retVal = TRUE;
    const u8 *itemEffect;
    u8 idx = ITEM_EFFECT_ARG_START;
    u32 i;
    s8 friendshipChange = 0;
    u8 holdEffect;
    u8 battleMonId = MAX_BATTLERS_COUNT;
    u16 heldItem;
    u8 val;
    u32 evDelta;

    // Get item hold effect
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);
    if (heldItem == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
            holdEffect = gEnigmaBerries[gBattlerInMenuId].holdEffect;
        else
            holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
    }
    else
    {
        holdEffect = ItemId_GetHoldEffect(heldItem);
    }

    // Get battler id (if relevant)
    gPotentialItemEffectBattler = gBattlerInMenuId;
    if (gMain.inBattle)
    {
        gActiveBattler = gBattlerInMenuId;
        cmdIndex = (GetBattlerSide(gActiveBattler) != B_SIDE_PLAYER);
        while (cmdIndex < gBattlersCount)
        {
            if (gBattlerPartyIndexes[cmdIndex] == partyIndex)
            {
                battleMonId = cmdIndex;
                break;
            }
            cmdIndex += 2;
        }
    }
    else
    {
        gActiveBattler = 0;
        battleMonId = MAX_BATTLERS_COUNT;
    }

    // Skip using the item if it won't do anything
    if (!IS_POKEMON_ITEM(item))
        return TRUE;
    if (gItemEffectTable[item - ITEM_POTION] == NULL && item != ITEM_ENIGMA_BERRY)
        return TRUE;

    // Get item effect
    if (item == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
            itemEffect = gEnigmaBerries[gActiveBattler].itemEffect;
        else
            itemEffect = gSaveBlock1Ptr->enigmaBerry.itemEffect;
    }
    else
    {
        itemEffect = gItemEffectTable[item - ITEM_POTION];
    }

    // Do item effect
    for (cmdIndex = 0; cmdIndex < ITEM_EFFECT_ARG_START; cmdIndex++)
    {
        switch (cmdIndex)
        {

        // Handle ITEM0 effects (infatuation, Dire Hit, X Attack). ITEM0_SACRED_ASH is handled in party_menu.c
        case 0:
            // Cure infatuation
            if ((itemEffect[cmdIndex] & ITEM0_INFATUATION)
                && gMain.inBattle && battleMonId != MAX_BATTLERS_COUNT && (gBattleMons[battleMonId].status2 & STATUS2_INFATUATION))
            {
                gBattleMons[battleMonId].status2 &= ~STATUS2_INFATUATION;
                retVal = FALSE;
            }

            // Dire Hit
            if ((itemEffect[cmdIndex] & ITEM0_DIRE_HIT)
             && !(gBattleMons[gActiveBattler].status2 & STATUS2_FOCUS_ENERGY))
            {
                gBattleMons[gActiveBattler].status2 |= STATUS2_FOCUS_ENERGY;
                retVal = FALSE;
            }

            // X Attack
            if ((itemEffect[cmdIndex] & ITEM0_X_ATTACK)
             && gBattleMons[gActiveBattler].statStages[STAT_ATK] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_ATK] += itemEffect[cmdIndex] & ITEM0_X_ATTACK;
                if (gBattleMons[gActiveBattler].statStages[STAT_ATK] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_ATK] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            break;

        // Handle ITEM1 effects (in-battle stat boosting effects)
        case 1:
            // X Defend
            if ((itemEffect[cmdIndex] & ITEM1_X_DEFEND)
             && gBattleMons[gActiveBattler].statStages[STAT_DEF] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_DEF] += (itemEffect[cmdIndex] & ITEM1_X_DEFEND) >> 4;
                if (gBattleMons[gActiveBattler].statStages[STAT_DEF] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_DEF] = MAX_STAT_STAGE;
                retVal = FALSE;
            }

            // X Speed
            if ((itemEffect[cmdIndex] & ITEM1_X_SPEED)
             && gBattleMons[gActiveBattler].statStages[STAT_SPEED] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_SPEED] += itemEffect[cmdIndex] & ITEM1_X_SPEED;
                if (gBattleMons[gActiveBattler].statStages[STAT_SPEED] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_SPEED] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            break;

        // Handle ITEM2 effects (more stat boosting effects)
        case 2:
            // X Accuracy
            if ((itemEffect[cmdIndex] & ITEM2_X_ACCURACY)
             && gBattleMons[gActiveBattler].statStages[STAT_ACC] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_ACC] += (itemEffect[cmdIndex] & ITEM2_X_ACCURACY) >> 4;
                if (gBattleMons[gActiveBattler].statStages[STAT_ACC] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_ACC] = MAX_STAT_STAGE;
                retVal = FALSE;
            }

            // X Sp Attack
            if ((itemEffect[cmdIndex] & ITEM2_X_SPATK)
             && gBattleMons[gActiveBattler].statStages[STAT_SPATK] < MAX_STAT_STAGE)
            {
                gBattleMons[gActiveBattler].statStages[STAT_SPATK] += itemEffect[cmdIndex] & ITEM2_X_SPATK;
                if (gBattleMons[gActiveBattler].statStages[STAT_SPATK] > MAX_STAT_STAGE)
                    gBattleMons[gActiveBattler].statStages[STAT_SPATK] = MAX_STAT_STAGE;
                retVal = FALSE;
            }
            break;

        // Handle ITEM3 effects (Guard Spec, Rare Candy, cure status)
        case 3:
            // Guard Spec
            if ((itemEffect[cmdIndex] & ITEM3_GUARD_SPEC)
             && gSideTimers[GetBattlerSide(gActiveBattler)].mistTimer == 0)
            {
                gSideTimers[GetBattlerSide(gActiveBattler)].mistTimer = 5;
                retVal = FALSE;
            }

            // Rare Candy
            if ((itemEffect[cmdIndex] & ITEM3_LEVEL_UP)
             && GetMonData(mon, MON_DATA_LEVEL, NULL) != MAX_LEVEL)
            {
                data = gExperienceTables[gSpeciesInfo[GetMonData(mon, MON_DATA_SPECIES, NULL)].growthRate][GetMonData(mon, MON_DATA_LEVEL, NULL) + 1];
                SetMonData(mon, MON_DATA_EXP, &data);
                CalculateMonStats(mon);
                retVal = FALSE;
            }

            // Cure status
            if ((itemEffect[cmdIndex] & ITEM3_SLEEP)
             && HealStatusConditions(mon, partyIndex, STATUS1_SLEEP, battleMonId) == 0)
            {
                if (battleMonId != MAX_BATTLERS_COUNT)
                    gBattleMons[battleMonId].status2 &= ~STATUS2_NIGHTMARE;
                retVal = FALSE;
            }
            if ((itemEffect[cmdIndex] & ITEM3_POISON) && HealStatusConditions(mon, partyIndex, STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER, battleMonId) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_BURN) && HealStatusConditions(mon, partyIndex, STATUS1_BURN, battleMonId) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_FREEZE) && HealStatusConditions(mon, partyIndex, STATUS1_FREEZE, battleMonId) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_PARALYSIS) && HealStatusConditions(mon, partyIndex, STATUS1_PARALYSIS, battleMonId) == 0)
                retVal = FALSE;
            if ((itemEffect[cmdIndex] & ITEM3_CONFUSION)  // heal confusion
             && gMain.inBattle && battleMonId != MAX_BATTLERS_COUNT && (gBattleMons[battleMonId].status2 & STATUS2_CONFUSION))
            {
                gBattleMons[battleMonId].status2 &= ~STATUS2_CONFUSION;
                retVal = FALSE;
            }
            break;

        // Handle ITEM4 effects (Change HP/Atk EVs, HP heal, PP heal, PP up, Revive, and evolution stones)
        case 4:
            val = itemEffect[cmdIndex];

            // PP Up
            if (val & ITEM4_PP_UP)
            {
                val &= ~ITEM4_PP_UP;
                data = (GetMonData(mon, MON_DATA_PP_BONUSES, NULL) & gPPUpGetMask[moveIndex]) >> (moveIndex * 2);
                i = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex);
                if (data < 3 && i > 4)
                {
                    data = GetMonData(mon, MON_DATA_PP_BONUSES, NULL) + gPPUpAddValues[moveIndex];
                    SetMonData(mon, MON_DATA_PP_BONUSES, &data);

                    data = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), data, moveIndex) - i;
                    data = GetMonData(mon, MON_DATA_PP1 + moveIndex, NULL) + data;
                    SetMonData(mon, MON_DATA_PP1 + moveIndex, &data);
                    retVal = FALSE;
                }
            }
            i = 0;

            // Loop through and try each of the remaining ITEM4 effects
            while (val != 0)
            {
                if (val & 1)
                {
                    u16 evCount;
                    s32 r5;
     
                    switch (i)
                    {
                    case 0: // ITEM4_EV_HP
                    case 1: // ITEM4_EV_ATK
                        evCount = GetMonEVCount(mon);

                        // Has EV increase limit already been reached?
                        if (evCount >= MAX_TOTAL_EVS)
                            return TRUE;
                        data = GetMonData(mon, sGetMonDataEVConstants[i], NULL);
                        if (data < EV_ITEM_RAISE_LIMIT)
                        {
                            // Limit the increase
                            if (data + itemEffect[idx] > EV_ITEM_RAISE_LIMIT)
                                evDelta = EV_ITEM_RAISE_LIMIT - (data + itemEffect[idx]) + itemEffect[idx];
                            else
                                evDelta = itemEffect[idx];
                            if (evCount + evDelta > MAX_TOTAL_EVS)
                                evDelta += MAX_TOTAL_EVS - (evCount + evDelta);

                            // Update EVs and stats
                            data += evDelta;
                            SetMonData(mon, sGetMonDataEVConstants[i], &data);
                            CalculateMonStats(mon);
                            idx++;
                            retVal = FALSE;
                        }
                        break;
                    case 2: // ITEM4_HEAL_HP
                        // If Revive, update number of times revive has been used
                        if (val & (ITEM4_REVIVE >> 2))
                        {
                            if (GetMonData(mon, MON_DATA_HP, NULL) != 0)
                            {
                                idx++;
                                break;
                            }
                            if (gMain.inBattle)
                            {
                                if (battleMonId != MAX_BATTLERS_COUNT)
                                {
                                    gAbsentBattlerFlags &= ~gBitTable[battleMonId];
                                    CopyPlayerPartyMonToBattleData(battleMonId, GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battleMonId]), TRUE);
                                    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER && gBattleResults.numRevivesUsed < 255)
                                        gBattleResults.numRevivesUsed++;
                                }
                                else
                                {
                                    gAbsentBattlerFlags &= ~gBitTable[gActiveBattler ^ 2];
                                    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER && gBattleResults.numRevivesUsed < 255)
                                        gBattleResults.numRevivesUsed++;
                                }
                            }
                        }
                        else
                        {
                            if (GetMonData(mon, MON_DATA_HP, NULL) == 0)
                            {
                                idx++;
                                break;
                            }
                        }

                        // Get amount of HP to restore
                        data = itemEffect[idx++];
                        switch (data)
                        {
                        case ITEM6_HEAL_HP_FULL:
                            data = GetMonData(mon, MON_DATA_MAX_HP, NULL) - GetMonData(mon, MON_DATA_HP, NULL);
                            break;
                        case ITEM6_HEAL_HP_HALF:
                            data = GetMonData(mon, MON_DATA_MAX_HP, NULL) / 2;
                            if (data == 0)
                                data = 1;
                            break;
                        case ITEM6_HEAL_HP_LVL_UP:
                            data = gBattleScripting.levelUpHP;
                            break;
                        }

                        // Only restore HP if not at max health
                        if (GetMonData(mon, MON_DATA_MAX_HP, NULL) != GetMonData(mon, MON_DATA_HP, NULL))
                        {
                            if (!usedByAI)
                            {
                                // Restore HP
                                data = GetMonData(mon, MON_DATA_HP, NULL) + data;
                                if (data > GetMonData(mon, MON_DATA_MAX_HP, NULL))
                                    data = GetMonData(mon, MON_DATA_MAX_HP, NULL);
                                SetMonData(mon, MON_DATA_HP, &data);
                                
                                // Update battler (if applicable)
                                if (gMain.inBattle && battleMonId != MAX_BATTLERS_COUNT)
                                {
                                    gBattleMons[battleMonId].hp = data;
                                    if (!(val & (ITEM4_REVIVE >> 2)) && GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
                                    {
                                        if (gBattleResults.numHealingItemsUsed < 255)
                                            gBattleResults.numHealingItemsUsed++;

                                        r5 = gActiveBattler;
                                        gActiveBattler = battleMonId;
                                        BtlController_EmitGetMonData(BUFFER_A, REQUEST_ALL_BATTLE, 0);
                                        MarkBattlerForControllerExec(gActiveBattler);
                                        gActiveBattler = r5;
                                    }
                                }
                            }
                            else
                            {
                                gBattleMoveDamage = -data;
                            }
                            retVal = FALSE;
                        }
                        val &= ~(ITEM4_REVIVE >> 2);
                        break;
                    case 3: // ITEM4_HEAL_PP
                        if (!(val & (ITEM4_HEAL_PP_ONE >> 3)))
                        {
                            // Heal PP for all moves
                            for (r5 = 0; r5 < MAX_MON_MOVES; r5++)
                            {
                                u16 move_id;

                                data = GetMonData(mon, MON_DATA_PP1 + r5, NULL);
                                move_id = GetMonData(mon, MON_DATA_MOVE1 + r5, NULL);
                                if (data != CalculatePPWithBonus(move_id, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), r5))
                                {
                                    data += itemEffect[idx];
                                    move_id = GetMonData(mon, MON_DATA_MOVE1 + r5, NULL);
                                    if (data > CalculatePPWithBonus(move_id, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), r5))
                                    {
                                        move_id = GetMonData(mon, MON_DATA_MOVE1 + r5, NULL);
                                        data = CalculatePPWithBonus(move_id, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), r5);
                                    }
                                    SetMonData(mon, MON_DATA_PP1 + r5, &data);
                                    
                                    // Heal battler PP too (if applicable)
                                    if (gMain.inBattle && battleMonId != MAX_BATTLERS_COUNT && MOVE_IS_PERMANENT(battleMonId, r5))
                                        gBattleMons[battleMonId].pp[r5] = data;

                                    retVal = FALSE;
                                }
                            }
                            idx++;
                        }
                        else
                        {
                            // Heal PP for one move
                            u16 move_id;
                            data = GetMonData(mon, MON_DATA_PP1 + moveIndex, NULL);
                            move_id = GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL);
                            if (data != CalculatePPWithBonus(move_id, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex))
                            {
                                data += itemEffect[idx++];
                                move_id = GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL);
                                if (data > CalculatePPWithBonus(move_id, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex))
                                {
                                    move_id = GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL);
                                    data = CalculatePPWithBonus(move_id, GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex);
                                }
                                SetMonData(mon, MON_DATA_PP1 + moveIndex, &data);
                                
                                // Heal battler PP too (if applicable)
                                if (gMain.inBattle && battleMonId != MAX_BATTLERS_COUNT && MOVE_IS_PERMANENT(battleMonId, moveIndex))
                                    gBattleMons[battleMonId].pp[moveIndex] = data;

                                retVal = FALSE;
                            }
                        }
                        break;

                    // cases 4-6 are ITEM4_HEAL_PP_ONE, ITEM4_PP_UP, and ITEM4_REVIVE, which
                    // are already handled above by other cases or before the loop

                    case 7: // ITEM4_EVO_STONE
                        {
                            u16 targetSpecies = GetEvolutionTargetSpecies(mon, EVO_MODE_ITEM_USE, item);

                            if (targetSpecies != SPECIES_NONE)
                            {
                                BeginEvolutionScene(mon, targetSpecies, FALSE, partyIndex);
                                return FALSE;
                            }
                        }
                        break;
                    }
                }
                i++;
                val >>= 1;
            }
            break;

        // Handle ITEM5 effects (Change Def/SpDef/SpAtk/Speed EVs, PP Max, and friendship changes)
        case 5:
            val = itemEffect[cmdIndex];
            i = 0;

            // Loop through and try each of the ITEM5 effects
            while (val != 0)
            {
                if (val & 1)
                {
                    u16 evCount;

                    switch (i)
                    {
                    case 0: // ITEM5_EV_DEF
                    case 1: // ITEM5_EV_SPEED
                    case 2: // ITEM5_EV_SPDEF
                    case 3: // ITEM5_EV_SPATK
                        evCount = GetMonEVCount(mon);
                        
                        // Has EV increase limit already been reached?
                        if (evCount >= MAX_TOTAL_EVS)
                            return TRUE;
                        data = GetMonData(mon, sGetMonDataEVConstants[i + 2], NULL);
                        if (data < EV_ITEM_RAISE_LIMIT)
                        {
                            // Limit the increase
                            if (data + itemEffect[idx] > EV_ITEM_RAISE_LIMIT)
                                evDelta = EV_ITEM_RAISE_LIMIT - (data + itemEffect[idx]) + itemEffect[idx];
                            else
                                evDelta = itemEffect[idx];
                            if (evCount + evDelta > MAX_TOTAL_EVS)
                                evDelta += MAX_TOTAL_EVS - (evCount + evDelta);
                            
                            // Update EVs and stats
                            data += evDelta;
                            SetMonData(mon, sGetMonDataEVConstants[i + 2], &data);
                            CalculateMonStats(mon);
                            retVal = FALSE;
                            idx++;
                        }
                        break;
                    case 4: // ITEM5_PP_MAX
                        data = (GetMonData(mon, MON_DATA_PP_BONUSES, NULL) & gPPUpGetMask[moveIndex]) >> (moveIndex * 2);
                        
                        // Check if 3 PP Ups have been applied already
                        if (data < 3)
                        {
                            evDelta = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), GetMonData(mon, MON_DATA_PP_BONUSES, NULL), moveIndex);
                            data = GetMonData(mon, MON_DATA_PP_BONUSES, NULL);
                            data &= gPPUpClearMask[moveIndex];
                            data += gPPUpAddValues[moveIndex] * 3; // Apply 3 PP Ups (max)

                            SetMonData(mon, MON_DATA_PP_BONUSES, &data);
                            data = CalculatePPWithBonus(GetMonData(mon, MON_DATA_MOVE1 + moveIndex, NULL), data, moveIndex) - evDelta;
                            data = GetMonData(mon, MON_DATA_PP1 + moveIndex, NULL) + data;
                            SetMonData(mon, MON_DATA_PP1 + moveIndex, &data);
                            retVal = FALSE;
                        }
                        break;
                    case 5: // ITEM5_FRIENDSHIP_LOW
                        // Changes to friendship are given differently depending on
                        // how much friendship the Pokémon already has.
                        // In general, Pokémon with lower friendship receive more,
                        // and Pokémon with higher friendship receive less.
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) < 100)
                            UPDATE_FRIENDSHIP_FROM_ITEM();
                        idx++;
                        break;
                    case 6: // ITEM5_FRIENDSHIP_MID
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) >= 100 && GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) < 200)
                            UPDATE_FRIENDSHIP_FROM_ITEM();
                        idx++;
                        break;
                    case 7: // ITEM5_FRIENDSHIP_HIGH
                        if (GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) >= 200)
                            UPDATE_FRIENDSHIP_FROM_ITEM();
                        idx++;
                        break;
                    }
                }
                i++;
                val >>= 1;
            }
            break;
        }
    }
    return retVal;
}