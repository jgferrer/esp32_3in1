#include "dungeon/pd_game_state.h"
#include "dungeon/pd_storage.h"
#include "dungeon/pd_renderer.h"
#include "launcher/launcher.h"
#include "hal/buzzer.h"
#include "hal/led.h"

PocketDungeonGame pd_game;

void PocketDungeonGame::init() {
    state = PD_STATE_TITLE;
    floor = 1;
    encounters_in_floor = 0;
    total_encounters_cleared = 0;
    score = 0;
    selected_die_idx = -1;
    dice_rolled = false;
    combat_resolved = false;
    dice_count = 0;
    pd_renderer_reset();
    led_dungeon();
}

void PocketDungeonGame::select_hero(PD_HeroClass h_class) {
    hero.hero_class = h_class;
    hero.level = 1;
    hero.xp = 0;
    hero.potions = 1;
    hero.skill_used = false;

    if (h_class == PD_HERO_WARRIOR) {
        hero.name = "GUERRERO";
        hero.max_hp = 12;
        hero.base_yellow_dice = 3;
        hero.base_green_dice  = 1;
        hero.base_blue_dice   = 1;
        hero.base_heroic_dice = 0;
    } else if (h_class == PD_HERO_ROGUE) {
        hero.name = "PICARA";
        hero.max_hp = 10;
        hero.base_yellow_dice = 1;
        hero.base_green_dice  = 3;
        hero.base_blue_dice   = 1;
        hero.base_heroic_dice = 0;
    } else { // PD_HERO_MAGE
        hero.name = "MAGA";
        hero.max_hp = 8;
        hero.base_yellow_dice = 1;
        hero.base_green_dice  = 1;
        hero.base_blue_dice   = 3;
        hero.base_heroic_dice = 0;
    }
    hero.hp = hero.max_hp;

    state = PD_STATE_DOOR_SELECT;
    pd_renderer_reset();
    buzzer_skill();
}

void PocketDungeonGame::enter_next_encounter() {
    encounters_in_floor++;
    if (floor < 3 && encounters_in_floor > 3) {
        floor++;
        encounters_in_floor = 1;
    }

    current_encounter = pd_dungeon_get_random_card(floor);
    for (uint8_t i = 0; i < current_encounter.box_count; i++) {
        current_encounter.boxes[i].filled = false;
        current_encounter.boxes[i].placed_die_idx = -1;
    }

    dice_rolled = false;
    combat_resolved = false;
    selected_die_idx = -1;
    hero.skill_used = false;

    state = PD_STATE_COMBAT;
    pd_renderer_reset();
    led_combat();
    buzzer_click();
}

void PocketDungeonGame::roll_dice() {
    if (dice_rolled) return;

    dice_count = 0;
    for (uint8_t i = 0; i < hero.base_yellow_dice && dice_count < PD_MAX_DICE; i++) {
        dice[dice_count++] = { PD_DICE_YELLOW, (uint8_t)(1 + (rand() % 6)), false, true };
    }
    for (uint8_t i = 0; i < hero.base_green_dice && dice_count < PD_MAX_DICE; i++) {
        dice[dice_count++] = { PD_DICE_GREEN, (uint8_t)(1 + (rand() % 6)), false, true };
    }
    for (uint8_t i = 0; i < hero.base_blue_dice && dice_count < PD_MAX_DICE; i++) {
        dice[dice_count++] = { PD_DICE_BLUE, (uint8_t)(1 + (rand() % 6)), false, true };
    }
    for (uint8_t i = 0; i < hero.base_heroic_dice && dice_count < PD_MAX_DICE; i++) {
        dice[dice_count++] = { PD_DICE_HEROIC, (uint8_t)(1 + (rand() % 6)), false, true };
    }

    dice_rolled = true;
    selected_die_idx = -1;
    buzzer_dice_roll();
}

bool PocketDungeonGame::select_die(int8_t idx) {
    if (idx < 0 || idx >= dice_count) {
        selected_die_idx = -1;
        return false;
    }
    if (dice[idx].placed || !dice[idx].active) {
        return false;
    }
    selected_die_idx = idx;
    buzzer_click();
    return true;
}

bool PocketDungeonGame::place_selected_die_on_box(uint8_t box_idx) {
    if (selected_die_idx < 0 || selected_die_idx >= dice_count) return false;
    if (box_idx >= current_encounter.box_count) return false;
    if (current_encounter.boxes[box_idx].filled) return false;

    PD_Die& d = dice[selected_die_idx];
    PD_ChallengeBox& b = current_encounter.boxes[box_idx];

    bool color_ok = (d.type == PD_DICE_HEROIC || b.req_type == PD_DICE_HEROIC || d.type == b.req_type);
    bool value_ok = (d.value >= b.min_value);

    if (color_ok && value_ok) {
        b.filled = true;
        b.placed_die_idx = selected_die_idx;
        d.placed = true;
        selected_die_idx = -1;
        buzzer_dice_place();
        return true;
    } else {
        buzzer_damage();
        return false;
    }
}

bool PocketDungeonGame::remove_die_from_box(uint8_t box_idx) {
    if (box_idx >= current_encounter.box_count) return false;
    PD_ChallengeBox& b = current_encounter.boxes[box_idx];
    if (!b.filled || b.placed_die_idx < 0) return false;

    dice[b.placed_die_idx].placed = false;
    b.filled = false;
    b.placed_die_idx = -1;
    buzzer_click();
    return true;
}

void PocketDungeonGame::auto_assign_dice() {
    if (!dice_rolled) return;

    for (uint8_t b = 0; b < current_encounter.box_count; b++) {
        if (current_encounter.boxes[b].filled) continue;

        int8_t best_die = -1;
        uint8_t min_surplus = 255;

        for (uint8_t d = 0; d < dice_count; d++) {
            if (dice[d].placed || !dice[d].active) continue;

            bool color_ok = (dice[d].type == PD_DICE_HEROIC || current_encounter.boxes[b].req_type == PD_DICE_HEROIC || dice[d].type == current_encounter.boxes[b].req_type);
            bool value_ok = (dice[d].value >= current_encounter.boxes[b].min_value);

            if (color_ok && value_ok) {
                uint8_t surplus = dice[d].value - current_encounter.boxes[b].min_value;
                if (surplus < min_surplus) {
                    min_surplus = surplus;
                    best_die = d;
                }
            }
        }

        if (best_die >= 0) {
            current_encounter.boxes[b].filled = true;
            current_encounter.boxes[b].placed_die_idx = best_die;
            dice[best_die].placed = true;
        }
    }
    selected_die_idx = -1;
    buzzer_dice_place();
}

void PocketDungeonGame::use_skill() {
    if (hero.skill_used || !dice_rolled) return;

    if (hero.hero_class == PD_HERO_WARRIOR) {
        int8_t target = -1;
        uint8_t min_v = 7;
        for (uint8_t i = 0; i < dice_count; i++) {
            if (!dice[i].placed && dice[i].type == PD_DICE_YELLOW && dice[i].value < min_v) {
                min_v = dice[i].value;
                target = i;
            }
        }
        if (target >= 0) {
            dice[target].value = (dice[target].value + 2 > 6) ? 6 : (dice[target].value + 2);
            hero.skill_used = true;
            buzzer_skill();
        }
    } else if (hero.hero_class == PD_HERO_ROGUE) {
        for (uint8_t i = 0; i < dice_count; i++) {
            if (!dice[i].placed && dice[i].type == PD_DICE_GREEN) {
                dice[i].type = PD_DICE_HEROIC;
                hero.skill_used = true;
                buzzer_skill();
                break;
            }
        }
    } else { // PD_HERO_MAGE
        int8_t target = -1;
        uint8_t min_v = 7;
        for (uint8_t i = 0; i < dice_count; i++) {
            if (!dice[i].placed && dice[i].value < min_v) {
                min_v = dice[i].value;
                target = i;
            }
        }
        if (target >= 0) {
            dice[target].type = PD_DICE_HEROIC;
            dice[target].value = 6;
            hero.skill_used = true;
            buzzer_skill();
        }
    }
}

void PocketDungeonGame::use_potion() {
    if (hero.potions == 0 || hero.hp >= hero.max_hp) return;
    hero.potions--;
    hero.hp += 4;
    if (hero.hp > hero.max_hp) hero.hp = hero.max_hp;
    buzzer_potion();
}

void PocketDungeonGame::resolve_combat() {
    if (!dice_rolled) return;

    int8_t total_damage = 0;
    for (uint8_t i = 0; i < current_encounter.box_count; i++) {
        if (!current_encounter.boxes[i].filled) {
            total_damage += current_encounter.boxes[i].penalty_damage;
        }
    }

    if (total_damage > 0) {
        hero.hp -= total_damage;
        buzzer_damage();
        led_damage();
    } else {
        buzzer_victory();
        led_victory();
    }

    if (hero.hp <= 0) {
        hero.hp = 0;
        state = PD_STATE_GAME_OVER;
        end_state_time_ms = millis();
        pd_storage_save_high_score(score);
        buzzer_game_over();
        return;
    }

    total_encounters_cleared++;
    score += (floor * 100) + (hero.hp * 10);

    if (current_encounter.is_boss) {
        state = PD_STATE_VICTORY;
        end_state_time_ms = millis();
        score += 1000;
        pd_storage_save_high_score(score);
        pd_storage_increment_victories();
        buzzer_victory();
        return;
    }

    state = PD_STATE_LOOT;
    pd_renderer_reset();
}

void PocketDungeonGame::choose_loot(uint8_t option) {
    if (option == 0) {
        if (current_encounter.loot_die_type == PD_DICE_YELLOW) hero.base_yellow_dice++;
        else if (current_encounter.loot_die_type == PD_DICE_GREEN) hero.base_green_dice++;
        else if (current_encounter.loot_die_type == PD_DICE_BLUE) hero.base_blue_dice++;
        else hero.base_heroic_dice++;
        buzzer_skill();
    } else if (option == 1) {
        hero.hp += 3;
        if (hero.hp > hero.max_hp) hero.hp = hero.max_hp;
        if (hero.potions < 2) hero.potions++;
        buzzer_potion();
    } else {
        hero.xp++;
        if (hero.xp % 2 == 0) {
            hero.level++;
            hero.max_hp += 2;
            hero.hp += 2;
            hero.base_heroic_dice++;
        }
        buzzer_dice_place();
    }

    state = PD_STATE_DOOR_SELECT;
    pd_renderer_reset();
    led_dungeon();
}

void PocketDungeonGame::handle_touch(int16_t tx, int16_t ty) {
    // Botón HOME en la barra superior (Y <= 24, X >= 200)
    if (ty <= 24 && tx >= 200) {
        launcher_return_home();
        return;
    }

    // -------------------------------------------------------------
    // PANTALLA DE TÍTULO
    // -------------------------------------------------------------
    if (state == PD_STATE_TITLE) {
        if (ty >= 275 && ty <= 310) {
            launcher_return_home();
            return;
        }
        state = PD_STATE_HERO_SELECT;
        buzzer_click();
        pd_renderer_reset();
        return;
    }

    // -------------------------------------------------------------
    // SELECCIÓN DE HÉROE
    // -------------------------------------------------------------
    if (state == PD_STATE_HERO_SELECT) {
        if (ty >= 55 && ty <= 125) {
            select_hero(PD_HERO_WARRIOR);
        } else if (ty >= 135 && ty <= 205) {
            select_hero(PD_HERO_ROGUE);
        } else if (ty >= 215 && ty <= 285) {
            select_hero(PD_HERO_MAGE);
        }
        return;
    }

    // -------------------------------------------------------------
    // SELECCIÓN DE PUERTA / EXPLORACIÓN
    // -------------------------------------------------------------
    if (state == PD_STATE_DOOR_SELECT) {
        if (tx >= 15 && tx <= 110 && ty >= 75 && ty <= 220) {
            enter_next_encounter();
            return;
        }
        if (tx >= 130 && tx <= 225 && ty >= 75 && ty <= 220) {
            enter_next_encounter();
            return;
        }
        if (ty >= 240 && ty <= 280 && tx >= 60 && tx <= 180) {
            use_potion();
            return;
        }
        return;
    }

    // -------------------------------------------------------------
    // COMBATE
    // -------------------------------------------------------------
    if (state == PD_STATE_COMBAT) {
        if (ty >= 95 && ty <= 170) {
            uint8_t count = current_encounter.box_count;
            int16_t box_w = 40;
            int16_t start_x = 120 - (count * (box_w + 6)) / 2;

            for (uint8_t b = 0; b < count; b++) {
                int16_t bx = start_x + b * (box_w + 6);
                if (tx >= bx && tx <= bx + box_w) {
                    if (current_encounter.boxes[b].filled) {
                        remove_die_from_box(b);
                    } else if (selected_die_idx >= 0) {
                        place_selected_die_on_box(b);
                    }
                    return;
                }
            }
        }

        if (dice_rolled && ty >= 180 && ty <= 250) {
            int16_t die_w = 32;
            int16_t start_x = 12;
            for (uint8_t d = 0; d < dice_count; d++) {
                int16_t dx = start_x + (d % 6) * 37;
                int16_t dy = 198 + (d / 6) * 35;
                if (tx >= dx && tx <= dx + die_w && ty >= dy && ty <= dy + die_w) {
                    select_die(d);
                    return;
                }
            }
        }

        if (ty >= 260 && ty <= 310) {
            if (!dice_rolled) {
                if (tx >= 20 && tx <= 220) {
                    roll_dice();
                }
            } else {
                if (tx >= 8 && tx <= 74) {
                    auto_assign_dice();
                    return;
                }
                if (tx >= 80 && tx <= 152) {
                    use_skill();
                    return;
                }
                if (tx >= 158 && tx <= 232) {
                    resolve_combat();
                    return;
                }
            }
        }
        return;
    }

    // -------------------------------------------------------------
    // FASE DE BOTÍN
    // -------------------------------------------------------------
    if (state == PD_STATE_LOOT) {
        if (ty >= 80 && ty <= 135) {
            choose_loot(0);
        } else if (ty >= 145 && ty <= 200) {
            choose_loot(1);
        } else if (ty >= 210 && ty <= 265) {
            choose_loot(2);
        }
        return;
    }

    // -------------------------------------------------------------
    // FIN DE JUEGO / VICTORIA
    // -------------------------------------------------------------
    if (state == PD_STATE_GAME_OVER || state == PD_STATE_VICTORY) {
        if (millis() - end_state_time_ms < 800) return;
        // Botón 1: REINTENTAR / JUGAR DE NUEVO (Y = 145 a 178)
        if (tx >= 25 && tx <= 215 && ty >= 140 && ty <= 178) {
            init();
            return;
        }
        // Botón 2: SALIR AL LANZADOR (Y = 182 a 218)
        if (tx >= 25 && tx <= 215 && ty >= 180 && ty <= 218) {
            launcher_return_home();
            return;
        }
        return;
    }
}
