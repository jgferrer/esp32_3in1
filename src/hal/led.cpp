#include "hal/led.h"
#include "config.h"

void led_init() {
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    led_off();
}

void status_led_init() {
    led_init();
}

void led_set(bool r, bool g, bool b) {
    // Active-LOW
    digitalWrite(LED_RED_PIN, r ? LOW : HIGH);
    digitalWrite(LED_GREEN_PIN, g ? LOW : HIGH);
    digitalWrite(LED_BLUE_PIN, b ? LOW : HIGH);
}

void status_led_set(LedColor color) {
    switch (color) {
        case LED_COLOR_OFF:     led_set(false, false, false); break;
        case LED_COLOR_RED:     led_set(true, false, false);  break;
        case LED_COLOR_GREEN:   led_set(false, true, false);  break;
        case LED_COLOR_BLUE:    led_set(false, false, true);  break;
        case LED_COLOR_YELLOW:  led_set(true, true, false);   break;
        case LED_COLOR_MAGENTA: led_set(true, false, true);   break;
        case LED_COLOR_CYAN:    led_set(false, true, true);   break;
        case LED_COLOR_WHITE:   led_set(true, true, true);    break;
    }
}

void led_playing() {
    led_set(false, false, true);
}

void led_safe() {
    led_set(false, false, true);
}

void led_breach() {
    led_set(true, false, false);
}

void led_thunder() {
    led_set(true, true, false);
}

void led_victory() {
    led_set(false, true, false);
}

void led_hint() {
    led_set(true, true, false);
}

void led_dungeon() {
    led_set(false, false, true); // Azul tenue
}

void led_combat() {
    led_set(true, true, false); // Amarillo alerta
}

void led_damage() {
    led_set(true, false, false); // Rojo peligro
}

void led_off() {
    led_set(false, false, false);
}

