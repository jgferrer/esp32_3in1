#include "towerdef/td_map_data.h"

// Camino que recorren los invasores hacia la fortaleza
const Point MAP_WAYPOINTS[NUM_WAYPOINTS] = {
    {  30.0f,  24.0f }, // Entrada norte
    {  30.0f,  80.0f }, // Primer giro hacia el este
    { 200.0f,  80.0f }, // Giro al sur
    { 200.0f, 145.0f }, // Giro al oeste
    {  50.0f, 145.0f }, // Giro al sur
    {  50.0f, 210.0f }, // Giro final hacia el castillo real
    { 180.0f, 210.0f }  // Puerta de la fortaleza
};

// Pedestales estratégicos para erigir torres defensivas
const Point INITIAL_PLOTS[NUM_PLOTS] = {
    { 115.0f,  52.0f }, // Torre norte
    {  80.0f, 112.0f }, // Flanco central izquierdo
    { 150.0f, 112.0f }, // Flanco central derecho
    { 220.0f, 112.0f }, // Extremo derecho
    {  20.0f, 178.0f }, // Extremo izquierdo
    { 115.0f, 178.0f }, // Flanco sur
    { 205.0f, 178.0f }, // Guardia derecha
    { 115.0f, 235.0f }  // Última defensa antes del castillo
};
