#include "towerdef/td_renderer.h"
#include "towerdef/td_game_state.h"
#include "hal/display.h"
#include "towerdef/td_storage.h"
#include <math.h>

static LGFX_Sprite canvas(&tft);
static bool canvas_ready = false;

void td_renderer_init() {
    tft.setTextWrap(false);
    canvas_ready = canvas.createSprite(SCREEN_WIDTH, BATTLEFIELD_HEIGHT);
    if (canvas_ready) {
        canvas.setTextWrap(false);
    }
}

static void render_title_screen() {
    tft.fillScreen(COLOR_HUD_BG);

    tft.fillRect(0, 0, SCREEN_WIDTH, 48, 0x18C3);
    tft.drawFastHLine(0, 48, SCREEN_WIDTH, COLOR_GOLD_TXT);

    tft.setTextColor(COLOR_GOLD_TXT, 0x18C3);
    tft.drawCenterString("MICRO-KINGDOM", 120, 10, &fonts::Font2);
    tft.setTextColor(COLOR_WHITE, 0x18C3);
    tft.drawCenterString("TOWER DEFENSE TACTIL", 120, 30, &fonts::Font0);

    int16_t cx = 120, cy = 110;
    tft.fillRect(cx - 35, cy - 20, 70, 45, COLOR_CASTLE);
    tft.drawRect(cx - 35, cy - 20, 70, 45, 0x39E7);
    for (int i = 0; i < 4; i++) {
        tft.fillRect(cx - 35 + (i * 18), cy - 28, 12, 9, COLOR_CASTLE);
        tft.drawRect(cx - 35 + (i * 18), cy - 28, 12, 9, 0x39E7);
    }
    tft.fillRoundRect(cx - 12, cy + 5, 24, 20, 6, 0x4A24);
    tft.drawFastVLine(cx - 30, cy - 42, 16, COLOR_WHITE);
    tft.fillTriangle(cx - 30, cy - 42, cx - 14, cy - 35, cx - 30, cy - 28, 0xF800);

    uint32_t hi_score = td_storage_get_high_score();
    uint8_t max_wave = td_storage_get_max_wave();
    char stats_buf[48];
    snprintf(stats_buf, sizeof(stats_buf), "RECORD: %u pts | OLEADA MAX: %u", hi_score, max_wave);
    tft.setTextColor(COLOR_GOLD_TXT, COLOR_HUD_BG);
    tft.drawCenterString(stats_buf, 120, 175, &fonts::Font0);

    // Botón JUGAR
    tft.fillRoundRect(30, 205, 180, 38, 6, 0x2EA5);
    tft.drawRoundRect(30, 205, 180, 38, 6, COLOR_WHITE);
    tft.setTextColor(COLOR_WHITE, 0x2EA5);
    tft.drawCenterString("TOCA PARA JUGAR", 120, 218, &fonts::Font0);

    // Botón VOLVER AL LANZADOR
    tft.fillRoundRect(40, 260, 160, 32, 5, 0x8A22);
    tft.drawRoundRect(40, 260, 160, 32, 5, COLOR_GOLD_TXT);
    tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
    tft.drawCenterString("< SALIR AL LANZADOR", 120, 270, &fonts::Font0);
}

static void render_hud() {
    tft.fillRect(0, 0, SCREEN_WIDTH, BATTLEFIELD_Y_START, COLOR_HUD_BG);
    tft.drawFastHLine(0, BATTLEFIELD_Y_START - 1, SCREEN_WIDTH, 0x39E7);

    char lives_str[16];
    snprintf(lives_str, sizeof(lives_str), "HP:%d", td_game.lives);
    tft.setTextColor(COLOR_HEART_TXT, COLOR_HUD_BG);
    tft.drawString(lives_str, 4, 6, &fonts::Font0);

    char gold_str[16];
    snprintf(gold_str, sizeof(gold_str), "ORO:%u", td_game.gold);
    tft.setTextColor(COLOR_GOLD_TXT, COLOR_HUD_BG);
    tft.drawString(gold_str, 56, 6, &fonts::Font0);

    char wave_str[16];
    snprintf(wave_str, sizeof(wave_str), "W:%u/%u", td_game.wave, TOTAL_WAVES);
    tft.setTextColor(COLOR_WHITE, COLOR_HUD_BG);
    tft.drawString(wave_str, 120, 6, &fonts::Font0);

    tft.fillRoundRect(170, 3, 32, 18, 3, (td_game.game_speed == 2) ? 0xFD20 : 0x2126);
    tft.setTextColor(COLOR_WHITE, (td_game.game_speed == 2) ? 0xFD20 : 0x2126);
    tft.drawCenterString((td_game.game_speed == 2) ? "2X" : "1X", 186, 7, &fonts::Font0);

    // Botón HOME en cabecera
    tft.fillRoundRect(208, 3, 28, 18, 3, 0x8A22);
    tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
    tft.drawCenterString("H", 222, 7, &fonts::Font0);
}

static void render_battlefield() {
    LovyanGFX& gfx = canvas_ready ? (LovyanGFX&)canvas : (LovyanGFX&)tft;
    int16_t y_offset = canvas_ready ? 0 : BATTLEFIELD_Y_START;

    gfx.fillRect(0, y_offset, SCREEN_WIDTH, BATTLEFIELD_HEIGHT, COLOR_GRASS);

    for (int i = 0; i < 20; i++) {
        int16_t gx = (i * 37) % SCREEN_WIDTH;
        int16_t gy = y_offset + ((i * 43) % BATTLEFIELD_HEIGHT);
        gfx.drawFastVLine(gx, gy, 3, COLOR_GRASS_DARK);
        gfx.drawFastVLine(gx + 1, gy - 1, 4, COLOR_GRASS_DARK);
    }

    for (int i = 0; i < NUM_WAYPOINTS - 1; i++) {
        Point p1 = MAP_WAYPOINTS[i];
        Point p2 = MAP_WAYPOINTS[i + 1];

        int16_t x1 = (int16_t)p1.x;
        int16_t y1 = (int16_t)p1.y - BATTLEFIELD_Y_START + y_offset;
        int16_t x2 = (int16_t)p2.x;
        int16_t y2 = (int16_t)p2.y - BATTLEFIELD_Y_START + y_offset;

        for (int w = -9; w <= 9; w++) {
            if (x1 == x2) {
                gfx.drawLine(x1 + w, y1, x2 + w, y2, (abs(w) >= 8) ? COLOR_PATH_BORDER : COLOR_PATH);
            } else {
                gfx.drawLine(x1, y1 + w, x2, y2 + w, (abs(w) >= 8) ? COLOR_PATH_BORDER : COLOR_PATH);
            }
        }
        gfx.fillCircle(x1, y1, 8, COLOR_PATH);
        gfx.fillCircle(x2, y2, 8, COLOR_PATH);
    }

    Point castle_pt = MAP_WAYPOINTS[NUM_WAYPOINTS - 1];
    int16_t cx = (int16_t)castle_pt.x;
    int16_t cy = (int16_t)castle_pt.y - BATTLEFIELD_Y_START + y_offset;

    gfx.fillRect(cx - 16, cy - 20, 34, 30, COLOR_CASTLE);
    gfx.drawRect(cx - 16, cy - 20, 34, 30, 0x39E7);
    for (int m = 0; m < 3; m++) {
        gfx.fillRect(cx - 16 + (m * 12), cy - 25, 8, 6, COLOR_CASTLE);
    }
    gfx.fillRoundRect(cx - 6, cy - 6, 14, 16, 4, 0x3183);
    gfx.drawFastVLine(cx + 12, cy - 35, 12, COLOR_WHITE);
    gfx.fillTriangle(cx + 12, cy - 35, cx + 22, cy - 30, cx + 12, cy - 25, 0x05BF);

    for (uint8_t i = 0; i < NUM_PLOTS; i++) {
        int16_t px = td_game.plots[i].x;
        int16_t py = td_game.plots[i].y - BATTLEFIELD_Y_START + y_offset;

        if (td_game.selected_plot_idx == i && td_game.plots[i].type != TOWER_NONE) {
            float r = tower_get_range(td_game.plots[i].type, td_game.plots[i].level);
            gfx.drawCircle(px, py, (int16_t)r, COLOR_WHITE);
        }

        gfx.fillCircle(px, py, 11, 0x52AA);
        gfx.fillCircle(px, py, 9, 0x738E);

        if (td_game.plots[i].type == TOWER_NONE) {
            gfx.drawCircle(px, py, 8, 0x9CD3);
            gfx.drawFastHLine(px - 3, py, 7, COLOR_WHITE);
            gfx.drawFastVLine(px, py - 3, 7, COLOR_WHITE);
        } else if (td_game.plots[i].type == TOWER_ARROW) {
            gfx.fillCircle(px, py, 8, 0x8A22);
            gfx.fillRect(px - 4, py - 6, 8, 8, 0x2EA5);
            gfx.fillTriangle(px - 6, py - 6, px, py - 11, px + 6, py - 6, 0xFD20);
        } else if (td_game.plots[i].type == TOWER_CANNON) {
            gfx.fillCircle(px, py, 8, 0x39E7);
            gfx.fillCircle(px, py - 2, 5, 0x18C3);
            gfx.fillRect(px - 2, py - 8, 5, 7, 0x0000);
        } else if (td_game.plots[i].type == TOWER_ICE) {
            gfx.fillCircle(px, py, 8, 0x05BF);
            gfx.fillTriangle(px - 5, py + 2, px, py - 10, px + 5, py + 2, 0x07FF);
            gfx.fillCircle(px, py - 10, 2, COLOR_WHITE);
        }

        if (td_game.plots[i].type != TOWER_NONE && td_game.plots[i].level >= 2) {
            gfx.fillCircle(px - 3, py + 6, 1, COLOR_GOLD_TXT);
            gfx.fillCircle(px + 3, py + 6, 1, COLOR_GOLD_TXT);
            if (td_game.plots[i].level >= 3) {
                gfx.fillCircle(px, py + 3, 1, COLOR_GOLD_TXT);
            }
        }
    }

    uint32_t now = millis();
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (!td_game.enemies[i].active) continue;

        int16_t ex = (int16_t)td_game.enemies[i].x;
        int16_t ey = (int16_t)td_game.enemies[i].y - BATTLEFIELD_Y_START + y_offset;

        bool is_slowed = (now < td_game.enemies[i].slow_until_ms);

        if (td_game.enemies[i].type == ENEMY_GOBLIN) {
            gfx.fillCircle(ex, ey, 5, is_slowed ? 0x07FF : 0x2DE0);
            gfx.drawFastHLine(ex - 2, ey - 2, 2, 0xF800);
            gfx.drawFastHLine(ex + 1, ey - 2, 2, 0xF800);
        } else if (td_game.enemies[i].type == ENEMY_ORC) {
            gfx.fillCircle(ex, ey, 7, is_slowed ? 0x05BF : 0xB142);
            gfx.drawCircle(ex, ey, 7, 0x4208);
            gfx.drawFastHLine(ex - 3, ey - 2, 2, COLOR_WHITE);
            gfx.drawFastHLine(ex + 2, ey - 2, 2, COLOR_WHITE);
        } else if (td_game.enemies[i].type == ENEMY_GOLEM) {
            gfx.fillRoundRect(ex - 7, ey - 7, 14, 14, 3, is_slowed ? 0x54DE : 0x6B4D);
            gfx.drawRoundRect(ex - 7, ey - 7, 14, 14, 3, 0x2945);
        } else if (td_game.enemies[i].type == ENEMY_BOSS) {
            gfx.fillCircle(ex, ey, 11, is_slowed ? 0x04DF : 0x9000);
            gfx.drawCircle(ex, ey, 11, COLOR_GOLD_TXT);
            gfx.drawLine(ex - 8, ey - 6, ex - 12, ey - 12, COLOR_GOLD_TXT);
            gfx.drawLine(ex + 8, ey - 6, ex + 12, ey - 12, COLOR_GOLD_TXT);
        }

        int16_t bw = (td_game.enemies[i].type == ENEMY_BOSS) ? 22 : 12;
        int16_t bh = 2;
        int16_t bx = ex - (bw / 2);
        int16_t by = ey - ((td_game.enemies[i].type == ENEMY_BOSS) ? 16 : 10);

        gfx.fillRect(bx, by, bw, bh, 0x8000);
        float hp_ratio = td_game.enemies[i].hp / td_game.enemies[i].max_hp;
        if (hp_ratio > 0.0f) {
            int16_t fill_w = (int16_t)(bw * hp_ratio);
            if (fill_w > bw) fill_w = bw;
            gfx.fillRect(bx, by, fill_w, bh, 0x07E0);
        }
    }

    for (uint8_t i = 0; i < MAX_PROJECTILES; i++) {
        if (!td_game.projectiles[i].active) continue;

        int16_t prx = (int16_t)td_game.projectiles[i].x;
        int16_t pry = (int16_t)td_game.projectiles[i].y - BATTLEFIELD_Y_START + y_offset;

        if (td_game.projectiles[i].type == PROJ_ARROW) {
            gfx.fillCircle(prx, pry, 2, 0xFDE0);
        } else if (td_game.projectiles[i].type == PROJ_CANNONBALL) {
            gfx.fillCircle(prx, pry, 3, 0x18C3);
            gfx.drawCircle(prx, pry, 4, 0xFD20);
        } else if (td_game.projectiles[i].type == PROJ_ICE_BOLT) {
            gfx.fillCircle(prx, pry, 3, 0x07FF);
            gfx.fillCircle(prx, pry, 1, COLOR_WHITE);
        }
    }

    if (td_game.lightning_anim_active) {
        int16_t lx = (int16_t)td_game.lightning_target.x;
        int16_t ly = (int16_t)td_game.lightning_target.y - BATTLEFIELD_Y_START + y_offset;

        int16_t mid_y = ly / 2;
        gfx.drawLine(lx - 10, y_offset, lx + 8, mid_y, COLOR_GOLD_TXT);
        gfx.drawLine(lx + 8, mid_y, lx - 4, mid_y + 15, COLOR_WHITE);
        gfx.drawLine(lx - 4, mid_y + 15, lx, ly, COLOR_GOLD_TXT);

        gfx.drawCircle(lx, ly, 14, 0x07FF);
        gfx.drawCircle(lx, ly, 24, COLOR_GOLD_TXT);
    }

    for (uint8_t i = 0; i < MAX_PARTICLES; i++) {
        if (!td_game.particles[i].active) continue;
        int16_t ptx = (int16_t)td_game.particles[i].x;
        int16_t pty = (int16_t)td_game.particles[i].y - BATTLEFIELD_Y_START + y_offset;
        gfx.fillRect(ptx, pty, 2, 2, td_game.particles[i].color);
    }

    if (canvas_ready) {
        canvas.pushSprite(0, BATTLEFIELD_Y_START);
    }
}

static void render_bottom_tray() {
    tft.fillRect(0, BOTTOM_TRAY_Y_START, SCREEN_WIDTH, BOTTOM_TRAY_HEIGHT, COLOR_TRAY_BG);
    tft.drawFastHLine(0, BOTTOM_TRAY_Y_START, SCREEN_WIDTH, 0x39E7);

    if (td_game.selected_plot_idx >= 0 && td_game.plots[td_game.selected_plot_idx].type != TOWER_NONE) {
        TowerPlot& p = td_game.plots[td_game.selected_plot_idx];

        char title[32];
        snprintf(title, sizeof(title), "TORRE %s (NV %u)", tower_get_name(p.type), p.level);
        tft.setTextColor(COLOR_GOLD_TXT, COLOR_TRAY_BG);
        tft.drawString(title, 8, BOTTOM_TRAY_Y_START + 4, &fonts::Font0);

        if (p.level < 3) {
            uint16_t up_cost = tower_get_upgrade_cost(p.type, p.level);
            char up_str[24];
            snprintf(up_str, sizeof(up_str), "+SUBIR (%ug)", up_cost);
            bool can_up = (td_game.gold >= up_cost);
            tft.fillRoundRect(8, BOTTOM_TRAY_Y_START + 18, 108, 32, 4, can_up ? 0x2EA5 : 0x4208);
            tft.setTextColor(can_up ? COLOR_WHITE : 0x8410, can_up ? 0x2EA5 : 0x4208);
            tft.drawCenterString(up_str, 62, BOTTOM_TRAY_Y_START + 28, &fonts::Font0);
        } else {
            tft.fillRoundRect(8, BOTTOM_TRAY_Y_START + 18, 108, 32, 4, 0x2126);
            tft.setTextColor(COLOR_GOLD_TXT, 0x2126);
            tft.drawCenterString("MAXIMO NV3", 62, BOTTOM_TRAY_Y_START + 28, &fonts::Font0);
        }

        uint16_t sell_val = tower_get_sell_value(p.type, p.level);
        char sell_str[24];
        snprintf(sell_str, sizeof(sell_str), "VENDER +%u", sell_val);
        tft.fillRoundRect(122, BOTTOM_TRAY_Y_START + 18, 68, 32, 4, 0x8A22);
        tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
        tft.drawCenterString(sell_str, 156, BOTTOM_TRAY_Y_START + 28, &fonts::Font0);

        tft.fillRoundRect(196, BOTTOM_TRAY_Y_START + 18, 36, 32, 4, 0x2945);
        tft.setTextColor(COLOR_WHITE, 0x2945);
        tft.drawCenterString("X", 214, BOTTOM_TRAY_Y_START + 28, &fonts::Font0);

    } else {
        bool sel_arrow = (td_game.selected_tool == TOOL_BUILD_ARROW);
        tft.fillRoundRect(4, BOTTOM_TRAY_Y_START + 4, 74, 30, 4, sel_arrow ? 0x2EA5 : 0x2126);
        if (sel_arrow) tft.drawRoundRect(4, BOTTOM_TRAY_Y_START + 4, 74, 30, 4, COLOR_GOLD_TXT);
        tft.setTextColor(sel_arrow ? COLOR_GOLD_TXT : COLOR_WHITE, sel_arrow ? 0x2EA5 : 0x2126);
        tft.drawCenterString("ARQUERO", 41, BOTTOM_TRAY_Y_START + 8, &fonts::Font0);
        tft.drawCenterString("100g", 41, BOTTOM_TRAY_Y_START + 20, &fonts::Font0);

        bool sel_cannon = (td_game.selected_tool == TOOL_BUILD_CANNON);
        tft.fillRoundRect(82, BOTTOM_TRAY_Y_START + 4, 74, 30, 4, sel_cannon ? 0x2EA5 : 0x2126);
        if (sel_cannon) tft.drawRoundRect(82, BOTTOM_TRAY_Y_START + 4, 74, 30, 4, COLOR_GOLD_TXT);
        tft.setTextColor(sel_cannon ? COLOR_GOLD_TXT : COLOR_WHITE, sel_cannon ? 0x2EA5 : 0x2126);
        tft.drawCenterString("CANON", 119, BOTTOM_TRAY_Y_START + 8, &fonts::Font0);
        tft.drawCenterString("150g", 119, BOTTOM_TRAY_Y_START + 20, &fonts::Font0);

        bool sel_ice = (td_game.selected_tool == TOOL_BUILD_ICE);
        tft.fillRoundRect(160, BOTTOM_TRAY_Y_START + 4, 76, 30, 4, sel_ice ? 0x2EA5 : 0x2126);
        if (sel_ice) tft.drawRoundRect(160, BOTTOM_TRAY_Y_START + 4, 76, 30, 4, COLOR_GOLD_TXT);
        tft.setTextColor(sel_ice ? COLOR_GOLD_TXT : COLOR_WHITE, sel_ice ? 0x2EA5 : 0x2126);
        tft.drawCenterString("M. HIELO", 198, BOTTOM_TRAY_Y_START + 8, &fonts::Font0);
        tft.drawCenterString("125g", 198, BOTTOM_TRAY_Y_START + 20, &fonts::Font0);

        bool l_ready = td_game.is_lightning_ready();
        bool sel_light = (td_game.selected_tool == TOOL_SPELL_LIGHTNING);
        uint16_t light_bg = sel_light ? COLOR_GOLD_TXT : (l_ready ? 0x6280 : 0x2945);
        tft.fillRoundRect(4, BOTTOM_TRAY_Y_START + 38, 70, 30, 4, light_bg);
        if (sel_light) tft.drawRoundRect(4, BOTTOM_TRAY_Y_START + 38, 70, 30, 4, COLOR_WHITE);
        tft.setTextColor(sel_light ? 0x0000 : (l_ready ? COLOR_GOLD_TXT : 0x8410), light_bg);

        if (l_ready) {
            tft.drawCenterString("RAYO!", 39, BOTTOM_TRAY_Y_START + 47, &fonts::Font0);
        } else {
            char cd_str[16];
            snprintf(cd_str, sizeof(cd_str), "RAYO (%u)", td_game.get_lightning_cd_remaining_sec());
            tft.drawCenterString(cd_str, 39, BOTTOM_TRAY_Y_START + 47, &fonts::Font0);
        }

        bool can_wave = !td_game.wave_in_progress;
        uint16_t wave_bg = can_wave ? 0x1B84 : 0x2126;
        tft.fillRoundRect(80, BOTTOM_TRAY_Y_START + 38, 156, 30, 4, wave_bg);
        if (can_wave) tft.drawRoundRect(80, BOTTOM_TRAY_Y_START + 38, 156, 30, 4, COLOR_WHITE);
        tft.setTextColor(can_wave ? COLOR_WHITE : COLOR_GOLD_TXT, wave_bg);
        tft.drawCenterString(can_wave ? "> LANZAR OLEADA <" : "OLEADA EN CURSO...", 158, BOTTOM_TRAY_Y_START + 47, &fonts::Font0);
    }
}

static void render_game_over_overlay() {
    tft.fillRect(20, 60, 200, 160, 0x10A2);
    tft.drawRect(20, 60, 200, 160, 0xF800);

    tft.setTextColor(0xF800, 0x10A2);
    tft.drawCenterString("REINO DESTRUIDO", 120, 75, &fonts::Font2);

    char buf[48];
    snprintf(buf, sizeof(buf), "Puntuacion: %u pts", td_game.score);
    tft.setTextColor(COLOR_WHITE, 0x10A2);
    tft.drawCenterString(buf, 120, 105, &fonts::Font0);

    snprintf(buf, sizeof(buf), "Oleada alcanzada: %u", td_game.wave);
    tft.setTextColor(COLOR_GOLD_TXT, 0x10A2);
    tft.drawCenterString(buf, 120, 120, &fonts::Font0);

    tft.fillRoundRect(40, 145, 160, 32, 5, 0xF800);
    tft.setTextColor(COLOR_WHITE, 0xF800);
    tft.drawCenterString("REINTENTAR", 120, 155, &fonts::Font0);

    tft.fillRoundRect(40, 182, 160, 30, 5, 0x8A22);
    tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
    tft.drawCenterString("SALIR AL LANZADOR", 120, 192, &fonts::Font0);
}

static void render_victory_overlay() {
    tft.fillRect(20, 60, 200, 160, 0x10A2);
    tft.drawRect(20, 60, 200, 160, COLOR_GOLD_TXT);

    tft.setTextColor(COLOR_GOLD_TXT, 0x10A2);
    tft.drawCenterString("VICTORIA REAL", 120, 75, &fonts::Font2);
    tft.setTextColor(COLOR_WHITE, 0x10A2);
    tft.drawCenterString("EL REINO HA SIDO SALVADO!", 120, 100, &fonts::Font0);

    char buf[48];
    snprintf(buf, sizeof(buf), "Puntuacion Final: %u", td_game.score);
    tft.setTextColor(COLOR_GOLD_TXT, 0x10A2);
    tft.drawCenterString(buf, 120, 120, &fonts::Font0);

    tft.fillRoundRect(40, 145, 160, 32, 5, 0x2EA5);
    tft.setTextColor(COLOR_WHITE, 0x2EA5);
    tft.drawCenterString("JUGAR DE NUEVO", 120, 155, &fonts::Font0);

    tft.fillRoundRect(40, 182, 160, 30, 5, 0x8A22);
    tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
    tft.drawCenterString("SALIR AL LANZADOR", 120, 192, &fonts::Font0);
}

void td_renderer_draw() {
    if (td_game.state == TD_STATE_TITLE) {
        static bool title_rendered = false;
        if (!title_rendered) {
            render_title_screen();
            title_rendered = true;
        }
        return;
    }

    if (td_game.state == TD_STATE_PLAYING) {
        render_hud();
        render_battlefield();
        render_bottom_tray();
    } else if (td_game.state == TD_STATE_GAME_OVER) {
        render_hud();
        render_battlefield();
        render_game_over_overlay();
    } else if (td_game.state == TD_STATE_VICTORY) {
        render_hud();
        render_battlefield();
        render_victory_overlay();
    }
}
