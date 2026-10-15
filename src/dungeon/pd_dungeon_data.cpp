#include "dungeon/pd_dungeon_data.h"

const PD_EncounterCard PD_ENCOUNTERS_FLOOR_1[PD_TOTAL_ENCOUNTERS_F1] = {
    {
        "RATA GIGANTE", 1, false, 2,
        {
            { PD_DICE_YELLOW, 2, 1, false, -1 },
            { PD_DICE_GREEN,  3, 1, false, -1 }
        },
        PD_DICE_GREEN
    },
    {
        "TRAMPA FLECHAS", 1, false, 2,
        {
            { PD_DICE_GREEN,  4, 2, false, -1 },
            { PD_DICE_BLUE,   2, 1, false, -1 }
        },
        PD_DICE_GREEN
    },
    {
        "DUENDE LADRON", 1, false, 2,
        {
            { PD_DICE_GREEN,  3, 1, false, -1 },
            { PD_DICE_YELLOW, 3, 1, false, -1 }
        },
        PD_DICE_YELLOW
    },
    {
        "ESQUELETO GUARDIA", 1, false, 2,
        {
            { PD_DICE_YELLOW, 4, 1, false, -1 },
            { PD_DICE_BLUE,   3, 1, false, -1 }
        },
        PD_DICE_YELLOW
    },
    {
        "POZO DE PINCHOS", 1, false, 2,
        {
            { PD_DICE_GREEN,  5, 2, false, -1 },
            { PD_DICE_YELLOW, 2, 1, false, -1 }
        },
        PD_DICE_BLUE
    }
};

const PD_EncounterCard PD_ENCOUNTERS_FLOOR_2[PD_TOTAL_ENCOUNTERS_F2] = {
    {
        "ORCO BERSERKER", 2, false, 3,
        {
            { PD_DICE_YELLOW, 5, 2, false, -1 },
            { PD_DICE_YELLOW, 4, 1, false, -1 },
            { PD_DICE_GREEN,  3, 1, false, -1 }
        },
        PD_DICE_YELLOW
    },
    {
        "ESPECTRO SOMBRA", 2, false, 3,
        {
            { PD_DICE_BLUE,   5, 2, false, -1 },
            { PD_DICE_GREEN,  4, 1, false, -1 },
            { PD_DICE_BLUE,   3, 1, false, -1 }
        },
        PD_DICE_BLUE
    },
    {
        "GOLEM DE ROCA", 2, false, 3,
        {
            { PD_DICE_YELLOW, 6, 2, false, -1 },
            { PD_DICE_YELLOW, 5, 2, false, -1 },
            { PD_DICE_BLUE,   2, 1, false, -1 }
        },
        PD_DICE_YELLOW
    },
    {
        "TRAMPA DE LLAMAS", 2, false, 2,
        {
            { PD_DICE_GREEN,  5, 2, false, -1 },
            { PD_DICE_BLUE,   4, 2, false, -1 }
        },
        PD_DICE_BLUE
    },
    {
        "NIGROMANTE OSCURO", 2, false, 3,
        {
            { PD_DICE_BLUE,   6, 2, false, -1 },
            { PD_DICE_YELLOW, 4, 1, false, -1 },
            { PD_DICE_GREEN,  4, 1, false, -1 }
        },
        PD_DICE_BLUE
    }
};

const PD_EncounterCard PD_BOSS_ENCOUNTER = {
    "DRAGON ANCESTRAL", 3, true, 5,
    {
        { PD_DICE_YELLOW, 6, 2, false, -1 },
        { PD_DICE_YELLOW, 5, 2, false, -1 },
        { PD_DICE_GREEN,  5, 2, false, -1 },
        { PD_DICE_BLUE,   6, 2, false, -1 },
        { PD_DICE_BLUE,   5, 2, false, -1 }
    },
    PD_DICE_HEROIC
};

PD_EncounterCard pd_dungeon_get_random_card(uint8_t floor) {
    if (floor >= 3) {
        return PD_BOSS_ENCOUNTER;
    } else if (floor == 2) {
        uint8_t idx = rand() % PD_TOTAL_ENCOUNTERS_F2;
        return PD_ENCOUNTERS_FLOOR_2[idx];
    } else {
        uint8_t idx = rand() % PD_TOTAL_ENCOUNTERS_F1;
        return PD_ENCOUNTERS_FLOOR_1[idx];
    }
}
