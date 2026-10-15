#ifndef GAME_PUZZLE_DATA_H
#define GAME_PUZZLE_DATA_H

#include <Arduino.h>

#define MAX_GRID_SIZE 10
#define MAX_CLUES     5
#define TOTAL_PUZZLES 15

struct Puzzle {
    const char* name;
    uint8_t size; // 5 o 10
    uint8_t solution[MAX_GRID_SIZE][MAX_GRID_SIZE];
    uint16_t color_reveal[MAX_GRID_SIZE][MAX_GRID_SIZE]; // Colores RGB565 revelados al ganar
};

extern const Puzzle PUZZLES[TOTAL_PUZZLES];

void compute_clues(const Puzzle& puzzle,
                   uint8_t row_clues[MAX_GRID_SIZE][MAX_CLUES],
                   uint8_t row_counts[MAX_GRID_SIZE],
                   uint8_t col_clues[MAX_GRID_SIZE][MAX_CLUES],
                   uint8_t col_counts[MAX_GRID_SIZE]);

#endif // GAME_PUZZLE_DATA_H
