#ifndef PD_STORAGE_H
#define PD_STORAGE_H

#include <Arduino.h>

void pd_storage_init();
uint32_t pd_storage_get_high_score();
void pd_storage_save_high_score(uint32_t score);
uint16_t pd_storage_get_victories();
void pd_storage_increment_victories();

#endif // PD_STORAGE_H
