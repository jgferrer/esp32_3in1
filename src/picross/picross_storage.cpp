#include "picross/picross_storage.h"
#include <Preferences.h>

static Preferences prefs;

void picross_storage_init() {
}

bool picross_storage_is_completed(uint8_t level_idx) {
    if (prefs.begin("picross", true)) {
        char key[16];
        snprintf(key, sizeof(key), "c_%u", level_idx);
        bool val = prefs.getBool(key, false);
        prefs.end();
        return val;
    }
    return false;
}

uint32_t picross_storage_get_best_time(uint8_t level_idx) {
    if (prefs.begin("picross", true)) {
        char key[16];
        snprintf(key, sizeof(key), "t_%u", level_idx);
        uint32_t val = prefs.getUInt(key, 0);
        prefs.end();
        return val;
    }
    return 0;
}

void picross_storage_save_completion(uint8_t level_idx, uint32_t time_sec) {
    if (prefs.begin("picross", false)) {
        char key_c[16], key_t[16];
        snprintf(key_c, sizeof(key_c), "c_%u", level_idx);
        snprintf(key_t, sizeof(key_t), "t_%u", level_idx);

        prefs.putBool(key_c, true);

        uint32_t old_best = prefs.getUInt(key_t, 0);
        if (old_best == 0 || time_sec < old_best) {
            prefs.putUInt(key_t, time_sec);
        }
        prefs.end();
    }
}

uint8_t picross_storage_get_last_level() {
    if (prefs.begin("picross", true)) {
        uint8_t val = prefs.getUChar("last_lvl", 0);
        prefs.end();
        return val;
    }
    return 0;
}

void picross_storage_set_last_level(uint8_t level_idx) {
    if (prefs.begin("picross", false)) {
        prefs.putUChar("last_lvl", level_idx);
        prefs.end();
    }
}

bool picross_storage_get_sound() {
    if (prefs.begin("picross", true)) {
        bool val = prefs.getBool("sound", true);
        prefs.end();
        return val;
    }
    return true;
}

void picross_storage_set_sound(bool enabled) {
    if (prefs.begin("picross", false)) {
        prefs.putBool("sound", enabled);
        prefs.end();
    }
}
