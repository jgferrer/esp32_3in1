#ifndef PICROSS_RENDERER_H
#define PICROSS_RENDERER_H

#include <Arduino.h>

void picross_renderer_init();
void picross_renderer_draw_screen(bool force_full_redraw = false);
void picross_renderer_draw_timer_only();
void picross_renderer_draw_cell(uint8_t r, uint8_t c);

#endif // PICROSS_RENDERER_H
