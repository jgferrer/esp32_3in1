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

    float wave_mult = 1.0f + (wave - 1) * 0.22f;

    switch (type) {
        case ENEMY_GOBLIN:
            e.max_hp = 38.0f * wave_mult;
            e.speed = 36.0f; // Velocistas ágiles (esquivan cañones lentos)
            e.reward_gold = 7;
            break;
        case ENEMY_ORC:
            e.max_hp = 125.0f * wave_mult;
            e.speed = 21.0f;
            e.reward_gold = 14;
            break;
        case ENEMY_GOLEM:
            e.max_hp = 320.0f * wave_mult;
            e.speed = 13.0f; // Tanques pesados blindados
            e.reward_gold = 24;
            break;
        case ENEMY_BOSS:
            e.max_hp = (wave >= 10 ? 2200.0f : 850.0f);
            e.speed = 10.5f;
            e.reward_gold = 75;
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
