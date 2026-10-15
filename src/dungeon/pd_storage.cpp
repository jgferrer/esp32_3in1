#include "dungeon/pd_storage.h"
#include <Preferences.h>

static Preferences prefs;
static const char* PREFS_NAMESPACE = "pockdungeon";

void pd_storage_init() {
}

uint32_t pd_storage_get_high_score() {
    if (!prefs.begin(PREFS_NAMESPACE, true)) return 0;
    uint32_t val = prefs.getUInt("hiscore", 0);
    prefs.end();
    return val;
}

void pd_storage_save_high_score(uint32_t score) {
    if (score <= pd_storage_get_high_score()) return;
    if (prefs.begin(PREFS_NAMESPACE, false)) {
        prefs.putUInt("hiscore", score);
        prefs.end();
    }
}

uint16_t pd_storage_get_victories() {
    if (!prefs.begin(PREFS_NAMESPACE, true)) return 0;
    uint16_t val = prefs.getUShort("victories", 0);
    prefs.end();
    return val;
}

void pd_storage_increment_victories() {
    uint16_t v = pd_storage_get_victories() + 1;
    if (prefs.begin(PREFS_NAMESPACE, false)) {
        prefs.putUShort("victories", v);
        prefs.end();
    }
}
