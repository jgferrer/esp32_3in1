#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <Arduino.h>
#include "towerdef/td_types.h"

void enemy_spawn(Enemy& e, EnemyType type, uint8_t wave);
bool enemy_update(Enemy& e, float dt_sec, uint32_t now_ms);

#endif // GAME_ENEMY_H
