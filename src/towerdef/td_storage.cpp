#include "towerdef/td_storage.h"
#include <Preferences.h>

static Preferences prefs;

void td_storage_init() {
}

uint32_t td_storage_get_high_score() {
    if (prefs.begin("mkingdom", true)) {
        uint32_t val = prefs.getUInt("hi_score", 0);
        prefs.end();
        return val;
    }
    return 0;
}

void td_storage_save_high_score(uint32_t score) {
    if (prefs.begin("mkingdom", false)) {
        uint32_t old = prefs.getUInt("hi_score", 0);
        if (score > old) {
            prefs.putUInt("hi_score", score);
        }
        prefs.end();
    }
}

uint8_t td_storage_get_max_wave() {
    if (prefs.begin("mkingdom", true)) {
        uint8_t val = prefs.getUChar("max_wave", 1);
        prefs.end();
        return val;
    }
    return 1;
}

void td_storage_save_max_wave(uint8_t wave) {
    if (prefs.begin("mkingdom", false)) {
        uint8_t old = prefs.getUChar("max_wave", 1);
        if (wave > old) {
            prefs.putUChar("max_wave", wave);
        }
        prefs.end();
    }
}

bool td_storage_get_sound() {
    if (prefs.begin("mkingdom", true)) {
        bool val = prefs.getBool("sound", true);
        prefs.end();
        return val;
    }
    return true;
}

void td_storage_set_sound(bool enabled) {
    if (prefs.begin("mkingdom", false)) {
        prefs.putBool("sound", enabled);
        prefs.end();
    }
}
