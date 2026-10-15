#include "picross/picross_game.h"
#include "hal/buzzer.h"
#include "hal/led.h"
#include "picross/picross_storage.h"
#include "picross/picross_renderer.h"
#include "launcher/launcher.h"

PicrossGame picross_game;

void PicrossGame::init() {
    state = PICROSS_STATE_MENU;
    current_level_idx = picross_storage_get_last_level();
    if (current_level_idx >= TOTAL_PUZZLES) {
        current_level_idx = 0;
    }
    tool_mode = PICROSS_TOOL_FILL;
    is_dragging = false;
    last_drag_col = -1;
    last_drag_row = -1;
    current_drag_action = PICROSS_DRAG_NONE;
    timer_running = false;
    elapsed_seconds = 0;
}

void PicrossGame::load_level(uint8_t level_idx) {
    if (level_idx >= TOTAL_PUZZLES) level_idx = 0;
    current_level_idx = level_idx;
    picross_storage_set_last_level(current_level_idx);

    const Puzzle& p = PUZZLES[current_level_idx];
    grid_size = p.size;

    if (grid_size == 5) {
        cell_size = 28;
        grid_x = 75;
        grid_y = 92;
        clue_row_x = 10;
        clue_col_y = 40;
    } else {
        cell_size = 16;
        grid_x = 72;
        grid_y = 92;
        clue_row_x = 4;
        clue_col_y = 40;
    }

    for (uint8_t r = 0; r < MAX_GRID_SIZE; r++) {
        for (uint8_t c = 0; c < MAX_GRID_SIZE; c++) {
            player_grid[r][c] = PICROSS_CELL_EMPTY;
        }
        row_satisfied[r] = false;
        col_satisfied[r] = false;
    }

    compute_clues(p, row_clues, row_clue_counts, col_clues, col_clue_counts);

    hints_remaining = 3;
    start_time_ms = millis();
    elapsed_seconds = 0;
    timer_running = true;
    state = PICROSS_STATE_PLAYING;
    tool_mode = PICROSS_TOOL_FILL;
    is_dragging = false;

    update_clue_satisfaction();
    led_playing();
    buzzer_click();
}

void PicrossGame::reset_current_level() {
    for (uint8_t r = 0; r < grid_size; r++) {
        for (uint8_t c = 0; c < grid_size; c++) {
            player_grid[r][c] = PICROSS_CELL_EMPTY;
        }
    }
    update_clue_satisfaction();
    buzzer_erase();
    picross_renderer_draw_screen(true);
}

void PicrossGame::set_tool(PicrossToolMode mode) {
    tool_mode = mode;
    buzzer_click();
}

void PicrossGame::use_hint() {
    if (hints_remaining == 0 || state != PICROSS_STATE_PLAYING) return;

    const Puzzle& p = PUZZLES[current_level_idx];

    for (uint8_t r = 0; r < grid_size; r++) {
        for (uint8_t c = 0; c < grid_size; c++) {
            if (p.solution[r][c] == 1 && player_grid[r][c] != PICROSS_CELL_FILLED) {
                player_grid[r][c] = PICROSS_CELL_FILLED;
                picross_renderer_draw_cell(r, c);
                hints_remaining--;
                elapsed_seconds += 30;
                led_hint();
                buzzer_hint();
                delay(100);
                led_playing();
                update_clue_satisfaction();

                if (check_victory()) {
                    state = PICROSS_STATE_VICTORY;
                    timer_running = false;
                    picross_storage_save_completion(current_level_idx, elapsed_seconds);
                    led_victory();
                    buzzer_victory();
                }
                return;
            }
        }
    }
}

void PicrossGame::update_timer() {
    if (timer_running && state == PICROSS_STATE_PLAYING) {
        uint32_t now = millis();
        if (now - start_time_ms >= 1000) {
            elapsed_seconds += (now - start_time_ms) / 1000;
            start_time_ms = now;
        }
    }
}

void PicrossGame::update_clue_satisfaction() {
    const Puzzle& p = PUZZLES[current_level_idx];

    for (uint8_t r = 0; r < grid_size; r++) {
        bool match = true;
        for (uint8_t c = 0; c < grid_size; c++) {
            bool should_be = (p.solution[r][c] == 1);
            bool is_filled = (player_grid[r][c] == PICROSS_CELL_FILLED);
            if (should_be != is_filled) {
                match = false;
                break;
            }
        }
        row_satisfied[r] = match;
    }

    for (uint8_t c = 0; c < grid_size; c++) {
        bool match = true;
        for (uint8_t r = 0; r < grid_size; r++) {
            bool should_be = (p.solution[r][c] == 1);
            bool is_filled = (player_grid[r][c] == PICROSS_CELL_FILLED);
            if (should_be != is_filled) {
                match = false;
                break;
            }
        }
        col_satisfied[c] = match;
    }
}

bool PicrossGame::check_victory() {
    const Puzzle& p = PUZZLES[current_level_idx];
    for (uint8_t r = 0; r < grid_size; r++) {
        for (uint8_t c = 0; c < grid_size; c++) {
            bool should_be_filled = (p.solution[r][c] == 1);
            bool is_filled = (player_grid[r][c] == PICROSS_CELL_FILLED);
            if (should_be_filled != is_filled) {
                return false;
            }
        }
    }
    return true;
}

void PicrossGame::handle_touch_down(int16_t tx, int16_t ty) {
    if (state == PICROSS_STATE_MENU) {
        // Botón HOME en la cabecera: X = 6 a 50, Y = 6 a 34
        if (tx >= 6 && tx <= 54 && ty >= 4 && ty <= 36) {
            buzzer_click();
            launcher_return_home();
            return;
        }

        // Selector de 15 niveles
        for (uint8_t i = 0; i < TOTAL_PUZZLES; i++) {
            int16_t col = i % 3;
            int16_t row = i / 3;
            int16_t bx = 12 + col * 74;
            int16_t by = 46 + row * 52;
            int16_t bw = 70;
            int16_t bh = 46;

            if (tx >= bx && tx <= bx + bw && ty >= by && ty <= by + bh) {
                buzzer_click();
                load_level(i);
                return;
            }
        }
        return;
    }

    if (state == PICROSS_STATE_VICTORY) {
        if (ty >= 276 && ty <= 312) {
            if (tx >= 16 && tx <= 116) {
                buzzer_click();
                if (current_level_idx < TOTAL_PUZZLES - 1) {
                    load_level(current_level_idx + 1);
                } else {
                    state = PICROSS_STATE_MENU;
                }
            } else if (tx >= 124 && tx <= 224) {
                buzzer_click();
                state = PICROSS_STATE_MENU;
            }
        }
        return;
    }

    if (state == PICROSS_STATE_PLAYING) {
        // 1. Barra Superior
        if (ty >= 4 && ty <= 34) {
            if (tx >= 6 && tx <= 36) {
                buzzer_click();
                if (current_level_idx > 0) load_level(current_level_idx - 1);
                return;
            }
            if (tx >= 42 && tx <= 74) {
                buzzer_click();
                state = PICROSS_STATE_MENU;
                return;
            }
            if (tx >= 80 && tx <= 110) {
                buzzer_click();
                if (current_level_idx < TOTAL_PUZZLES - 1) load_level(current_level_idx + 1);
                return;
            }
            if (tx >= 204 && tx <= 236) {
                bool s = !buzzer_is_enabled();
                buzzer_set_enabled(s);
                picross_storage_set_sound(s);
                if (s) buzzer_click();
                return;
            }
        }

        // 2. Barra Inferior de Herramientas
        if (ty >= 256 && ty <= 286) {
            if (tx >= 8 && tx <= 116) {
                set_tool(PICROSS_TOOL_FILL);
                return;
            }
            if (tx >= 124 && tx <= 232) {
                set_tool(PICROSS_TOOL_CROSS);
                return;
            }
        } else if (ty >= 290 && ty <= 316) {
            if (tx >= 8 && tx <= 116) {
                reset_current_level();
                return;
            }
            if (tx >= 124 && tx <= 232) {
                use_hint();
                return;
            }
        }

        // 3. Cuadrícula de Juego
        int16_t grid_w = grid_size * cell_size;
        int16_t grid_h = grid_size * cell_size;

        if (tx >= grid_x && tx < grid_x + grid_w && ty >= grid_y && ty < grid_y + grid_h) {
            int16_t c = (tx - grid_x) / cell_size;
            int16_t r = (ty - grid_y) / cell_size;

            if (c >= 0 && c < grid_size && r >= 0 && r < grid_size) {
                is_dragging = true;
                last_drag_col = c;
                last_drag_row = r;

                if (tool_mode == PICROSS_TOOL_FILL) {
                    if (player_grid[r][c] == PICROSS_CELL_FILLED) {
                        current_drag_action = PICROSS_DRAG_CLEAR;
                        player_grid[r][c] = PICROSS_CELL_EMPTY;
                        buzzer_erase();
                    } else {
                        current_drag_action = PICROSS_DRAG_SET_FILL;
                        player_grid[r][c] = PICROSS_CELL_FILLED;
                        buzzer_fill();
                    }
                } else {
                    if (player_grid[r][c] == PICROSS_CELL_CROSSED) {
                        current_drag_action = PICROSS_DRAG_CLEAR;
                        player_grid[r][c] = PICROSS_CELL_EMPTY;
                        buzzer_erase();
                    } else {
                        current_drag_action = PICROSS_DRAG_SET_CROSS;
                        player_grid[r][c] = PICROSS_CELL_CROSSED;
                        buzzer_cross();
                    }
                }

                picross_renderer_draw_cell(r, c);
                update_clue_satisfaction();
            }
        }
    }
}

void PicrossGame::handle_touch_move(int16_t tx, int16_t ty) {
    if (state != PICROSS_STATE_PLAYING || !is_dragging) return;

    int16_t grid_w = grid_size * cell_size;
    int16_t grid_h = grid_size * cell_size;

    if (tx >= grid_x && tx < grid_x + grid_w && ty >= grid_y && ty < grid_y + grid_h) {
        int16_t c = (tx - grid_x) / cell_size;
        int16_t r = (ty - grid_y) / cell_size;

        if (c >= 0 && c < grid_size && r >= 0 && r < grid_size) {
            if (c != last_drag_col || r != last_drag_row) {
                last_drag_col = c;
                last_drag_row = r;

                if (current_drag_action == PICROSS_DRAG_SET_FILL) {
                    if (player_grid[r][c] != PICROSS_CELL_FILLED) {
                        player_grid[r][c] = PICROSS_CELL_FILLED;
                        picross_renderer_draw_cell(r, c);
                        buzzer_fill();
                    }
                } else if (current_drag_action == PICROSS_DRAG_SET_CROSS) {
                    if (player_grid[r][c] != PICROSS_CELL_CROSSED) {
                        player_grid[r][c] = PICROSS_CELL_CROSSED;
                        picross_renderer_draw_cell(r, c);
                        buzzer_cross();
                    }
                } else if (current_drag_action == PICROSS_DRAG_CLEAR) {
                    if (player_grid[r][c] != PICROSS_CELL_EMPTY) {
                        player_grid[r][c] = PICROSS_CELL_EMPTY;
                        picross_renderer_draw_cell(r, c);
                        buzzer_erase();
                    }
                }

                update_clue_satisfaction();
            }
        }
    }
}

void PicrossGame::handle_touch_up() {
    if (is_dragging) {
        is_dragging = false;
        last_drag_col = -1;
        last_drag_row = -1;
        current_drag_action = PICROSS_DRAG_NONE;

        update_clue_satisfaction();

        if (check_victory()) {
            state = PICROSS_STATE_VICTORY;
            timer_running = false;
            picross_storage_save_completion(current_level_idx, elapsed_seconds);
            led_victory();
            buzzer_victory();
        }
    }
}
