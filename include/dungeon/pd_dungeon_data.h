#ifndef PD_DUNGEON_DATA_H
#define PD_DUNGEON_DATA_H

#include "dungeon/pd_types.h"

#define PD_TOTAL_ENCOUNTERS_F1 5
#define PD_TOTAL_ENCOUNTERS_F2 5
#define PD_TOTAL_BOSSES        1

extern const PD_EncounterCard PD_ENCOUNTERS_FLOOR_1[PD_TOTAL_ENCOUNTERS_F1];
extern const PD_EncounterCard PD_ENCOUNTERS_FLOOR_2[PD_TOTAL_ENCOUNTERS_F2];
extern const PD_EncounterCard PD_BOSS_ENCOUNTER;

PD_EncounterCard pd_dungeon_get_random_card(uint8_t floor);

#endif // PD_DUNGEON_DATA_H
