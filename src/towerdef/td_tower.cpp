#include "towerdef/td_tower.h"

uint16_t tower_get_cost(TowerType type) {
    switch (type) {
        case TOWER_ARROW:  return 85;
        case TOWER_CANNON: return 160;
        case TOWER_ICE:    return 115;
        default: return 0;
    }
}

uint16_t tower_get_upgrade_cost(TowerType type, uint8_t current_level) {
    if (current_level >= 3) return 0;
    uint16_t base = tower_get_cost(type);
    if (current_level == 1) return (base * 80) / 100;
    return (base * 130) / 100;
}

uint16_t tower_get_sell_value(TowerType type, uint8_t level) {
    uint16_t total = tower_get_cost(type);
    if (level >= 2) total += tower_get_upgrade_cost(type, 1);
    if (level >= 3) total += tower_get_upgrade_cost(type, 2);
    return (total * 65) / 100; // 65% de reembolso
}

float tower_get_range(TowerType type, uint8_t level) {
    float bonus = (level - 1) * 8.0f;
    switch (type) {
        case TOWER_ARROW:  return 60.0f + bonus; // Mayor alcance, francotirador veloz
        case TOWER_CANNON: return 44.0f + bonus; // Corto alcance, artillería pesada
        case TOWER_ICE:    return 50.0f + bonus; // Alcance medio de escarcha
        default: return 0.0f;
    }
}

float tower_get_damage(TowerType type, uint8_t level) {
    float mult = (level == 1) ? 1.0f : ((level == 2) ? 1.75f : 2.7f);
    switch (type) {
        case TOWER_ARROW:  return 16.0f * mult;
        case TOWER_CANNON: return 50.0f * mult;
        case TOWER_ICE:    return 14.0f * mult;
        default: return 0.0f;
    }
}

uint32_t tower_get_cooldown(TowerType type, uint8_t level) {
    switch (type) {
        case TOWER_ARROW:  return 400 - (level - 1) * 60;  // 2.5 disparos/segundo
        case TOWER_CANNON: return 1600 - (level - 1) * 120; // Cadencia lenta
        case TOWER_ICE:    return 800 - (level - 1) * 80;   // Cadencia media
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
