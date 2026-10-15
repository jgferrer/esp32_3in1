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
        case PROJ_ARROW:      p.speed = 140.0f; break; // Rápido y certero
        case PROJ_CANNONBALL: p.speed = 72.0f;  break; // Pesado, lento en el aire
        case PROJ_ICE_BOLT:   p.speed = 110.0f; break; // Ráfaga mágica
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
            // Explosión de área con caída de daño
            for (uint8_t i = 0; i < max_enemies; i++) {
                if (enemies[i].active) {
                    float edx = enemies[i].x - p.target_x;
                    float edy = enemies[i].y - p.target_y;
                    float edist = sqrtf(edx * edx + edy * edy);
                    if (edist <= p.splash_radius) {
                        float falloff = 1.0f - (edist / (p.splash_radius * 1.5f));
                        if (falloff < 0.5f) falloff = 0.5f;

                        float dmg = p.damage * falloff;
                        // Goblins ágiles esquivan el 35% del daño por onda expansiva
                        if (enemies[i].type == ENEMY_GOBLIN) {
                            dmg *= 0.65f;
                        }
                        enemies[i].hp -= dmg;
                    }
                }
            }
        } else if (p.type == PROJ_ICE_BOLT) {
            // Escarcha en área: ralentiza a grupos en radio de 26 px
            float frost_radius = 26.0f;
            for (uint8_t i = 0; i < max_enemies; i++) {
                if (enemies[i].active) {
                    float edx = enemies[i].x - p.target_x;
                    float edy = enemies[i].y - p.target_y;
                    float edist = sqrtf(edx * edx + edy * edy);
                    if (edist <= frost_radius) {
                        enemies[i].hp -= (i == p.target_enemy_idx) ? p.damage : (p.damage * 0.5f);
                        enemies[i].slow_until_ms = now_ms + 3200; // 3.2s ralentizado
                    }
                }
            }
        } else {
            // Flecha directa: perfecta contra Goblins, pero Orcos y Gólems tienen armadura
            if (p.target_enemy_idx >= 0 && p.target_enemy_idx < max_enemies) {
                if (enemies[p.target_enemy_idx].active) {
                    float dmg = p.damage;
                    if (enemies[p.target_enemy_idx].type == ENEMY_ORC) {
                        dmg *= 0.75f; // -25% por cota de malla
                    } else if (enemies[p.target_enemy_idx].type == ENEMY_GOLEM) {
                        dmg *= 0.60f; // -40% por piel de roca
                    }
                    enemies[p.target_enemy_idx].hp -= dmg;
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
