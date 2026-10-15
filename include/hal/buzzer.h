#ifndef HAL_BUZZER_H
#define HAL_BUZZER_H

#include <Arduino.h>

void buzzer_init();
void buzzer_set_enabled(bool enabled);
bool buzzer_is_enabled();

void buzzer_beep(uint32_t freq, uint32_t duration_ms);
void buzzer_click();
void buzzer_play_click(); // Compatibilidad Gotchi

// Sonidos Tamagotchi
void buzzer_play_happy();
void buzzer_play_eat();
void buzzer_play_poop();
void buzzer_play_clean();
void buzzer_play_sick();

// Sonidos Picross
void buzzer_fill();
void buzzer_cross();
void buzzer_erase();
void buzzer_line_complete();
void buzzer_hint();
void buzzer_victory();

// Sonidos Tower Defense
void buzzer_build();
void buzzer_shoot_arrow();
void buzzer_cannon_boom();
void buzzer_ice_beam();
void buzzer_thunder_strike();
void buzzer_coin();
void buzzer_enemy_breach();
void buzzer_game_over();

#endif // HAL_BUZZER_H
