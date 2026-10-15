#ifndef PD_TYPES_H
#define PD_TYPES_H

#include <Arduino.h>

enum PD_GameState {
    PD_STATE_TITLE,
    PD_STATE_HERO_SELECT,
    PD_STATE_DOOR_SELECT,
    PD_STATE_COMBAT,
    PD_STATE_LOOT,
    PD_STATE_GAME_OVER,
    PD_STATE_VICTORY
};

enum PD_HeroClass {
    PD_HERO_WARRIOR,
    PD_HERO_ROGUE,
    PD_HERO_MAGE
};

enum PD_DieType {
    PD_DICE_YELLOW = 0, // Fuerza (STR)
    PD_DICE_GREEN  = 1, // Agilidad (AGI)
    PD_DICE_BLUE   = 2, // Magia (MAG)
    PD_DICE_HEROIC = 3  // Comodín / Heroico (WILDCARD)
};

struct PD_Die {
    PD_DieType type;
    uint8_t value;    // 1 a 6
    bool placed;      // Si ya está asignado a una casilla
    bool active;      // Si está disponible en la tirada actual
};

struct PD_ChallengeBox {
    PD_DieType req_type;       // Color requerido (o PD_DICE_HEROIC para cualquier color)
    uint8_t min_value;         // Valor mínimo del dado
    uint8_t penalty_damage;    // Daño al héroe si no se cubre
    bool filled;               // Si tiene dado colocado
    int8_t placed_die_idx;     // Índice del dado colocado (-1 si vacía)
};

#define PD_MAX_CHALLENGE_BOXES 5
#define PD_MAX_DICE 10

struct PD_EncounterCard {
    const char* name;
    uint8_t floor;           // 1, 2 o 3 (Jefe)
    bool is_boss;
    uint8_t box_count;
    PD_ChallengeBox boxes[PD_MAX_CHALLENGE_BOXES];
    PD_DieType loot_die_type; // Atributo concedido si se reclama como objeto
};

struct PD_HeroState {
    PD_HeroClass hero_class;
    const char* name;
    int8_t hp;
    int8_t max_hp;
    uint8_t potions;
    uint8_t xp;
    uint8_t level;
    uint8_t base_yellow_dice;
    uint8_t base_green_dice;
    uint8_t base_blue_dice;
    uint8_t base_heroic_dice;
    bool skill_used;
};

#endif // PD_TYPES_H
