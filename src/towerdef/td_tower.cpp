#include "towerdef/td_tower.h"

uint16_t tower_get_cost(TowerType type) {
    switch (type) {
        case TOWER_ARROW:  return 100;
        case TOWER_CANNON: return 150;
        case TOWER_ICE:    return 125;
        default: return 0;
    }
}

uint16_t tower_get_upgrade_cost(TowerType type, uint8_t current_level) {
    if (current_level >= 3) return 0;
    uint16_t base = tower_get_cost(type);
    if (current_level == 1) return (base * 75) / 100;
    return (base * 120) / 100;
}

uint16_t tower_get_sell_value(TowerType type, uint8_t level) {
    uint16_t total = tower_get_cost(type);
    if (level >= 2) total += tower_get_upgrade_cost(type, 1);
    if (level >= 3) total += tower_get_upgrade_cost(type, 2);
    return (total * 70) / 100; // 70% de reembolso
}

float tower_get_range(TowerType type, uint8_t level) {
    float bonus = (level - 1) * 10.0f;
    switch (type) {
        case TOWER_ARROW:  return 52.0f + bonus;
        case TOWER_CANNON: return 48.0f + bonus;
        case TOWER_ICE:    return 50.0f + bonus;
        default: return 0.0f;
    }
}

float tower_get_damage(TowerType type, uint8_t level) {
    float mult = (level == 1) ? 1.0f : ((level == 2) ? 1.7f : 2.6f);
    switch (type) {
        case TOWER_ARROW:  return 18.0f * mult;
        case TOWER_CANNON: return 45.0f * mult;
        case TOWER_ICE:    return 12.0f * mult;
        default: return 0.0f;
    }
}

uint32_t tower_get_cooldown(TowerType type, uint8_t level) {
    uint32_t reduction = (level - 1) * 100;
    switch (type) {
        case TOWER_ARROW:  return 600 - reduction;
        case TOWER_CANNON: return 1400 - reduction;
        case TOWER_ICE:    return 900 - reduction;
        default: return 1000;
    }
}

const char* tower_get_name(TowerType type) {
    switch (type) {
        case TOWER_ARROW:  return "ARQUEROS";
        case TOWER_CANNON: return "CANON";
        case TOWER_ICE:    return "MAGO HIELO";
        default: return "VACIO";
    }
}
