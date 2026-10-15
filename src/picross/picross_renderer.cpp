#include "picross/picross_renderer.h"
#include "hal/display.h"
#include "hal/buzzer.h"
#include "picross/picross_storage.h"
#include "picross/picross_game.h"

#ifndef COLOR_BG
#define COLOR_BG          0x18C3
#endif
#ifndef COLOR_HEADER
#define COLOR_HEADER      0x2126
#endif
#ifndef COLOR_GRID_BG
#define COLOR_GRID_BG     0xF7BE
#endif
#ifndef COLOR_GRID_LINE
#define COLOR_GRID_LINE   0xAD55
#endif
#ifndef COLOR_GRID_MAJOR
#define COLOR_GRID_MAJOR  0x4208
#endif
#ifndef COLOR_FILLED
#define COLOR_FILLED      0x18C3
#endif
#ifndef COLOR_CROSS
#define COLOR_CROSS       0xD185
#endif
#ifndef COLOR_TEXT
#define COLOR_TEXT        0xFFFF
#endif
#ifndef COLOR_TEXT_MUTED
#define COLOR_TEXT_MUTED  0x8410
#endif
#ifndef COLOR_ACCENT
#define COLOR_ACCENT      0x2D7F
#endif
#ifndef COLOR_ACCENT_ACT
#define COLOR_ACCENT_ACT  0xFDE0
#endif
#ifndef COLOR_SUCCESS
#define COLOR_SUCCESS     0x2EA5
#endif
#ifndef COLOR_GOLD
#define COLOR_GOLD        0xFDE0
#endif

static PicrossGameState last_rendered_state = (PicrossGameState)-1;
static uint8_t last_rendered_level = 255;
static PicrossToolMode last_rendered_tool = (PicrossToolMode)-1;
static uint32_t last_rendered_sec = 0xFFFFFFFF;
static uint8_t last_hints_count = 255;

void picross_renderer_init() {
    tft.setTextWrap(false);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
}

static void format_time(uint32_t sec, char* buf, size_t sz) {
    uint32_t m = sec / 60;
    uint32_t s = sec % 60;
    snprintf(buf, sz, "%02u:%02u", m, s);
}

void picross_renderer_draw_cell(uint8_t r, uint8_t c) {
    int16_t x = picross_game.grid_x + c * picross_game.cell_size;
    int16_t y = picross_game.grid_y + r * picross_game.cell_size;
    int16_t sz = picross_game.cell_size;

    PicrossCellState cs = picross_game.player_grid[r][c];

    tft.fillRect(x + 1, y + 1, sz - 1, sz - 1, COLOR_GRID_BG);

    if (cs == PICROSS_CELL_FILLED) {
        int16_t pad = (sz >= 20) ? 2 : 1;
        tft.fillRect(x + pad, y + pad, sz - (pad * 2), sz - (pad * 2), COLOR_FILLED);
        tft.drawFastHLine(x + pad, y + pad, sz - (pad * 2), 0x39E7);
        tft.drawFastVLine(x + pad, y + pad, sz - (pad * 2), 0x39E7);
    } else if (cs == PICROSS_CELL_CROSSED) {
        int16_t pad = (sz >= 20) ? 5 : 3;
        tft.drawLine(x + pad, y + pad, x + sz - pad - 1, y + sz - pad - 1, COLOR_CROSS);
        tft.drawLine(x + pad, y + sz - pad - 1, x + sz - pad - 1, y + pad, COLOR_CROSS);
        if (sz >= 20) {
            tft.drawLine(x + pad + 1, y + pad, x + sz - pad, y + sz - pad - 1, COLOR_CROSS);
            tft.drawLine(x + pad + 1, y + sz - pad - 1, x + sz - pad, y + pad, COLOR_CROSS);
        }
    }
}

static void draw_clues() {
    uint8_t s = picross_game.grid_size;

    tft.fillRect(picross_game.grid_x - 1, picross_game.clue_col_y, s * picross_game.cell_size + 2, picross_game.grid_y - picross_game.clue_col_y - 2, COLOR_BG);

    for (uint8_t c = 0; c < s; c++) {
        uint16_t col_color = picross_game.col_satisfied[c] ? COLOR_TEXT_MUTED : COLOR_TEXT;
        tft.setTextColor(col_color, COLOR_BG);

        int16_t cx = picross_game.grid_x + c * picross_game.cell_size + (picross_game.cell_size / 2);
        uint8_t cnt = picross_game.col_clue_counts[c];

        for (int8_t i = cnt - 1; i >= 0; i--) {
            int8_t pos_from_bottom = (cnt - 1 - i);
            int16_t cy = picross_game.grid_y - 12 - (pos_from_bottom * 10);
            tft.drawCenterString(String(picross_game.col_clues[c][i]).c_str(), cx, cy, &fonts::Font0);
        }
    }

    tft.fillRect(picross_game.clue_row_x, picross_game.grid_y - 1, picross_game.grid_x - picross_game.clue_row_x - 2, s * picross_game.cell_size + 2, COLOR_BG);

    for (uint8_t r = 0; r < s; r++) {
        uint16_t row_color = picross_game.row_satisfied[r] ? COLOR_TEXT_MUTED : COLOR_TEXT;
        tft.setTextColor(row_color, COLOR_BG);

        int16_t cy = picross_game.grid_y + r * picross_game.cell_size + (picross_game.cell_size / 2) - 3;
        uint8_t cnt = picross_game.row_clue_counts[r];

        String s_row = "";
        for (uint8_t i = 0; i < cnt; i++) {
            s_row += String(picross_game.row_clues[r][i]);
            if (i < cnt - 1) s_row += " ";
        }
        tft.drawRightString(s_row.c_str(), picross_game.grid_x - 4, cy, &fonts::Font0);
    }
}

static void draw_grid() {
    uint8_t s = picross_game.grid_size;
    int16_t gw = s * picross_game.cell_size;
    int16_t gh = s * picross_game.cell_size;

    tft.fillRect(picross_game.grid_x, picross_game.grid_y, gw, gh, COLOR_GRID_BG);

    for (uint8_t r = 0; r < s; r++) {
        for (uint8_t c = 0; c < s; c++) {
            picross_renderer_draw_cell(r, c);
        }
    }

    for (uint8_t i = 0; i <= s; i++) {
        int16_t lx = picross_game.grid_x + i * picross_game.cell_size;
        int16_t ly = picross_game.grid_y + i * picross_game.cell_size;

        bool major = (i == 0 || i == s || (s == 10 && i == 5));
        uint16_t color = major ? COLOR_GRID_MAJOR : COLOR_GRID_LINE;

        tft.drawFastVLine(lx, picross_game.grid_y, gh, color);
        tft.drawFastHLine(picross_game.grid_x, ly, gw, color);

        if (major) {
            if (lx > picross_game.grid_x && lx < picross_game.grid_x + gw) tft.drawFastVLine(lx - 1, picross_game.grid_y, gh, color);
            if (ly > picross_game.grid_y && ly < picross_game.grid_y + gh) tft.drawFastHLine(picross_game.grid_x, ly - 1, gw, color);
        }
    }
}

static void draw_header() {
    tft.fillRect(0, 0, SCREEN_WIDTH, 36, COLOR_HEADER);

    const Puzzle& p = picross_game.get_current_puzzle();
    char title[32];
    snprintf(title, sizeof(title), "#%02u: %s (%ux%u)", picross_game.current_level_idx + 1, p.name, p.size, p.size);

    tft.setTextColor(COLOR_GOLD, COLOR_HEADER);
    tft.drawString(title, 8, 4, &fonts::Font0);

    // [ < ]
    tft.fillRoundRect(6, 17, 30, 16, 3, 0x39E7);
    tft.setTextColor(COLOR_TEXT, 0x39E7);
    tft.drawCenterString("<", 21, 21, &fonts::Font0);

    // [ MENU ]
    tft.fillRoundRect(42, 17, 32, 16, 3, 0x39E7);
    tft.drawCenterString("MENU", 58, 21, &fonts::Font0);

    // [ > ]
    tft.fillRoundRect(80, 17, 30, 16, 3, 0x39E7);
    tft.drawCenterString(">", 95, 21, &fonts::Font0);

    // Cronómetro
    char time_str[16];
    format_time(picross_game.elapsed_seconds, time_str, sizeof(time_str));
    tft.fillRoundRect(120, 17, 76, 16, 3, 0x18C3);
    tft.setTextColor(COLOR_ACCENT_ACT, 0x18C3);
    tft.drawCenterString(time_str, 158, 21, &fonts::Font0);

    // Sonido
    bool snd = buzzer_is_enabled();
    tft.fillRoundRect(204, 17, 30, 16, 3, snd ? 0x2EA5 : 0x8410);
    tft.setTextColor(COLOR_TEXT, snd ? 0x2EA5 : 0x8410);
    tft.drawCenterString(snd ? "SND" : "MUT", 219, 21, &fonts::Font0);
}

void picross_renderer_draw_timer_only() {
    if (picross_game.state != PICROSS_STATE_PLAYING) return;
    if (picross_game.elapsed_seconds == last_rendered_sec) return;
    last_rendered_sec = picross_game.elapsed_seconds;

    char time_str[16];
    format_time(picross_game.elapsed_seconds, time_str, sizeof(time_str));
    tft.fillRect(122, 18, 72, 14, 0x18C3);
    tft.setTextColor(COLOR_ACCENT_ACT, 0x18C3);
    tft.drawCenterString(time_str, 158, 21, &fonts::Font0);
}

static void draw_bottom_controls() {
    bool fill_act = (picross_game.tool_mode == PICROSS_TOOL_FILL);
    uint16_t fill_bg = fill_act ? COLOR_ACCENT : 0x2126;
    uint16_t fill_txt = fill_act ? COLOR_GOLD : COLOR_TEXT_MUTED;
    tft.fillRoundRect(8, 256, 108, 28, 4, fill_bg);
    if (fill_act) tft.drawRoundRect(8, 256, 108, 28, 4, COLOR_GOLD);
    tft.setTextColor(fill_txt, fill_bg);
    tft.drawCenterString(fill_act ? "[*] PINTAR" : "  PINTAR", 62, 264, &fonts::Font0);

    bool cross_act = (picross_game.tool_mode == PICROSS_TOOL_CROSS);
    uint16_t cross_bg = cross_act ? COLOR_CROSS : 0x2126;
    uint16_t cross_txt = cross_act ? COLOR_TEXT : COLOR_TEXT_MUTED;
    tft.fillRoundRect(124, 256, 108, 28, 4, cross_bg);
    if (cross_act) tft.drawRoundRect(124, 256, 108, 28, 4, COLOR_TEXT);
    tft.setTextColor(cross_txt, cross_bg);
    tft.drawCenterString(cross_act ? "[*] CRUZAR" : "  CRUZAR", 178, 264, &fonts::Font0);

    tft.fillRoundRect(8, 290, 108, 26, 4, 0x2945);
    tft.setTextColor(COLOR_TEXT, 0x2945);
    tft.drawCenterString("REINICIAR", 62, 298, &fonts::Font0);

    char hint_str[24];
    snprintf(hint_str, sizeof(hint_str), "PISTA (%u)", picross_game.hints_remaining);
    tft.fillRoundRect(124, 290, 108, 26, 4, picross_game.hints_remaining > 0 ? 0x6280 : 0x39E7);
    tft.setTextColor(picross_game.hints_remaining > 0 ? COLOR_GOLD : COLOR_TEXT_MUTED, picross_game.hints_remaining > 0 ? 0x6280 : 0x39E7);
    tft.drawCenterString(hint_str, 178, 298, &fonts::Font0);
}

static void draw_menu_screen() {
    tft.fillScreen(COLOR_BG);

    tft.fillRect(0, 0, SCREEN_WIDTH, 38, COLOR_HEADER);

    // Botón HOME para volver al lanzador 3-en-1
    tft.fillRoundRect(6, 6, 46, 26, 4, 0x8A22);
    tft.setTextColor(COLOR_GOLD, 0x8A22);
    tft.drawCenterString("HOME", 29, 14, &fonts::Font0);

    tft.setTextColor(COLOR_GOLD, COLOR_HEADER);
    tft.drawCenterString("ESP-PICROSS", 140, 8, &fonts::Font0);
    tft.setTextColor(COLOR_TEXT_MUTED, COLOR_HEADER);
    tft.drawCenterString("ELIGE UN PUZZLE", 140, 22, &fonts::Font0);

    for (uint8_t i = 0; i < TOTAL_PUZZLES; i++) {
        int16_t col = i % 3;
        int16_t row = i / 3;
        int16_t bx = 12 + col * 74;
        int16_t by = 46 + row * 52;
        int16_t bw = 70;
        int16_t bh = 46;

        bool done = picross_storage_is_completed(i);
        uint32_t best = picross_storage_get_best_time(i);
        const Puzzle& p = PUZZLES[i];

        uint16_t card_bg = done ? 0x1A68 : 0x2126;
        tft.fillRoundRect(bx, by, bw, bh, 4, card_bg);

        if (i == picross_game.current_level_idx) {
            tft.drawRoundRect(bx, by, bw, bh, 4, COLOR_GOLD);
        } else {
            tft.drawRoundRect(bx, by, bw, bh, 4, done ? COLOR_SUCCESS : 0x39E7);
        }

        char num_str[16];
        snprintf(num_str, sizeof(num_str), "#%02u %ux%u", i + 1, p.size, p.size);
        tft.setTextColor(done ? COLOR_GOLD : COLOR_TEXT, card_bg);
        tft.drawCenterString(num_str, bx + (bw / 2), by + 5, &fonts::Font0);

        tft.setTextColor(COLOR_TEXT_MUTED, card_bg);
        char short_name[10];
        strncpy(short_name, p.name, 9);
        short_name[9] = '\0';
        tft.drawCenterString(short_name, bx + (bw / 2), by + 18, &fonts::Font0);

        if (done) {
            char rec_str[16];
            format_time(best, rec_str, sizeof(rec_str));
            tft.setTextColor(COLOR_SUCCESS, card_bg);
            tft.drawCenterString(rec_str, bx + (bw / 2), by + 31, &fonts::Font0);
        } else {
            tft.setTextColor(0x52AA, card_bg);
            tft.drawCenterString("---", bx + (bw / 2), by + 31, &fonts::Font0);
        }
    }
}

static void draw_victory_screen() {
    tft.fillScreen(COLOR_BG);

    tft.fillRect(0, 0, SCREEN_WIDTH, 42, 0x1B26);
    tft.setTextColor(COLOR_GOLD, 0x1B26);
    tft.drawCenterString("★ ¡PUZZLE RESUELTO! ★", 120, 10, &fonts::Font0);
    tft.setTextColor(COLOR_TEXT, 0x1B26);
    tft.drawCenterString("REVELACION PIXEL ART", 120, 24, &fonts::Font0);

    const Puzzle& p = picross_game.get_current_puzzle();
    uint8_t s = p.size;

    int16_t px_size = (s == 5) ? 22 : 13;
    int16_t art_w = s * px_size;
    int16_t art_h = s * px_size;
    int16_t art_x = (SCREEN_WIDTH - art_w) / 2;
    int16_t art_y = 52;

    tft.drawRoundRect(art_x - 4, art_y - 4, art_w + 8, art_h + 8, 4, COLOR_GOLD);
    tft.fillRect(art_x - 2, art_y - 2, art_w + 4, art_h + 4, 0x0000);

    for (uint8_t r = 0; r < s; r++) {
        for (uint8_t c = 0; c < s; c++) {
            uint16_t col = p.color_reveal[r][c];
            tft.fillRect(art_x + c * px_size, art_y + r * px_size, px_size, px_size, col);
        }
    }

    int16_t text_y = art_y + art_h + 12;
    char disc_str[36];
    snprintf(disc_str, sizeof(disc_str), "DESCUBIERTO: %s", p.name);
    tft.setTextColor(COLOR_GOLD, COLOR_BG);
    tft.drawCenterString(disc_str, 120, text_y, &fonts::Font0);

    char time_buf[16], rec_buf[16];
    format_time(picross_game.elapsed_seconds, time_buf, sizeof(time_buf));
    format_time(picross_storage_get_best_time(picross_game.current_level_idx), rec_buf, sizeof(rec_buf));

    char stats_str[48];
    snprintf(stats_str, sizeof(stats_str), "Tiempo: %s  |  Record: %s", time_buf, rec_buf);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawCenterString(stats_str, 120, text_y + 16, &fonts::Font0);

    tft.fillRoundRect(16, 276, 100, 34, 5, COLOR_SUCCESS);
    tft.setTextColor(COLOR_TEXT, COLOR_SUCCESS);
    tft.drawCenterString("SIGUIENTE >", 66, 288, &fonts::Font0);

    tft.fillRoundRect(124, 276, 100, 34, 5, 0x2126);
    tft.setTextColor(COLOR_GOLD, 0x2126);
    tft.drawCenterString("MENU", 174, 288, &fonts::Font0);
}

void picross_renderer_draw_screen(bool force_full_redraw) {
    if (force_full_redraw || picross_game.state != last_rendered_state || picross_game.current_level_idx != last_rendered_level) {
        last_rendered_state = picross_game.state;
        last_rendered_level = picross_game.current_level_idx;
        last_rendered_tool = picross_game.tool_mode;
        last_rendered_sec = picross_game.elapsed_seconds;
        last_hints_count = picross_game.hints_remaining;

        if (picross_game.state == PICROSS_STATE_MENU) {
            draw_menu_screen();
        } else if (picross_game.state == PICROSS_STATE_PLAYING) {
            tft.fillScreen(COLOR_BG);
            draw_header();
            draw_clues();
            draw_grid();
            draw_bottom_controls();
        } else if (picross_game.state == PICROSS_STATE_VICTORY) {
            draw_victory_screen();
        }
        return;
    }

    if (picross_game.state == PICROSS_STATE_PLAYING) {
        if (picross_game.tool_mode != last_rendered_tool || picross_game.hints_remaining != last_hints_count) {
            last_rendered_tool = picross_game.tool_mode;
            last_hints_count = picross_game.hints_remaining;
            draw_bottom_controls();
        }
        draw_clues();
        picross_renderer_draw_timer_only();
    }
}
