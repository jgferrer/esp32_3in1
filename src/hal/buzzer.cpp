#include "hal/buzzer.h"
#include "config.h"

static bool sound_enabled = true;

void buzzer_init() {
    ledcSetup(BUZZER_CHANNEL, 2000, 8);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_set_enabled(bool enabled) {
    sound_enabled = enabled;
    if (!sound_enabled) {
        ledcWrite(BUZZER_CHANNEL, 0);
    }
}

bool buzzer_is_enabled() {
    return sound_enabled;
}

void buzzer_beep(uint32_t freq, uint32_t duration_ms) {
    if (!sound_enabled || freq == 0) {
        ledcWrite(BUZZER_CHANNEL, 0);
        return;
    }
    ledcWriteTone(BUZZER_CHANNEL, freq);
    delay(duration_ms);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_click() {
    buzzer_beep(2200, 8);
}

void buzzer_play_click() {
    buzzer_click();
}

// -------------------------------------------------------------
// TAMAGOTCHI
// -------------------------------------------------------------
void buzzer_play_happy() {
    buzzer_beep(988, 70);  // B5
    buzzer_beep(1319, 70); // E6
    buzzer_beep(1760, 100);// A6
}

void buzzer_play_eat() {
    for (int i = 0; i < 3; i++) {
        buzzer_beep(600, 40);
        delay(30);
        buzzer_beep(900, 50);
        delay(40);
    }
}

void buzzer_play_poop() {
    buzzer_beep(400, 100);
    buzzer_beep(300, 150);
    buzzer_beep(200, 200);
}

void buzzer_play_clean() {
    buzzer_beep(1200, 60);
    buzzer_beep(1500, 60);
    buzzer_beep(1800, 80);
}

void buzzer_play_sick() {
    buzzer_beep(350, 120);
    buzzer_beep(300, 120);
    buzzer_beep(250, 180);
}

// -------------------------------------------------------------
// PICROSS
// -------------------------------------------------------------
void buzzer_fill() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 880);
    delay(12);
    ledcWriteTone(BUZZER_CHANNEL, 1320);
    delay(15);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_cross() {
    buzzer_beep(1800, 10);
}

void buzzer_erase() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 600);
    delay(10);
    ledcWriteTone(BUZZER_CHANNEL, 400);
    delay(10);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_line_complete() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 1046);
    delay(30);
    ledcWriteTone(BUZZER_CHANNEL, 1318);
    delay(35);
    ledcWriteTone(BUZZER_CHANNEL, 1568);
    delay(45);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_hint() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 1200);
    delay(20);
    ledcWriteTone(BUZZER_CHANNEL, 1600);
    delay(25);
    ledcWriteTone(BUZZER_CHANNEL, 2000);
    delay(30);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_victory() {
    if (!sound_enabled) return;
    const uint16_t notes[] = { 1046, 1318, 1568, 2093, 1760, 2093 };
    const uint8_t dur[] = { 60,   60,   60,  120,   80,  250 };
    for (size_t i = 0; i < sizeof(notes)/sizeof(notes[0]); i++) {
        ledcWriteTone(BUZZER_CHANNEL, notes[i]);
        delay(dur[i]);
        ledcWrite(BUZZER_CHANNEL, 0);
        delay(15);
    }
}

// -------------------------------------------------------------
// TOWER DEFENSE
// -------------------------------------------------------------
void buzzer_build() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 700);
    delay(20);
    ledcWriteTone(BUZZER_CHANNEL, 1050);
    delay(25);
    ledcWriteTone(BUZZER_CHANNEL, 1400);
    delay(30);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_shoot_arrow() {
    buzzer_beep(1800, 8);
}

void buzzer_cannon_boom() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 180);
    delay(30);
    ledcWriteTone(BUZZER_CHANNEL, 120);
    delay(35);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_ice_beam() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 1600);
    delay(15);
    ledcWriteTone(BUZZER_CHANNEL, 2200);
    delay(15);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_thunder_strike() {
    if (!sound_enabled) return;
    for (int i = 0; i < 4; i++) {
        ledcWriteTone(BUZZER_CHANNEL, 1500 - (i * 300));
        delay(15);
    }
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_coin() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 1318);
    delay(15);
    ledcWriteTone(BUZZER_CHANNEL, 1760);
    delay(25);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_enemy_breach() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 300);
    delay(50);
    ledcWriteTone(BUZZER_CHANNEL, 200);
    delay(80);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_game_over() {
    if (!sound_enabled) return;
    const uint16_t notes[] = { 440, 392, 349, 293 };
    const uint8_t dur[] = { 100, 100, 100, 250 };
    for (size_t i = 0; i < sizeof(notes)/sizeof(notes[0]); i++) {
        ledcWriteTone(BUZZER_CHANNEL, notes[i]);
        delay(dur[i]);
        ledcWrite(BUZZER_CHANNEL, 0);
        delay(20);
    }
}

void buzzer_dice_roll() {
    if (!sound_enabled) return;
    for (int i = 0; i < 4; i++) {
        ledcWriteTone(BUZZER_CHANNEL, 1200 + (rand() % 800));
        delay(18);
        ledcWrite(BUZZER_CHANNEL, 0);
        delay(12);
    }
}

void buzzer_dice_place() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 1400);
    delay(15);
    ledcWriteTone(BUZZER_CHANNEL, 1900);
    delay(20);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_damage() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 220);
    delay(60);
    ledcWriteTone(BUZZER_CHANNEL, 140);
    delay(90);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_skill() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 1000);
    delay(20);
    ledcWriteTone(BUZZER_CHANNEL, 1500);
    delay(25);
    ledcWriteTone(BUZZER_CHANNEL, 2100);
    delay(35);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_potion() {
    if (!sound_enabled) return;
    ledcWriteTone(BUZZER_CHANNEL, 1200);
    delay(25);
    ledcWriteTone(BUZZER_CHANNEL, 1600);
    delay(30);
    ledcWriteTone(BUZZER_CHANNEL, 2000);
    delay(40);
    ledcWrite(BUZZER_CHANNEL, 0);
}

