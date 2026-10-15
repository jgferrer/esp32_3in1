#include "towerdef/td_game_state.h"
#include "towerdef/td_renderer.h"
#include "hal/buzzer.h"
#include "hal/led.h"
#include "towerdef/td_storage.h"
#include "launcher/launcher.h"
#include <math.h>

TowerDefGame td_game;

void TowerDefGame::init() {
    state = TD_STATE_TITLE;
    td_renderer_reset();
    selected_tool = TOOL_NONE;
    selected_plot_idx = -1;
    wave = 1;
    lives = INITIAL_LIVES;
    gold = INITIAL_GOLD;
    score = 0;
    game_speed = 1;
    wave_in_progress = false;
    enemies_to_spawn = 0;
    lightning_cooldown_ms = 10000;
    lightning_last_used_ms = 0;
    lightning_anim_active = false;
    end_state_time_ms = 0;

    for (uint8_t i = 0; i < NUM_PLOTS; i++) {
        plots[i].x = (int16_t)INITIAL_PLOTS[i].x;
        plots[i].y = (int16_t)INITIAL_PLOTS[i].y;
        plots[i].radius = 12;
        plots[i].type = TOWER_NONE;
        plots[i].level = 1;
        plots[i].last_fire_ms = 0;
        plots[i].target_enemy_idx = -1;
    }

    for (uint8_t i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    for (uint8_t i = 0; i < MAX_PROJECTILES; i++) projectiles[i].active = false;
    for (uint8_t i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
}

void TowerDefGame::start_new_game() {
    init();
    state = TD_STATE_PLAYING;
    lives = INITIAL_LIVES;
    gold = INITIAL_GOLD;
    wave = 1;
    score = 0;
    game_speed = 1;
    selected_tool = TOOL_NONE;
    selected_plot_idx = -1;
    wave_in_progress = false;

    td_renderer_reset();

    led_safe();
    buzzer_build();
}

void TowerDefGame::start_next_wave() {
    if (wave_in_progress || state != TD_STATE_PLAYING) return;

    wave_in_progress = true;
    enemies_to_spawn = 7 + (wave * 2);
    enemy_spawn_type_count = 0;
    spawn_interval_ms = 1300 - (wave * 35);
    if (spawn_interval_ms < 650) spawn_interval_ms = 650;
    last_spawn_ms = millis() - spawn_interval_ms;

    buzzer_click();
}

void TowerDefGame::build_tower(uint8_t plot_idx, TowerType type) {
    if (plot_idx >= NUM_PLOTS) return;
    uint16_t cost = tower_get_cost(type);
    if (gold >= cost && plots[plot_idx].type == TOWER_NONE) {
        gold -= cost;
        plots[plot_idx].type = type;
        plots[plot_idx].level = 1;
        plots[plot_idx].last_fire_ms = millis();
        selected_tool = TOOL_NONE;
        selected_plot_idx = plot_idx;
        buzzer_build();
        add_particles(plots[plot_idx].x, plots[plot_idx].y, COLOR_WHITE, 6);
    } else {
        buzzer_enemy_breach();
    }
}

void TowerDefGame::upgrade_tower(uint8_t plot_idx) {
    if (plot_idx >= NUM_PLOTS) return;
    if (plots[plot_idx].type == TOWER_NONE || plots[plot_idx].level >= 3) return;

    uint16_t cost = tower_get_upgrade_cost(plots[plot_idx].type, plots[plot_idx].level);
    if (gold >= cost) {
        gold -= cost;
        plots[plot_idx].level++;
        buzzer_build();
        add_particles(plots[plot_idx].x, plots[plot_idx].y, COLOR_GOLD_TXT, 8);
    }
}

void TowerDefGame::sell_tower(uint8_t plot_idx) {
    if (plot_idx >= NUM_PLOTS) return;
    if (plots[plot_idx].type == TOWER_NONE) return;

    uint16_t refund = tower_get_sell_value(plots[plot_idx].type, plots[plot_idx].level);
    gold += refund;
    plots[plot_idx].type = TOWER_NONE;
    plots[plot_idx].level = 1;
    selected_plot_idx = -1;
    buzzer_coin();
    add_particles(plots[plot_idx].x, plots[plot_idx].y, COLOR_PATH, 5);
}

void TowerDefGame::trigger_lightning(float x, float y) {
    if (!is_lightning_ready()) return;

    lightning_last_used_ms = millis();
    lightning_anim_active = true;
    lightning_target.x = x;
    lightning_target.y = y;
    lightning_anim_end_ms = millis() + 350;

    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            float dx = enemies[i].x - x;
            float dy = enemies[i].y - y;
            if (sqrtf(dx * dx + dy * dy) <= 42.0f) {
                enemies[i].hp -= 140.0f;
            }
        }
    }

    buzzer_thunder_strike();
    led_thunder();
    add_particles(x, y, 0xFFE0, 10);
    selected_tool = TOOL_NONE;
}

bool TowerDefGame::is_lightning_ready() const {
    return (millis() - lightning_last_used_ms >= lightning_cooldown_ms);
}

uint32_t TowerDefGame::get_lightning_cd_remaining_sec() const {
    uint32_t elapsed = millis() - lightning_last_used_ms;
    if (elapsed >= lightning_cooldown_ms) return 0;
    return (lightning_cooldown_ms - elapsed + 999) / 1000;
}

void TowerDefGame::add_particles(float x, float y, uint16_t color, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        for (uint8_t p = 0; p < MAX_PARTICLES; p++) {
            if (!particles[p].active) {
                particles[p].active = true;
                particles[p].x = x;
                particles[p].y = y;
                float angle = ((float)(rand() % 360)) * (3.14159f / 180.0f);
                float spd = 15.0f + (rand() % 30);
                particles[p].vx = cosf(angle) * spd;
                particles[p].vy = sinf(angle) * spd;
                particles[p].color = color;
                particles[p].life = 0;
                particles[p].max_life = 8 + (rand() % 8);
                break;
            }
        }
    }
}

void TowerDefGame::update_wave_spawning(uint32_t now_ms) {
    if (!wave_in_progress || enemies_to_spawn == 0) return;

    if (now_ms - last_spawn_ms >= spawn_interval_ms) {
        last_spawn_ms = now_ms;

        int16_t free_slot = -1;
        for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) {
                free_slot = i;
                break;
            }
        }

        if (free_slot >= 0) {
            EnemyType t = ENEMY_GOBLIN;
            enemy_spawn_type_count++;

            if ((wave == 5 || wave == 10) && enemies_to_spawn == 1) {
                t = ENEMY_BOSS;
            } else if (wave == 1) {
                // Oleada 1: Solo Goblins ágiles
                t = ENEMY_GOBLIN;
            } else if (wave == 2) {
                // Oleada 2: Goblins rápidos con algunos Orcos
                t = (enemy_spawn_type_count % 3 == 0) ? ENEMY_ORC : ENEMY_GOBLIN;
            } else if (wave == 3 || wave == 4) {
                // Asaltos combinados con primeros Gólems
                if (enemy_spawn_type_count % 5 == 0) t = ENEMY_GOLEM;
                else if (enemy_spawn_type_count % 2 == 0) t = ENEMY_ORC;
                else t = ENEMY_GOBLIN;
            } else if (wave >= 5 && wave <= 7) {
                // Frentes blindados
                if (enemy_spawn_type_count % 4 == 0) t = ENEMY_GOLEM;
                else if (enemy_spawn_type_count % 3 == 0) t = ENEMY_ORC;
                else t = ENEMY_GOBLIN;
            } else {
                // Oleadas 8 a 10: Invasión masiva pesada
                if (enemy_spawn_type_count % 3 == 0) t = ENEMY_GOLEM;
                else if (enemy_spawn_type_count % 2 == 0) t = ENEMY_ORC;
                else t = ENEMY_GOBLIN;
            }

            enemy_spawn(enemies[free_slot], t, wave);
            enemies_to_spawn--;
        }
    }
}

void TowerDefGame::update_towers(uint32_t now_ms) {
    for (uint8_t t = 0; t < NUM_PLOTS; t++) {
        if (plots[t].type == TOWER_NONE) continue;

        uint32_t cd = tower_get_cooldown(plots[t].type, plots[t].level);
        if (now_ms - plots[t].last_fire_ms < cd) continue;

        float range = tower_get_range(plots[t].type, plots[t].level);
        float range_sq = range * range;

        int16_t best_idx = -1;
        int16_t max_wp = -1;
        float best_dist = 9999.0f;

        for (uint8_t e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active) continue;

            float dx = enemies[e].x - plots[t].x;
            float dy = enemies[e].y - plots[t].y;
            float dist_sq = dx * dx + dy * dy;

            if (dist_sq <= range_sq) {
                if (enemies[e].current_wp > max_wp) {
                    max_wp = enemies[e].current_wp;
                    best_idx = e;
                    best_dist = dist_sq;
                } else if (enemies[e].current_wp == max_wp && dist_sq < best_dist) {
                    best_idx = e;
                    best_dist = dist_sq;
                }
            }
        }

        if (best_idx >= 0) {
            plots[t].last_fire_ms = now_ms;
            plots[t].target_enemy_idx = best_idx;

            for (uint8_t p = 0; p < MAX_PROJECTILES; p++) {
                if (!projectiles[p].active) {
                    ProjectileType pt = PROJ_ARROW;
                    float splash = 0.0f;

                    if (plots[t].type == TOWER_CANNON) {
                        pt = PROJ_CANNONBALL;
                        splash = 24.0f + (plots[t].level * 6.0f);
                    } else if (plots[t].type == TOWER_ICE) {
                        pt = PROJ_ICE_BOLT;
                    }

                    float dmg = tower_get_damage(plots[t].type, plots[t].level);
                    projectile_spawn(projectiles[p], pt, plots[t].x, plots[t].y,
                                     best_idx, enemies[best_idx].x, enemies[best_idx].y,
                                     dmg, splash);

                    if (plots[t].type == TOWER_ARROW) buzzer_shoot_arrow();
                    else if (plots[t].type == TOWER_CANNON) buzzer_cannon_boom();
                    else if (plots[t].type == TOWER_ICE) buzzer_ice_beam();

                    break;
                }
            }
        }
    }
}

void TowerDefGame::update_enemies(float dt_sec, uint32_t now_ms) {
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        bool reached_castle = enemy_update(enemies[i], dt_sec, now_ms);
        if (reached_castle) {
            lives--;
            buzzer_enemy_breach();
            led_breach();

            if (lives <= 0) {
                lives = 0;
                state = TD_STATE_GAME_OVER;
                end_state_time_ms = millis();
                buzzer_game_over();
                td_storage_save_high_score(score);
                td_storage_save_max_wave(wave);
                return;
            }
        } else if (enemies[i].hp <= 0.0f) {
            enemies[i].active = false;
            gold += enemies[i].reward_gold;
            score += enemies[i].reward_gold * 10;
            buzzer_coin();
            add_particles(enemies[i].x, enemies[i].y, COLOR_GOLD_TXT, 5);
        }
    }
}

void TowerDefGame::update_projectiles(float dt_sec, uint32_t now_ms) {
    for (uint8_t i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) continue;

        bool hit = projectile_update(projectiles[i], enemies, MAX_ENEMIES, dt_sec, now_ms);
        if (hit) {
            uint16_t p_color = (projectiles[i].type == PROJ_CANNONBALL) ? 0xFD20 :
                               ((projectiles[i].type == PROJ_ICE_BOLT) ? 0x07FF : 0xFFFF);
            add_particles(projectiles[i].x, projectiles[i].y, p_color, 4);
        }
    }
}

void TowerDefGame::update_particles(float dt_sec) {
    for (uint8_t i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;

        particles[i].x += particles[i].vx * dt_sec;
        particles[i].y += particles[i].vy * dt_sec;
        particles[i].life++;

        if (particles[i].life >= particles[i].max_life) {
            particles[i].active = false;
        }
    }
}

void TowerDefGame::check_wave_completion() {
    if (!wave_in_progress || enemies_to_spawn > 0) return;

    bool any_active = false;
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            any_active = true;
            break;
        }
    }

    if (!any_active) {
        wave_in_progress = false;
        uint16_t bonus = 25 + (wave * 8);
        gold += bonus;
        score += wave * 100;
        buzzer_build();

        if (wave >= TOTAL_WAVES) {
            state = TD_STATE_VICTORY;
            end_state_time_ms = millis();
            buzzer_victory();
            led_victory();
            td_storage_save_high_score(score);
            td_storage_save_max_wave(wave);
        } else {
            wave++;
        }
    }
}

void TowerDefGame::update(float dt_sec) {
    if (state != TD_STATE_PLAYING) return;

    float effective_dt = dt_sec * (float)game_speed;
    uint32_t now_ms = millis();

    if (lightning_anim_active && now_ms > lightning_anim_end_ms) {
        lightning_anim_active = false;
        led_safe();
    }

    update_wave_spawning(now_ms);
    update_towers(now_ms);
    update_enemies(effective_dt, now_ms);
    update_projectiles(effective_dt, now_ms);
    update_particles(effective_dt);
    check_wave_completion();
}

void TowerDefGame::handle_touch(int16_t tx, int16_t ty) {
    if (state == TD_STATE_TITLE) {
        // Botón SALIR en pantalla de título: X = 30 a 210, Y = 275 a 305
        if (ty >= 275 && ty <= 310) {
            launcher_return_home();
            return;
        }
        start_new_game();
        return;
    }

    if (state == TD_STATE_GAME_OVER || state == TD_STATE_VICTORY) {
        // Debounce para evitar que toques residuales de combate activen botones
        if (millis() - end_state_time_ms < 800) {
            return;
        }

        // Botones centrados X: 40..200 (con margen 25..215)
        // Botón 1 (Reintentar / Jugar de nuevo): Y: 145..177 (margen 140..178)
        // Botón 2 (Salir al lanzador): Y: 182..212 (margen 180..218)
        if (tx >= 25 && tx <= 215) {
            if (ty >= 140 && ty <= 178) {
                start_new_game();
                return;
            } else if (ty >= 180 && ty <= 218) {
                launcher_return_home();
                return;
            }
        }
        return;
    }

    if (state == TD_STATE_PLAYING) {
        // HUD Superior
        if (ty < BATTLEFIELD_Y_START) {
            // Botón HOME en la esquina superior derecha
            if (tx >= 210) {
                launcher_return_home();
                return;
            } else if (tx >= 170) {
                game_speed = (game_speed == 1) ? 2 : 1;
                buzzer_click();
                return;
            }
            return;
        }

        // Zona de Batalla
        if (ty < BOTTOM_TRAY_Y_START) {
            if (selected_tool == TOOL_SPELL_LIGHTNING) {
                trigger_lightning((float)tx, (float)ty);
                return;
            }

            for (uint8_t i = 0; i < NUM_PLOTS; i++) {
                int16_t dx = tx - plots[i].x;
                int16_t dy = ty - plots[i].y;
                if ((dx * dx + dy * dy) <= (plots[i].radius + 8) * (plots[i].radius + 8)) {
                    buzzer_click();

                    if (plots[i].type == TOWER_NONE) {
                        if (selected_tool == TOOL_BUILD_ARROW) {
                            build_tower(i, TOWER_ARROW);
                        } else if (selected_tool == TOOL_BUILD_CANNON) {
                            build_tower(i, TOWER_CANNON);
                        } else if (selected_tool == TOOL_BUILD_ICE) {
                            build_tower(i, TOWER_ICE);
                        } else {
                            selected_plot_idx = i;
                        }
                    } else {
                        selected_plot_idx = i;
                        selected_tool = TOOL_NONE;
                    }
                    return;
                }
            }

            selected_plot_idx = -1;
            return;
        }

        // Panel Inferior
        if (selected_plot_idx >= 0 && plots[selected_plot_idx].type != TOWER_NONE) {
            if (ty >= 252 && ty <= 286) {
                if (tx >= 8 && tx <= 118) {
                    upgrade_tower(selected_plot_idx);
                    return;
                }
                if (tx >= 124 && tx <= 186) {
                    sell_tower(selected_plot_idx);
                    return;
                }
                if (tx >= 192 && tx <= 234) {
                    selected_plot_idx = -1;
                    buzzer_click();
                    return;
                }
            }
        } else {
            if (ty >= 248 && ty <= 280) {
                if (tx >= 4 && tx <= 78) {
                    selected_tool = (selected_tool == TOOL_BUILD_ARROW) ? TOOL_NONE : TOOL_BUILD_ARROW;
                    selected_plot_idx = -1;
                    buzzer_click();
                    return;
                }
                if (tx >= 82 && tx <= 158) {
                    selected_tool = (selected_tool == TOOL_BUILD_CANNON) ? TOOL_NONE : TOOL_BUILD_CANNON;
                    selected_plot_idx = -1;
                    buzzer_click();
                    return;
                }
                if (tx >= 162 && tx <= 236) {
                    selected_tool = (selected_tool == TOOL_BUILD_ICE) ? TOOL_NONE : TOOL_BUILD_ICE;
                    selected_plot_idx = -1;
                    buzzer_click();
                    return;
                }
            }

            if (ty >= 284 && ty <= 316) {
                if (tx >= 4 && tx <= 74) {
                    if (is_lightning_ready()) {
                        selected_tool = (selected_tool == TOOL_SPELL_LIGHTNING) ? TOOL_NONE : TOOL_SPELL_LIGHTNING;
                        buzzer_click();
                    } else {
                        buzzer_enemy_breach();
                    }
                    return;
                }
                if (tx >= 78 && tx <= 144) {
                    game_speed = (game_speed == 1) ? 2 : 1;
                    buzzer_click();
                    return;
                }
                if (tx >= 148 && tx <= 236) {
                    start_next_wave();
                    return;
                }
            }
        }
    }
}
