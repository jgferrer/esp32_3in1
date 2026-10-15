#include <Arduino.h>
#include "config.h"
#include "hal/display.h"
#include "hal/buzzer.h"
#include "hal/led.h"
#include "launcher/launcher.h"
#include "gotchi/gotchi_app.h"
#include "picross/picross_storage.h"
#include "picross/picross_game.h"
#include "picross/picross_renderer.h"
#include "towerdef/td_storage.h"
#include "towerdef/td_game_state.h"
#include "towerdef/td_renderer.h"

static AppID current_app = APP_LAUNCHER;
static uint32_t last_td_frame_ms = 0;

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=========================================");
    Serial.println("  ESP32-2432S028 RETRO STATION 3-IN-1    ");
    Serial.println("=========================================");

    // Periféricos base
    led_init();
    buzzer_init();
    display_init();

    current_app = launcher_get_active_app();
    Serial.printf("[MAIN] Booting App ID: %d\n", (int)current_app);

    switch (current_app) {
        case APP_GOTCHI:
            Serial.println("[MAIN] Iniciando Tamagotchi 16-Bit...");
            gotchi_init();
            break;

        case APP_PICROSS:
            Serial.println("[MAIN] Iniciando ESP-Picross Retro...");
            picross_storage_init();
            buzzer_set_enabled(picross_storage_get_sound());
            picross_renderer_init();
            picross_game.init();
            picross_renderer_draw_screen(true);
            break;

        case APP_TOWERDEF:
            Serial.println("[MAIN] Iniciando Micro-Kingdom Tower Defense...");
            td_storage_init();
            buzzer_set_enabled(td_storage_get_sound());
            td_renderer_init();
            td_game.init();
            td_renderer_draw();
            last_td_frame_ms = millis();
            break;

        case APP_LAUNCHER:
        default:
            Serial.println("[MAIN] Iniciando Retro-Station Launcher...");
            current_app = APP_LAUNCHER;
            launcher_init();
            launcher_draw();
            buzzer_play_happy();
            break;
    }
}

void loop() {
    switch (current_app) {
        case APP_GOTCHI:
            gotchi_loop();
            break;

        case APP_PICROSS: {
            uint16_t touchX, touchY;
            bool touched = tft.getTouch(&touchX, &touchY);
            static bool was_touched_picross = false;

            if (touched) {
                if (!was_touched_picross) {
                    was_touched_picross = true;
                    picross_game.handle_touch_down((int16_t)touchX, (int16_t)touchY);
                    picross_renderer_draw_screen(false);
                } else {
                    picross_game.handle_touch_move((int16_t)touchX, (int16_t)touchY);
                }
            } else {
                if (was_touched_picross) {
                    was_touched_picross = false;
                    picross_game.handle_touch_up();
                    picross_renderer_draw_screen(false);
                }
            }

            picross_game.update_timer();
            picross_renderer_draw_timer_only();
            delay(5);
            break;
        }

        case APP_TOWERDEF: {
            uint32_t now = millis();
            static uint32_t last_td_render_ms = 0;

            uint16_t touchX, touchY;
            bool touched = tft.getTouch(&touchX, &touchY);
            static bool was_touched_td = false;

            if (touched) {
                if (!was_touched_td) {
                    was_touched_td = true;
                    td_game.handle_touch((int16_t)touchX, (int16_t)touchY);
                }
            } else {
                was_touched_td = false;
            }

            // Limitación de refresco a ~30 FPS (33ms) para sincronizar con el bus SPI
            if (now - last_td_render_ms >= 33) {
                float dt = (now - last_td_frame_ms) / 1000.0f;
                if (dt > 0.1f) dt = 0.1f;
                last_td_frame_ms = now;
                last_td_render_ms = now;

                td_game.update(dt);
                td_renderer_draw();
            }

            delay(4);
            break;
        }

        case APP_LAUNCHER:
        default: {
            uint16_t touchX, touchY;
            bool touched = tft.getTouch(&touchX, &touchY);
            static bool was_touched_launcher = false;

            if (touched) {
                if (!was_touched_launcher) {
                    was_touched_launcher = true;
                    launcher_handle_touch((int16_t)touchX, (int16_t)touchY);
                }
            } else {
                was_touched_launcher = false;
            }
            delay(20);
            break;
        }
    }
}
