#ifndef GAME_PROJECTILE_H
#define GAME_PROJECTILE_H

#include <Arduino.h>
#include "towerdef/td_types.h"

void projectile_spawn(Projectile& p, ProjectileType type, float start_x, float start_y, int16_t target_idx, float target_x, float target_y, float damage, float splash_rad);
bool projectile_update(Projectile& p, Enemy enemies[], uint8_t max_enemies, float dt_sec, uint32_t now_ms);

#endif // GAME_PROJECTILE_H
