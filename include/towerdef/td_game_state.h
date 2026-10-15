#ifndef TD_GAME_STATE_H
#define TD_GAME_STATE_H

#include <Arduino.h>
#include "config.h"
#include "towerdef/td_types.h"
#include "towerdef/td_map_data.h"
#include "towerdef/td_tower.h"
#include "towerdef/td_enemy.h"
#include "towerdef/td_projectile.h"

enum TDGameState {
    TD_STATE_TITLE,
    TD_STATE_PLAYING,
    TD_STATE_PAUSED,
    TD_STATE_VICTORY,
    TD_STATE_GAME_OVER
};

class TowerDefGame {
public:
    TDGameState state;
    SelectedTool selected_tool;
    int8_t selected_plot_idx;

    uint8_t wave;
    int8_t lives;
    uint32_t gold;
    uint32_t score;
    uint8_t game_speed;

    bool wave_in_progress;
    uint8_t enemies_to_spawn;
    uint8_t enemy_spawn_type_count;
    uint32_t last_spawn_ms;
    uint32_t spawn_interval_ms;

    uint32_t lightning_cooldown_ms;
    uint32_t lightning_last_used_ms;
    bool lightning_anim_active;
    Point lightning_target;
    uint32_t lightning_anim_end_ms;

    TowerPlot plots[NUM_PLOTS];
    Enemy enemies[MAX_ENEMIES];
    Projectile projectiles[MAX_PROJECTILES];
    Particle particles[MAX_PARTICLES];

public:
    void init();
    void start_new_game();
    void start_next_wave();
    void update(float dt_sec);

    void handle_touch(int16_t tx, int16_t ty);
    void trigger_lightning(float x, float y);

    void build_tower(uint8_t plot_idx, TowerType type);
    void upgrade_tower(uint8_t plot_idx);
    void sell_tower(uint8_t plot_idx);

    void add_particles(float x, float y, uint16_t color, uint8_t count = 4);
    bool is_lightning_ready() const;
    uint32_t get_lightning_cd_remaining_sec() const;

private:
    void update_wave_spawning(uint32_t now_ms);
    void update_towers(uint32_t now_ms);
    void update_enemies(float dt_sec, uint32_t now_ms);
    void update_projectiles(float dt_sec, uint32_t now_ms);
    void update_particles(float dt_sec);
    void check_wave_completion();
};

extern TowerDefGame td_game;

#endif // TD_GAME_STATE_H
