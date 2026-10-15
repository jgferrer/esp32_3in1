#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <Arduino.h>

enum GameState {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_VICTORY,
    STATE_GAME_OVER
};

enum SelectedTool {
    TOOL_NONE,
    TOOL_BUILD_ARROW,
    TOOL_BUILD_CANNON,
    TOOL_BUILD_ICE,
    TOOL_SPELL_LIGHTNING
};

enum TowerType {
    TOWER_NONE = 0,
    TOWER_ARROW = 1,
    TOWER_CANNON = 2,
    TOWER_ICE = 3
};

enum EnemyType {
    ENEMY_GOBLIN = 0,
    ENEMY_ORC = 1,
    ENEMY_GOLEM = 2,
    ENEMY_BOSS = 3
};

enum ProjectileType {
    PROJ_ARROW,
    PROJ_CANNONBALL,
    PROJ_ICE_BOLT
};

struct Point {
    float x;
    float y;
};

struct TowerPlot {
    int16_t x;
    int16_t y;
    int8_t radius;
    TowerType type;
    uint8_t level; // 1, 2, 3
    uint32_t last_fire_ms;
    int16_t target_enemy_idx;
};

struct Enemy {
    bool active;
    EnemyType type;
    float x;
    float y;
    int16_t current_wp; // Índice de waypoint hacia el que camina
    float hp;
    float max_hp;
    float speed;
    uint32_t slow_until_ms;
    uint16_t reward_gold;
};

struct Projectile {
    bool active;
    ProjectileType type;
    float x;
    float y;
    float target_x;
    float target_y;
    int16_t target_enemy_idx;
    float speed;
    float damage;
    float splash_radius;
};

struct Particle {
    bool active;
    float x;
    float y;
    float vx;
    float vy;
    uint16_t color;
    uint8_t life;
    uint8_t max_life;
};

#endif // GAME_TYPES_H
