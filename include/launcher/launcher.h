#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <Arduino.h>
#include "config.h"

AppID launcher_get_active_app();
void launcher_set_active_app(AppID app);
void launcher_return_home();

void launcher_init();
void launcher_draw();
void launcher_handle_touch(int16_t tx, int16_t ty);

#endif // LAUNCHER_H
