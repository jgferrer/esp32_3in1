#include "towerdef/td_enemy.h"
#include "towerdef/td_map_data.h"
#include <math.h>

void enemy_spawn(Enemy& e, EnemyType type, uint8_t wave) {
    e.active = true;
    e.type = type;
    e.x = MAP_WAYPOINTS[0].x;
    e.y = MAP_WAYPOINTS[0].y;
    e.current_wp = 1;
    e.slow_until_ms = 0;

    float wave_mult = 1.0f + (wave - 1) * 0.18f;

    switch (type) {
        case ENEMY_GOBLIN:
            e.max_hp = 45.0f * wave_mult;
            e.speed = 28.0f; // píxeles por segundo
            e.reward_gold = 10;
            break;
        case ENEMY_ORC:
            e.max_hp = 110.0f * wave_mult;
            e.speed = 18.0f;
            e.reward_gold = 18;
            break;
        case ENEMY_GOLEM:
            e.max_hp = 250.0f * wave_mult;
            e.speed = 12.0f;
            e.reward_gold = 30;
            break;
        case ENEMY_BOSS:
            e.max_hp = (wave >= 10 ? 1200.0f : 550.0f);
            e.speed = 9.0f;
            e.reward_gold = 90;
            break;
    }
    e.hp = e.max_hp;
}

bool enemy_update(Enemy& e, float dt_sec, uint32_t now_ms) {
    if (!e.active) return false;

    if (e.current_wp >= NUM_WAYPOINTS) {
        e.active = false;
        return true; // Traspasó la puerta de la fortaleza (daña el reino)
    }

    Point target = MAP_WAYPOINTS[e.current_wp];
    float dx = target.x - e.x;
    float dy = target.y - e.y;
    float dist = sqrtf(dx * dx + dy * dy);

    float effective_speed = e.speed;
    if (now_ms < e.slow_until_ms) {
        effective_speed *= 0.5f; // Ralentizado por hielo
    }

    float move_step = effective_speed * dt_sec;

    if (dist <= move_step || dist < 1.0f) {
        e.x = target.x;
        e.y = target.y;
        e.current_wp++;
        if (e.current_wp >= NUM_WAYPOINTS) {
            e.active = false;
            return true;
        }
    } else {
        e.x += (dx / dist) * move_step;
        e.y += (dy / dist) * move_step;
    }

    return false;
}
