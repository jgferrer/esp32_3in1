#ifndef HAL_LED_H
#define HAL_LED_H

#include <Arduino.h>

enum LedColor {
    LED_COLOR_OFF,
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_BLUE,
    LED_COLOR_YELLOW,
    LED_COLOR_MAGENTA,
    LED_COLOR_CYAN,
    LED_COLOR_WHITE
};

void led_init();
void led_set(bool r, bool g, bool b);
void status_led_init(); // Compatibilidad Gotchi
void status_led_set(LedColor color);

void led_playing();
void led_safe();
void led_breach();
void led_thunder();
void led_victory();
void led_hint();
void led_off();

#endif // HAL_LED_H
