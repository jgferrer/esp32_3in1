#ifndef PICROSS_STORAGE_H
#define PICROSS_STORAGE_H

#include <Arduino.h>

void picross_storage_init();
bool picross_storage_is_completed(uint8_t level_idx);
uint32_t picross_storage_get_best_time(uint8_t level_idx);
void picross_storage_save_completion(uint8_t level_idx, uint32_t time_sec);
uint8_t picross_storage_get_last_level();
void picross_storage_set_last_level(uint8_t level_idx);
bool picross_storage_get_sound();
void picross_storage_set_sound(bool enabled);

#endif // PICROSS_STORAGE_H
