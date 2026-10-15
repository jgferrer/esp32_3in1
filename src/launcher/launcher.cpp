#include "launcher/launcher.h"
#include "hal/display.h"
#include "hal/buzzer.h"
#include "hal/led.h"
#include <Preferences.h>
#include <rom/rtc.h>

static Preferences prefs;

AppID launcher_get_active_app() {
    esp_reset_reason_t reason = esp_reset_reason();
    // Si la placa se acaba de conectar por USB o encender físicamente, mostrar siempre el menú inicial
    if (reason == ESP_RST_POWERON || reason == ESP_RST_EXT) {
        return APP_LAUNCHER;
    }

    if (prefs.begin("retro3in1", true)) {
        uint8_t val = prefs.getUChar("app", (uint8_t)APP_LAUNCHER);
        prefs.end();
        if (val <= APP_DUNGEON) return (AppID)val;
    }
    return APP_LAUNCHER;
}

void launcher_set_active_app(AppID app) {
    if (prefs.begin("retro3in1", false)) {
        prefs.putUChar("app", (uint8_t)app);
        prefs.end();
    }
    buzzer_click();
    delay(80);
    ESP.restart();
}

void launcher_return_home() {
    launcher_set_active_app(APP_LAUNCHER);
}

void launcher_init() {
    led_safe();
    tft.setTextWrap(false);
}

void launcher_draw() {
    tft.fillScreen(0x10A2);

    // Cabecera Principal
    tft.fillRect(0, 0, SCREEN_WIDTH, 38, 0x18C3);
    tft.drawFastHLine(0, 38, SCREEN_WIDTH, 0xFDE0);

    tft.setTextColor(0xFDE0, 0x18C3);
    tft.drawCenterString("RETRO-STATION 4-IN-1", 120, 6, &fonts::Font2);
    tft.setTextColor(0xFFFF, 0x18C3);
    tft.drawCenterString("TOCA UN JUEGO CON EL LAPIZ", 120, 24, &fonts::Font0);

    // -------------------------------------------------------------
    // TARJETA 1: TAMAGOTCHI 16-BIT (Y = 44 a 104)
    // -------------------------------------------------------------
    tft.fillRoundRect(8, 44, 224, 60, 5, 0x2908);
    tft.drawRoundRect(8, 44, 224, 60, 5, 0xF81F);

    // Icono decorativo mascota / huevo
    tft.fillCircle(28, 74, 14, 0xFDB4);
    tft.fillCircle(24, 71, 2, 0x0000);
    tft.fillCircle(32, 71, 2, 0x0000);
    tft.fillCircle(28, 77, 3, 0xF800);

    tft.setTextColor(0xFDE0, 0x2908);
    tft.drawString("1. TAMAGOTCHI 16-BIT", 52, 50, &fonts::Font0);
    tft.setTextColor(0xFFFF, 0x2908);
    tft.drawString("Mascota virtual con cuidados", 52, 64, &fonts::Font0);

    tft.fillRoundRect(52, 80, 78, 14, 3, 0x780F);
    tft.setTextColor(0xFFFF, 0x780F);
    tft.drawCenterString("[ MASCOTA ]", 91, 82, &fonts::Font0);

    // -------------------------------------------------------------
    // TARJETA 2: ESP-PICROSS RETRO (Y = 110 a 170)
    // -------------------------------------------------------------
    tft.fillRoundRect(8, 110, 224, 60, 5, 0x19E8);
    tft.drawRoundRect(8, 110, 224, 60, 5, 0x2EA5);

    // Icono Hongo / Cuadrícula
    tft.fillCircle(28, 137, 12, 0xF800);
    tft.fillRect(21, 137, 14, 10, 0xFDE0);
    tft.fillCircle(25, 134, 2, 0xFFFF);
    tft.fillCircle(31, 134, 2, 0xFFFF);

    tft.setTextColor(0xFDE0, 0x19E8);
    tft.drawString("2. ESP-PICROSS RETRO", 52, 116, &fonts::Font0);
    tft.setTextColor(0xFFFF, 0x19E8);
    tft.drawString("15 Nonogramas estilo DS", 52, 130, &fonts::Font0);

    tft.fillRoundRect(52, 146, 78, 14, 3, 0x2EA5);
    tft.setTextColor(0xFFFF, 0x2EA5);
    tft.drawCenterString("[ PUZZLE DS ]", 91, 148, &fonts::Font0);

    // -------------------------------------------------------------
    // TARJETA 3: MICRO-KINGDOM TOWER DEFENSE (Y = 176 a 236)
    // -------------------------------------------------------------
    tft.fillRoundRect(8, 176, 224, 60, 5, 0x1B64);
    tft.drawRoundRect(8, 176, 224, 60, 5, 0xFD20);

    // Icono Torre del castillo
    tft.fillRect(20, 200, 16, 20, 0x7BEF);
    tft.fillRect(18, 196, 5, 5, 0x7BEF);
    tft.fillRect(25, 196, 5, 5, 0x7BEF);
    tft.fillRect(32, 196, 5, 5, 0x7BEF);

    tft.setTextColor(0xFDE0, 0x1B64);
    tft.drawString("3. MICRO-KINGDOM TD", 52, 182, &fonts::Font0);
    tft.setTextColor(0xFFFF, 0x1B64);
    tft.drawString("Tower Defense y rayo tactil", 52, 196, &fonts::Font0);

    tft.fillRoundRect(52, 212, 86, 14, 3, 0xFD20);
    tft.setTextColor(0x0000, 0xFD20);
    tft.drawCenterString("[ ESTRATEGIA ]", 95, 214, &fonts::Font0);

    // -------------------------------------------------------------
    // TARJETA 4: POCKET DUNGEON (Y = 242 a 302)
    // -------------------------------------------------------------
    tft.fillRoundRect(8, 242, 224, 60, 5, 0x2145);
    tft.drawRoundRect(8, 242, 224, 60, 5, COLOR_CARD_BORDER);

    // Icono Dado 3D
    tft.fillRoundRect(18, 260, 20, 20, 4, COLOR_DICE_YELLOW);
    tft.drawRoundRect(18, 260, 20, 20, 4, COLOR_WHITE);
    tft.fillCircle(24, 266, 2, 0x0000);
    tft.fillCircle(32, 274, 2, 0x0000);
    tft.fillCircle(28, 270, 2, 0x0000);

    tft.setTextColor(COLOR_GOLD_TXT, 0x2145);
    tft.drawString("4. POCKET DUNGEON", 52, 248, &fonts::Font0);
    tft.setTextColor(0xFFFF, 0x2145);
    tft.drawString("Rogue dice crawler (One Deck)", 52, 262, &fonts::Font0);

    tft.fillRoundRect(52, 278, 86, 14, 3, 0x2CD9);
    tft.setTextColor(0xFFFF, 0x2CD9);
    tft.drawCenterString("[ MAZMORRA ]", 95, 280, &fonts::Font0);

    // Pie de pantalla
    tft.setTextColor(0x8410, 0x10A2);
    tft.drawCenterString("ESP32-2432S028 CYD | 4-IN-1 CONSOLE", 120, 308, &fonts::Font0);
}

void launcher_handle_touch(int16_t tx, int16_t ty) {
    if (ty >= 44 && ty <= 104) {
        buzzer_click();
        launcher_set_active_app(APP_GOTCHI);
    } else if (ty >= 110 && ty <= 170) {
        buzzer_click();
        launcher_set_active_app(APP_PICROSS);
    } else if (ty >= 176 && ty <= 236) {
        buzzer_click();
        launcher_set_active_app(APP_TOWERDEF);
    } else if (ty >= 242 && ty <= 302) {
        buzzer_click();
        launcher_set_active_app(APP_DUNGEON);
    }
}
