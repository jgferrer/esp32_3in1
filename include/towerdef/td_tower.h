#ifndef GAME_TOWER_H
#define GAME_TOWER_H

#include <Arduino.h>
#include "towerdef/td_types.h"

uint16_t tower_get_cost(TowerType type);
uint16_t tower_get_upgrade_cost(TowerType type, uint8_t current_level);
uint16_t tower_get_sell_value(TowerType type, uint8_t level);

float tower_get_range(TowerType type, uint8_t level);
float tower_get_damage(TowerType type, uint8_t level);
uint32_t tower_get_cooldown(TowerType type, uint8_t level);

const char* tower_get_name(TowerType type);

#endif // GAME_TOWER_H
