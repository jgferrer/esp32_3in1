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
        if (val <= APP_TOWERDEF) return (AppID)val;
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
    tft.fillRect(0, 0, SCREEN_WIDTH, 44, 0x18C3);
    tft.drawFastHLine(0, 44, SCREEN_WIDTH, 0xFDE0);

    tft.setTextColor(0xFDE0, 0x18C3);
    tft.drawCenterString("RETRO-STATION 3-IN-1", 120, 8, &fonts::Font2);
    tft.setTextColor(0xFFFF, 0x18C3);
    tft.drawCenterString("TOCA UN JUEGO CON EL LAPIZ", 120, 28, &fonts::Font0);

    // -------------------------------------------------------------
    // TARJETA 1: TAMAGOTCHI 16-BIT (Y = 52 a 128)
    // -------------------------------------------------------------
    tft.fillRoundRect(10, 52, 220, 76, 6, 0x2908);
    tft.drawRoundRect(10, 52, 220, 76, 6, 0xF81F);

    // Icono decorativo de mascota / huevo
    tft.fillCircle(36, 90, 16, 0xFDB4);
    tft.fillCircle(31, 86, 3, 0x0000);
    tft.fillCircle(41, 86, 3, 0x0000);
    tft.fillCircle(36, 94, 4, 0xF800);

    tft.setTextColor(0xFDE0, 0x2908);
    tft.drawString("1. TAMAGOTCHI 16-BIT", 62, 60, &fonts::Font0);
    tft.setTextColor(0xFFFF, 0x2908);
    tft.drawString("Mascota virtual retro", 62, 76, &fonts::Font0);
    tft.setTextColor(0xCE59, 0x2908);
    tft.drawString("Alimentar, banar y cuidar", 62, 90, &fonts::Font0);

    tft.fillRoundRect(62, 106, 80, 14, 3, 0x780F);
    tft.setTextColor(0xFFFF, 0x780F);
    tft.drawCenterString("[ MASCOTA ]", 102, 108, &fonts::Font0);

    // -------------------------------------------------------------
    // TARJETA 2: ESP-PICROSS RETRO (Y = 138 a 214)
    // -------------------------------------------------------------
    tft.fillRoundRect(10, 138, 220, 76, 6, 0x19E8);
    tft.drawRoundRect(10, 138, 220, 76, 6, 0x2EA5);

    // Icono decorativo de Hongo / Cuadrícula
    tft.fillCircle(36, 172, 14, 0xF800);
    tft.fillRect(28, 172, 16, 12, 0xFDE0);
    tft.fillCircle(32, 168, 3, 0xFFFF);
    tft.fillCircle(40, 168, 3, 0xFFFF);

    tft.setTextColor(0xFDE0, 0x19E8);
    tft.drawString("2. ESP-PICROSS RETRO", 62, 146, &fonts::Font0);
    tft.setTextColor(0xFFFF, 0x19E8);
    tft.drawString("15 Puzzles nonogramas", 62, 162, &fonts::Font0);
    tft.setTextColor(0xCE59, 0x19E8);
    tft.drawString("Modo pintar y cruces estilo DS", 62, 176, &fonts::Font0);

    tft.fillRoundRect(62, 192, 80, 14, 3, 0x2EA5);
    tft.setTextColor(0xFFFF, 0x2EA5);
    tft.drawCenterString("[ PUZZLE DS ]", 102, 194, &fonts::Font0);

    // -------------------------------------------------------------
    // TARJETA 3: MICRO-KINGDOM TOWER DEFENSE (Y = 224 a 300)
    // -------------------------------------------------------------
    tft.fillRoundRect(10, 224, 220, 76, 6, 0x1B64);
    tft.drawRoundRect(10, 224, 220, 76, 6, 0xFD20);

    // Icono decorativo de Torre del castillo
    tft.fillRect(26, 252, 20, 24, 0x7BEF);
    tft.drawRect(26, 252, 20, 24, 0x39E7);
    tft.fillRect(24, 246, 6, 6, 0x7BEF);
    tft.fillRect(33, 246, 6, 6, 0x7BEF);
    tft.fillRect(42, 246, 6, 6, 0x7BEF);

    tft.setTextColor(0xFDE0, 0x1B64);
    tft.drawString("3. MICRO-KINGDOM TD", 62, 232, &fonts::Font0);
    tft.setTextColor(0xFFFF, 0x1B64);
    tft.drawString("Tower Defense en vivo", 62, 248, &fonts::Font0);
    tft.setTextColor(0xCE59, 0x1B64);
    tft.drawString("Torres, jefes y rayo tactil", 62, 262, &fonts::Font0);

    tft.fillRoundRect(62, 278, 86, 14, 3, 0xFD20);
    tft.setTextColor(0x0000, 0xFD20);
    tft.drawCenterString("[ ESTRATEGIA ]", 105, 280, &fonts::Font0);

    // Pie de pantalla
    tft.setTextColor(0x8410, 0x10A2);
    tft.drawCenterString("ESP32-2432S028 CYD | Control Táctil", 120, 308, &fonts::Font0);
}

void launcher_handle_touch(int16_t tx, int16_t ty) {
    if (ty >= 52 && ty <= 128) {
        buzzer_click();
        launcher_set_active_app(APP_GOTCHI);
    } else if (ty >= 138 && ty <= 214) {
        buzzer_click();
        launcher_set_active_app(APP_PICROSS);
    } else if (ty >= 224 && ty <= 300) {
        buzzer_click();
        launcher_set_active_app(APP_TOWERDEF);
    }
}
