#ifndef PICROSS_GAME_H
#define PICROSS_GAME_H

#include <Arduino.h>
#include "picross/puzzle_data.h"

enum PicrossGameState {
    PICROSS_STATE_MENU,
    PICROSS_STATE_PLAYING,
    PICROSS_STATE_VICTORY
};

enum PicrossToolMode {
    PICROSS_TOOL_FILL,
    PICROSS_TOOL_CROSS
};

enum PicrossCellState : uint8_t {
    PICROSS_CELL_EMPTY   = 0,
    PICROSS_CELL_FILLED  = 1,
    PICROSS_CELL_CROSSED = 2
};

enum PicrossDragAction {
    PICROSS_DRAG_NONE,
    PICROSS_DRAG_SET_FILL,
    PICROSS_DRAG_SET_CROSS,
    PICROSS_DRAG_CLEAR
};

class PicrossGame {
public:
    PicrossGameState state;
    uint8_t current_level_idx;
    PicrossToolMode tool_mode;
    uint8_t hints_remaining;

    uint8_t grid_size;
    PicrossCellState player_grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
    uint8_t row_clues[MAX_GRID_SIZE][MAX_CLUES];
    uint8_t row_clue_counts[MAX_GRID_SIZE];
    uint8_t col_clues[MAX_GRID_SIZE][MAX_CLUES];
    uint8_t col_clue_counts[MAX_GRID_SIZE];

    bool row_satisfied[MAX_GRID_SIZE];
    bool col_satisfied[MAX_GRID_SIZE];

    uint32_t start_time_ms;
    uint32_t elapsed_seconds;
    bool timer_running;

    bool is_dragging;
    int8_t last_drag_col;
    int8_t last_drag_row;
    PicrossDragAction current_drag_action;

    int16_t grid_x;
    int16_t grid_y;
    int16_t cell_size;
    int16_t clue_row_x;
    int16_t clue_col_y;

public:
    void init();
    void load_level(uint8_t level_idx);
    void reset_current_level();
    void use_hint();
    void update_timer();
    void set_tool(PicrossToolMode mode);

    void handle_touch_down(int16_t tx, int16_t ty);
    void handle_touch_move(int16_t tx, int16_t ty);
    void handle_touch_up();

    bool check_victory();
    void update_clue_satisfaction();

    const Puzzle& get_current_puzzle() const { return PUZZLES[current_level_idx]; }
};

extern PicrossGame picross_game;

#endif // PICROSS_GAME_H
