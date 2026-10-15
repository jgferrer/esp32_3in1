#ifndef GAME_MAP_DATA_H
#define GAME_MAP_DATA_H

#include <Arduino.h>
#include "towerdef/td_types.h"

#define NUM_WAYPOINTS 7
#define NUM_PLOTS     8

extern const Point MAP_WAYPOINTS[NUM_WAYPOINTS];
extern const Point INITIAL_PLOTS[NUM_PLOTS];

#endif // GAME_MAP_DATA_H
