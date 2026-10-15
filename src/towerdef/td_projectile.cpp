#include "towerdef/td_projectile.h"
#include <math.h>

void projectile_spawn(Projectile& p, ProjectileType type, float start_x, float start_y, int16_t target_idx, float target_x, float target_y, float damage, float splash_rad) {
    p.active = true;
    p.type = type;
    p.x = start_x;
    p.y = start_y;
    p.target_enemy_idx = target_idx;
    p.target_x = target_x;
    p.target_y = target_y;
    p.damage = damage;
    p.splash_radius = splash_rad;

    switch (type) {
        case PROJ_ARROW:      p.speed = 120.0f; break;
        case PROJ_CANNONBALL: p.speed = 85.0f;  break;
        case PROJ_ICE_BOLT:   p.speed = 100.0f; break;
    }
}

bool projectile_update(Projectile& p, Enemy enemies[], uint8_t max_enemies, float dt_sec, uint32_t now_ms) {
    if (!p.active) return false;

    // Si el objetivo sigue vivo, perseguir su posición actual
    if (p.target_enemy_idx >= 0 && p.target_enemy_idx < max_enemies) {
        if (enemies[p.target_enemy_idx].active) {
            p.target_x = enemies[p.target_enemy_idx].x;
            p.target_y = enemies[p.target_enemy_idx].y;
        }
    }

    float dx = p.target_x - p.x;
    float dy = p.target_y - p.y;
    float dist = sqrtf(dx * dx + dy * dy);

    float step = p.speed * dt_sec;

    if (dist <= step || dist < 4.0f) {
        // Impacto
        p.active = false;

        if (p.type == PROJ_CANNONBALL) {
            // Daño en área (Splash)
            for (uint8_t i = 0; i < max_enemies; i++) {
                if (enemies[i].active) {
                    float edx = enemies[i].x - p.target_x;
                    float edy = enemies[i].y - p.target_y;
                    float edist = sqrtf(edx * edx + edy * edy);
                    if (edist <= p.splash_radius) {
                        enemies[i].hp -= p.damage;
                    }
                }
            }
        } else {
            // Daño directo a objetivo
            if (p.target_enemy_idx >= 0 && p.target_enemy_idx < max_enemies) {
                if (enemies[p.target_enemy_idx].active) {
                    enemies[p.target_enemy_idx].hp -= p.damage;
                    if (p.type == PROJ_ICE_BOLT) {
                        enemies[p.target_enemy_idx].slow_until_ms = now_ms + 3000;
                    }
                }
            }
        }
        return true; // Impactó
    } else {
        p.x += (dx / dist) * step;
        p.y += (dy / dist) * step;
        return false;
    }
}
