#ifndef PD_GAME_STATE_H
#define PD_GAME_STATE_H

#include <Arduino.h>
#include "config.h"
#include "dungeon/pd_types.h"
#include "dungeon/pd_dungeon_data.h"

class PocketDungeonGame {
public:
    PD_GameState state;
    PD_HeroState hero;

    uint8_t floor;
    uint8_t encounters_in_floor;
    uint8_t total_encounters_cleared;
    uint32_t score;

    PD_EncounterCard current_encounter;
    PD_Die dice[PD_MAX_DICE];
    uint8_t dice_count;
    int8_t selected_die_idx;
    int8_t selected_die2_idx;
    bool dice_rolled;
    bool combat_resolved;
    uint32_t end_state_time_ms;

public:
    void init();
    void select_hero(PD_HeroClass h_class);
    void enter_next_encounter();
    void roll_dice();

    bool select_die(int8_t idx);
    bool combine_dice();
    bool place_selected_die_on_box(uint8_t box_idx);
    bool remove_die_from_box(uint8_t box_idx);
    void auto_assign_dice();

    void use_skill();
    void use_potion();
    void resolve_combat();
    void choose_loot(uint8_t option);

    void handle_touch(int16_t tx, int16_t ty);
};

extern PocketDungeonGame pd_game;

#endif // PD_GAME_STATE_H
