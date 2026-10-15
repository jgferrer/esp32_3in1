#ifndef TD_STORAGE_H
#define TD_STORAGE_H

#include <Arduino.h>

void td_storage_init();
uint32_t td_storage_get_high_score();
void td_storage_save_high_score(uint32_t score);
uint8_t td_storage_get_max_wave();
void td_storage_save_max_wave(uint8_t wave);
bool td_storage_get_sound();
void td_storage_set_sound(bool enabled);

#endif // TD_STORAGE_H
