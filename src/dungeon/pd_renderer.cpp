#include "dungeon/pd_renderer.h"
#include "hal/display.h"
#include "dungeon/pd_game_state.h"
#include "dungeon/pd_storage.h"

static PD_GameState last_rendered_state = (PD_GameState)-1;
static bool state_rendered = false;

static uint8_t last_dice_count = 255;
static int8_t last_selected_die = -2;
static bool last_skill_used = false;
static int8_t last_hero_hp = -1;

void pd_renderer_reset() {
    state_rendered = false;
    last_rendered_state = (PD_GameState)-1;
    last_dice_count = 255;
    last_selected_die = -2;
    last_skill_used = false;
    last_hero_hp = -1;
}

void pd_renderer_init() {
    tft.setTextWrap(false);
    pd_renderer_reset();
}

static uint16_t get_die_color(PD_DieType type) {
    switch (type) {
        case PD_DICE_YELLOW: return COLOR_DICE_YELLOW;
        case PD_DICE_GREEN:  return COLOR_DICE_GREEN;
        case PD_DICE_BLUE:   return COLOR_DICE_BLUE;
        default:             return COLOR_DICE_HEROIC;
    }
}

// ----------------------------------------------------------------------------
// CABECERA SUPERIOR (HUD)
// ----------------------------------------------------------------------------
static void draw_hud() {
    tft.fillRect(0, 0, SCREEN_WIDTH, 24, 0x18C3);
    tft.drawFastHLine(0, 24, SCREEN_WIDTH, COLOR_CARD_BORDER);

    // Piso
    char f_str[16];
    snprintf(f_str, sizeof(f_str), "P%u/3", pd_game.floor);
    tft.setTextColor(COLOR_GOLD_TXT, 0x18C3);
    tft.drawString(f_str, 6, 6, &fonts::Font0);

    // Vidas / Corazones
    char hp_str[16];
    snprintf(hp_str, sizeof(hp_str), "HP %d/%d", pd_game.hero.hp, pd_game.hero.max_hp);
    tft.setTextColor(COLOR_HP_RED, 0x18C3);
    tft.drawString(hp_str, 50, 6, &fonts::Font0);

    // Pociones
    char pot_str[16];
    snprintf(pot_str, sizeof(pot_str), "POC %u", pd_game.hero.potions);
    tft.setTextColor(COLOR_POTION_CYAN, 0x18C3);
    tft.drawString(pot_str, 115, 6, &fonts::Font0);

    // Nivel
    char lv_str[16];
    snprintf(lv_str, sizeof(lv_str), "NV%u", pd_game.hero.level);
    tft.setTextColor(COLOR_WHITE, 0x18C3);
    tft.drawString(lv_str, 160, 6, &fonts::Font0);

    // Botón HOME lanzador en la esquina superior derecha
    tft.fillRoundRect(200, 2, 36, 20, 3, 0x2126);
    tft.setTextColor(COLOR_GOLD_TXT, 0x2126);
    tft.drawCenterString("SALIR", 218, 6, &fonts::Font0);
}

// ----------------------------------------------------------------------------
// PANTALLA DE TÍTULO
// ----------------------------------------------------------------------------
static void render_title_screen() {
    tft.fillScreen(COLOR_DUNGEON_BG);

    tft.drawRect(4, 4, 232, 312, COLOR_CARD_BORDER);
    tft.drawRect(6, 6, 228, 308, 0x3186);

    tft.setTextColor(COLOR_GOLD_TXT, COLOR_DUNGEON_BG);
    tft.drawCenterString("POCKET DUNGEON", 120, 30, &fonts::Font2);

    tft.setTextColor(COLOR_WHITE, COLOR_DUNGEON_BG);
    tft.drawCenterString("ROGUE DICE CRAWLER", 120, 54, &fonts::Font0);

    // Icono decorativo dados
    tft.fillRoundRect(85, 80, 70, 70, 10, 0x2145);
    tft.drawRoundRect(85, 80, 70, 70, 10, COLOR_GOLD_TXT);
    tft.fillCircle(105, 100, 6, COLOR_DICE_YELLOW);
    tft.fillCircle(135, 100, 6, COLOR_DICE_GREEN);
    tft.fillCircle(120, 115, 6, COLOR_DICE_HEROIC);
    tft.fillCircle(105, 130, 6, COLOR_DICE_BLUE);
    tft.fillCircle(135, 130, 6, COLOR_DICE_YELLOW);

    char hi_buf[32];
    snprintf(hi_buf, sizeof(hi_buf), "Record: %u pts", pd_storage_get_high_score());
    tft.setTextColor(COLOR_GOLD_TXT, COLOR_DUNGEON_BG);
    tft.drawCenterString(hi_buf, 120, 165, &fonts::Font0);

    char vic_buf[32];
    snprintf(vic_buf, sizeof(vic_buf), "Mazmorras Superadas: %u", pd_storage_get_victories());
    tft.setTextColor(COLOR_POTION_CYAN, COLOR_DUNGEON_BG);
    tft.drawCenterString(vic_buf, 120, 182, &fonts::Font0);

    // Botón Iniciar
    tft.fillRoundRect(25, 215, 190, 42, 6, 0x2EA5);
    tft.drawRoundRect(25, 215, 190, 42, 6, COLOR_WHITE);
    tft.setTextColor(COLOR_WHITE, 0x2EA5);
    tft.drawCenterString("ENTRAR A LA MAZMORRA", 120, 228, &fonts::Font0);

    // Botón Salir al Lanzador
    tft.fillRoundRect(40, 275, 160, 30, 4, 0x8A22);
    tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
    tft.drawCenterString("SALIR AL LANZADOR", 120, 285, &fonts::Font0);
}

// ----------------------------------------------------------------------------
// SELECCIÓN DE HÉROE
// ----------------------------------------------------------------------------
static void render_hero_select() {
    tft.fillScreen(COLOR_DUNGEON_BG);

    tft.fillRect(0, 0, SCREEN_WIDTH, 36, 0x18C3);
    tft.drawFastHLine(0, 36, SCREEN_WIDTH, COLOR_CARD_BORDER);
    tft.setTextColor(COLOR_GOLD_TXT, 0x18C3);
    tft.drawCenterString("ELIGE TU HEROE", 120, 10, &fonts::Font2);

    // Tarjeta 1: GUERRERO
    tft.fillRoundRect(10, 50, 220, 75, 6, 0x2145);
    tft.drawRoundRect(10, 50, 220, 75, 6, COLOR_DICE_YELLOW);
    tft.setTextColor(COLOR_DICE_YELLOW, 0x2145);
    tft.drawString("1. GUERRERO (12 HP)", 20, 58, &fonts::Font0);
    tft.setTextColor(COLOR_WHITE, 0x2145);
    tft.drawString("Dados: 3 Fuerza, 1 Agil, 1 Magia", 20, 74, &fonts::Font0);
    tft.setTextColor(COLOR_GOLD_TXT, 0x2145);
    tft.drawString("Poder: Golpe Escudo (+2 dado amarillo)", 20, 90, &fonts::Font0);
    tft.fillRoundRect(20, 104, 70, 14, 2, 0xFDE0);
    tft.setTextColor(0x0000, 0xFDE0);
    tft.drawCenterString("ELEGIR", 55, 106, &fonts::Font0);

    // Tarjeta 2: PICARA
    tft.fillRoundRect(10, 135, 220, 75, 6, 0x2145);
    tft.drawRoundRect(10, 135, 220, 75, 6, COLOR_DICE_GREEN);
    tft.setTextColor(COLOR_DICE_GREEN, 0x2145);
    tft.drawString("2. PICARA (10 HP)", 20, 143, &fonts::Font0);
    tft.setTextColor(COLOR_WHITE, 0x2145);
    tft.drawString("Dados: 1 Fuerza, 3 Agil, 1 Magia", 20, 159, &fonts::Font0);
    tft.setTextColor(COLOR_GOLD_TXT, 0x2145);
    tft.drawString("Poder: Sigilo (Convierte agil en comodin)", 20, 175, &fonts::Font0);
    tft.fillRoundRect(20, 189, 70, 14, 2, 0x2EA5);
    tft.setTextColor(COLOR_WHITE, 0x2EA5);
    tft.drawCenterString("ELEGIR", 55, 191, &fonts::Font0);

    // Tarjeta 3: MAGA
    tft.fillRoundRect(10, 220, 220, 75, 6, 0x2145);
    tft.drawRoundRect(10, 220, 220, 75, 6, COLOR_DICE_BLUE);
    tft.setTextColor(COLOR_DICE_BLUE, 0x2145);
    tft.drawString("3. MAGA (8 HP)", 20, 228, &fonts::Font0);
    tft.setTextColor(COLOR_WHITE, 0x2145);
    tft.drawString("Dados: 1 Fuerza, 1 Agil, 3 Magia", 20, 244, &fonts::Font0);
    tft.setTextColor(COLOR_GOLD_TXT, 0x2145);
    tft.drawString("Poder: Transmutar (Crea comodin de 6)", 20, 260, &fonts::Font0);
    tft.fillRoundRect(20, 274, 70, 14, 2, 0x2CD9);
    tft.setTextColor(COLOR_WHITE, 0x2CD9);
    tft.drawCenterString("ELEGIR", 55, 276, &fonts::Font0);
}

// ----------------------------------------------------------------------------
// SELECCIÓN DE PUERTA / EXPLORACIÓN
// ----------------------------------------------------------------------------
static void render_door_select() {
    tft.fillScreen(COLOR_DUNGEON_BG);
    draw_hud();

    tft.setTextColor(COLOR_GOLD_TXT, COLOR_DUNGEON_BG);
    tft.drawCenterString("PASILLO DE LA MAZMORRA", 120, 36, &fonts::Font0);
    tft.setTextColor(COLOR_WHITE, COLOR_DUNGEON_BG);
    tft.drawCenterString("Elige una puerta para avanzar", 120, 52, &fonts::Font0);

    // Puerta Izquierda
    tft.fillRoundRect(15, 75, 95, 145, 8, 0x39E7);
    tft.drawRoundRect(15, 75, 95, 145, 8, COLOR_CARD_BORDER);
    tft.drawRoundRect(22, 85, 81, 125, 4, 0x2124);
    tft.fillCircle(30, 145, 4, COLOR_GOLD_TXT);
    tft.setTextColor(COLOR_WHITE, 0x39E7);
    tft.drawCenterString("PUERTA", 62, 110, &fonts::Font0);
    tft.drawCenterString("IZQUIERDA", 62, 126, &fonts::Font0);
    tft.setTextColor(COLOR_GOLD_TXT, 0x39E7);
    tft.drawCenterString("[ ENTRAR ]", 62, 175, &fonts::Font0);

    // Puerta Derecha
    tft.fillRoundRect(130, 75, 95, 145, 8, 0x39E7);
    tft.drawRoundRect(130, 75, 95, 145, 8, COLOR_CARD_BORDER);
    tft.drawRoundRect(137, 85, 81, 125, 4, 0x2124);
    tft.fillCircle(145, 145, 4, COLOR_GOLD_TXT);
    tft.setTextColor(COLOR_WHITE, 0x39E7);
    tft.drawCenterString("PUERTA", 177, 110, &fonts::Font0);
    tft.drawCenterString("DERECHA", 177, 126, &fonts::Font0);
    tft.setTextColor(COLOR_GOLD_TXT, 0x39E7);
    tft.drawCenterString("[ ENTRAR ]", 177, 175, &fonts::Font0);

    // Botón de Poción si está herido
    bool can_heal = (pd_game.hero.potions > 0 && pd_game.hero.hp < pd_game.hero.max_hp);
    uint16_t pot_bg = can_heal ? COLOR_POTION_CYAN : 0x2126;
    tft.fillRoundRect(30, 245, 180, 36, 6, pot_bg);
    tft.setTextColor(can_heal ? COLOR_BLACK : 0x8410, pot_bg);
    tft.drawCenterString(can_heal ? "TOMAR POCION (+4 HP)" : "SIN POCIONES DISPONIBLES", 120, 257, &fonts::Font0);
}

// ----------------------------------------------------------------------------
// PANTALLA DE COMBATE Y ENCUENTRO
// ----------------------------------------------------------------------------
static void render_combat_screen() {
    tft.fillScreen(COLOR_DUNGEON_BG);
    draw_hud();

    // 1. Carta del Encuentro
    tft.fillRoundRect(10, 30, 220, 145, 6, COLOR_CARD_BG);
    tft.drawRoundRect(10, 30, 220, 145, 6, pd_game.current_encounter.is_boss ? 0xF800 : COLOR_CARD_BORDER);

    tft.setTextColor(pd_game.current_encounter.is_boss ? 0xF800 : COLOR_GOLD_TXT, COLOR_CARD_BG);
    tft.drawCenterString(pd_game.current_encounter.name, 120, 40, &fonts::Font2);

    char sub_str[32];
    snprintf(sub_str, sizeof(sub_str), pd_game.current_encounter.is_boss ? "JEFE FINAL DEL PISO 3" : "ENCUENTRO PELIGROSO");
    tft.setTextColor(COLOR_WHITE, COLOR_CARD_BG);
    tft.drawCenterString(sub_str, 120, 64, &fonts::Font0);

    tft.drawFastHLine(20, 80, 200, 0x3186);

    uint8_t count = pd_game.current_encounter.box_count;
    int16_t box_w = 38;
    int16_t box_h = 50;
    int16_t start_x = 120 - (count * (box_w + 5)) / 2;

    for (uint8_t b = 0; b < count; b++) {
        int16_t bx = start_x + b * (box_w + 5);
        int16_t by = 95;
        PD_ChallengeBox& box = pd_game.current_encounter.boxes[b];
        uint16_t b_col = get_die_color(box.req_type);

        if (box.filled) {
            tft.fillRoundRect(bx, by, box_w, box_h, 4, 0x1B84);
            tft.drawRoundRect(bx, by, box_w, box_h, 4, COLOR_WHITE);

            char val_str[8];
            snprintf(val_str, sizeof(val_str), "%u", pd_game.dice[box.placed_die_idx].value);
            tft.setTextColor(COLOR_WHITE, 0x1B84);
            tft.drawCenterString(val_str, bx + box_w / 2, by + 12, &fonts::Font2);

            tft.setTextColor(COLOR_GOLD_TXT, 0x1B84);
            tft.drawCenterString("LISTO", bx + box_w / 2, by + 34, &fonts::Font0);
        } else {
            tft.fillRoundRect(bx, by, box_w, box_h, 4, 0x18C3);
            tft.drawRoundRect(bx, by, box_w, box_h, 4, b_col);

            char req_str[16];
            snprintf(req_str, sizeof(req_str), "%u+", box.min_value);
            tft.setTextColor(b_col, 0x18C3);
            tft.drawCenterString(req_str, bx + box_w / 2, by + 10, &fonts::Font2);

            char dmg_str[8];
            snprintf(dmg_str, sizeof(dmg_str), "-%dHP", box.penalty_damage);
            tft.setTextColor(COLOR_HP_RED, 0x18C3);
            tft.drawCenterString(dmg_str, bx + box_w / 2, by + 34, &fonts::Font0);
        }
    }

    // 2. Bandeja de Dados
    tft.drawRect(8, 180, 224, 75, 0x2126);
    tft.setTextColor(0x8410, COLOR_DUNGEON_BG);
    tft.drawString("BANDEJA DE DADOS:", 14, 184, &fonts::Font0);

    if (pd_game.dice_rolled) {
        int16_t die_w = 32;
        int16_t start_dx = 12;

        for (uint8_t d = 0; d < pd_game.dice_count; d++) {
            int16_t dx = start_dx + (d % 6) * 37;
            int16_t dy = 198 + (d / 6) * 32;
            PD_Die& die = pd_game.dice[d];
            uint16_t d_col = get_die_color(die.type);

            if (die.placed) {
                tft.drawRoundRect(dx, dy, die_w, die_w, 4, 0x3186);
                tft.setTextColor(0x4208, COLOR_DUNGEON_BG);
                tft.drawCenterString("-", dx + die_w / 2, dy + 8, &fonts::Font2);
            } else {
                bool is_sel = (pd_game.selected_die_idx == d);
                tft.fillRoundRect(dx, dy, die_w, die_w, 4, d_col);
                tft.drawRoundRect(dx, dy, die_w, die_w, 4, is_sel ? COLOR_WHITE : 0x0000);
                if (is_sel) tft.drawRoundRect(dx - 1, dy - 1, die_w + 2, die_w + 2, 5, COLOR_GOLD_TXT);

                char d_val[8];
                snprintf(d_val, sizeof(d_val), "%u", die.value);
                tft.setTextColor(die.type == PD_DICE_YELLOW ? 0x0000 : COLOR_WHITE, d_col);
                tft.drawCenterString(d_val, dx + die_w / 2, dy + 8, &fonts::Font2);
            }
        }
    } else {
        tft.setTextColor(COLOR_WHITE, COLOR_DUNGEON_BG);
        tft.drawCenterString("Presiona 'TIRAR DADOS' abajo", 120, 218, &fonts::Font0);
    }

    // 3. Barra de Botones Inferior
    if (!pd_game.dice_rolled) {
        tft.fillRoundRect(20, 264, 200, 44, 6, 0x2EA5);
        tft.drawRoundRect(20, 264, 200, 44, 6, COLOR_WHITE);
        tft.setTextColor(COLOR_WHITE, 0x2EA5);
        tft.drawCenterString("TIRAR DADOS (INICIAR)", 120, 278, &fonts::Font0);
    } else {
        tft.fillRoundRect(8, 264, 66, 44, 4, 0x1B84);
        tft.setTextColor(COLOR_WHITE, 0x1B84);
        tft.drawCenterString("AUTO", 41, 274, &fonts::Font0);
        tft.drawCenterString("COLOCAR", 41, 288, &fonts::Font0);

        bool sk_avail = !pd_game.hero.skill_used;
        uint16_t sk_bg = sk_avail ? 0x780F : 0x2126;
        tft.fillRoundRect(82, 264, 70, 44, 4, sk_bg);
        tft.setTextColor(sk_avail ? COLOR_WHITE : 0x8410, sk_bg);
        tft.drawCenterString("PODER", 117, 274, &fonts::Font0);
        tft.drawCenterString(sk_avail ? "ACTIVO" : "USADO", 117, 288, &fonts::Font0);

        tft.fillRoundRect(160, 264, 72, 44, 4, 0xF800);
        tft.setTextColor(COLOR_WHITE, 0xF800);
        tft.drawCenterString("RESOLVER", 196, 274, &fonts::Font0);
        tft.drawCenterString("TURNO", 196, 288, &fonts::Font0);
    }
}

// ----------------------------------------------------------------------------
// FASE DE BOTÍN
// ----------------------------------------------------------------------------
static void render_loot_screen() {
    tft.fillScreen(COLOR_DUNGEON_BG);
    draw_hud();

    tft.setTextColor(COLOR_GOLD_TXT, COLOR_DUNGEON_BG);
    tft.drawCenterString("VICTORIA EN EL ENCUENTRO", 120, 36, &fonts::Font2);
    tft.setTextColor(COLOR_WHITE, COLOR_DUNGEON_BG);
    tft.drawCenterString("Elige como reclamar la carta:", 120, 60, &fonts::Font0);

    // Opción 0: OBJETO (+1 DADO)
    tft.fillRoundRect(15, 80, 210, 55, 6, 0x2145);
    tft.drawRoundRect(15, 80, 210, 55, 6, COLOR_DICE_YELLOW);
    tft.setTextColor(COLOR_DICE_YELLOW, 0x2145);
    tft.drawString("1. RECLAMAR COMO OBJETO", 25, 90, &fonts::Font0);
    tft.setTextColor(COLOR_WHITE, 0x2145);
    tft.drawString("+1 Dado permanente a tu reserva", 25, 108, &fonts::Font0);

    // Opción 1: POCIÓN (+3 HP)
    tft.fillRoundRect(15, 145, 210, 55, 6, 0x2145);
    tft.drawRoundRect(15, 145, 210, 55, 6, COLOR_POTION_CYAN);
    tft.setTextColor(COLOR_POTION_CYAN, 0x2145);
    tft.drawString("2. POCION DE SALUD", 25, 155, &fonts::Font0);
    tft.setTextColor(COLOR_WHITE, 0x2145);
    tft.drawString("Recupera +3 Corazones ahora", 25, 173, &fonts::Font0);

    // Opción 2: EXPERIENCIA (+1 XP)
    tft.fillRoundRect(15, 210, 210, 55, 6, 0x2145);
    tft.drawRoundRect(15, 210, 210, 55, 6, COLOR_XP_PURPLE);
    tft.setTextColor(COLOR_XP_PURPLE, 0x2145);
    tft.drawString("3. EXPERIENCIA HEROICA (+1 XP)", 25, 220, &fonts::Font0);
    tft.setTextColor(COLOR_WHITE, 0x2145);
    tft.drawString("Sube de nivel y desbloquea comodines", 25, 238, &fonts::Font0);
}

// ----------------------------------------------------------------------------
// GAME OVER & VICTORIA
// ----------------------------------------------------------------------------
static void render_game_over() {
    tft.fillRect(20, 60, 200, 165, 0x10A2);
    tft.drawRect(20, 60, 200, 165, 0xF800);

    tft.setTextColor(0xF800, 0x10A2);
    tft.drawCenterString("HEROE CAIDO", 120, 75, &fonts::Font2);
    tft.setTextColor(COLOR_WHITE, 0x10A2);
    tft.drawCenterString("La mazmorra ha cobrado tu vida", 120, 100, &fonts::Font0);

    char p_str[32];
    snprintf(p_str, sizeof(p_str), "Puntuacion: %u pts", pd_game.score);
    tft.setTextColor(COLOR_GOLD_TXT, 0x10A2);
    tft.drawCenterString(p_str, 120, 120, &fonts::Font0);

    tft.fillRoundRect(40, 145, 160, 32, 5, 0xF800);
    tft.setTextColor(COLOR_WHITE, 0xF800);
    tft.drawCenterString("REINTENTAR", 120, 155, &fonts::Font0);

    tft.fillRoundRect(40, 182, 160, 30, 5, 0x8A22);
    tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
    tft.drawCenterString("SALIR AL LANZADOR", 120, 192, &fonts::Font0);
}

static void render_victory() {
    tft.fillRect(20, 60, 200, 165, 0x10A2);
    tft.drawRect(20, 60, 200, 165, COLOR_GOLD_TXT);

    tft.setTextColor(COLOR_GOLD_TXT, 0x10A2);
    tft.drawCenterString("VICTORIA SUPREMA", 120, 75, &fonts::Font2);
    tft.setTextColor(COLOR_WHITE, 0x10A2);
    tft.drawCenterString("EL DRAGON HA SIDO DERROTADO!", 120, 100, &fonts::Font0);

    char p_str[32];
    snprintf(p_str, sizeof(p_str), "Puntuacion Final: %u", pd_game.score);
    tft.setTextColor(COLOR_GOLD_TXT, 0x10A2);
    tft.drawCenterString(p_str, 120, 120, &fonts::Font0);

    tft.fillRoundRect(40, 145, 160, 32, 5, 0x2EA5);
    tft.setTextColor(COLOR_WHITE, 0x2EA5);
    tft.drawCenterString("JUGAR DE NUEVO", 120, 155, &fonts::Font0);

    tft.fillRoundRect(40, 182, 160, 30, 5, 0x8A22);
    tft.setTextColor(COLOR_GOLD_TXT, 0x8A22);
    tft.drawCenterString("SALIR AL LANZADOR", 120, 192, &fonts::Font0);
}

void pd_renderer_draw() {
    if (pd_game.state != last_rendered_state) {
        last_rendered_state = pd_game.state;
        state_rendered = false;
    }

    switch (pd_game.state) {
        case PD_STATE_TITLE:
            if (!state_rendered) {
                render_title_screen();
                state_rendered = true;
            }
            break;

        case PD_STATE_HERO_SELECT:
            if (!state_rendered) {
                render_hero_select();
                state_rendered = true;
            }
            break;

        case PD_STATE_DOOR_SELECT:
            if (!state_rendered) {
                render_door_select();
                state_rendered = true;
            }
            break;

        case PD_STATE_COMBAT:
            if (!state_rendered || last_selected_die != pd_game.selected_die_idx ||
                last_dice_count != pd_game.dice_count || last_skill_used != pd_game.hero.skill_used ||
                last_hero_hp != pd_game.hero.hp) {
                render_combat_screen();
                state_rendered = true;
                last_selected_die = pd_game.selected_die_idx;
                last_dice_count = pd_game.dice_count;
                last_skill_used = pd_game.hero.skill_used;
                last_hero_hp = pd_game.hero.hp;
            }
            break;

        case PD_STATE_LOOT:
            if (!state_rendered) {
                render_loot_screen();
                state_rendered = true;
            }
            break;

        case PD_STATE_GAME_OVER:
            if (!state_rendered) {
                render_game_over();
                state_rendered = true;
            }
            break;

        case PD_STATE_VICTORY:
            if (!state_rendered) {
                render_victory();
                state_rendered = true;
            }
            break;
    }
}
