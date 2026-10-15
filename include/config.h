#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// PANTALLA TFT 2.8" (ESP32-2432S028 CYD)
// ==========================================
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   320
#define SCREEN_ROTATION 0 // Vertical nativo

// Pines TFT (Bus HSPI)
#define TFT_MISO_PIN   12
#define TFT_MOSI_PIN   13
#define TFT_SCLK_PIN   14
#define TFT_CS_PIN     15
#define TFT_DC_PIN      2
#define TFT_RST_PIN    -1
#define TFT_BL_PIN     21

// Pines Panel Táctil Resistivo XPT2046 (Software SPI)
#define TOUCH_MOSI_PIN 32
#define TOUCH_MISO_PIN 39
#define TOUCH_CLK_PIN  25
#define TOUCH_CS_PIN   33
#define TOUCH_IRQ_PIN  36

// ==========================================
// PERIFÉRICOS
// ==========================================
// LED RGB Integrado (Active-LOW: 0=ON, 1=OFF)
#define LED_RED_PIN    4
#define LED_GREEN_PIN  16
#define LED_BLUE_PIN   17

// Audio Buzzer (PWM)
#define BUZZER_PIN     26
#define BUZZER_CHANNEL  0

// Sensor LDR (Opcional)
#define LDR_PIN        34

// ==========================================
// ID DE APLICACIONES RETRO 3-EN-1
// ==========================================
enum AppID : uint8_t {
    APP_LAUNCHER = 0,
    APP_GOTCHI   = 1,
    APP_PICROSS  = 2,
    APP_TOWERDEF = 3,
    APP_DUNGEON  = 4
};

// ==========================================
// CONSTANTES TAMAGOTCHI
// ==========================================
#define TICK_INTERVAL_MS     5000
#define SAVE_INTERVAL_MS    30000
#define MAX_POOPS               4
#define HUNGER_DECREASE_TICK    1
#define HAPPINESS_DECREASE_TICK 1
#define ENERGY_DECREASE_TICK    1

// ==========================================
// CONSTANTES MICRO-KINGDOM (TOWER DEFENSE)
// ==========================================
#define BATTLEFIELD_Y_START 24
#define BATTLEFIELD_HEIGHT  222
#define BOTTOM_TRAY_Y_START 246
#define BOTTOM_TRAY_HEIGHT  74

#define INITIAL_LIVES   10
#define INITIAL_GOLD    140
#define TOTAL_WAVES     10

#define MAX_ENEMIES     16
#define MAX_TOWERS      8
#define MAX_PROJECTILES 24
#define MAX_PARTICLES   16

// Colores Tower Defense
#define COLOR_GRASS       0x2444
#define COLOR_GRASS_DARK  0x1BA2
#define COLOR_PATH        0xCE55
#define COLOR_PATH_BORDER 0x93A5
#define COLOR_CASTLE      0x7BEF
#define COLOR_WATER       0x2B9F
#define COLOR_HUD_BG      0x10A2
#define COLOR_TRAY_BG     0x18E3
#define COLOR_GOLD_TXT    0xFDE0
#define COLOR_HEART_TXT   0xF882
#define COLOR_WHITE       0xFFFF
#define COLOR_BLACK       0x0000

// Colores Picross
#define COLOR_GRID_BG     0xF7BE
#define COLOR_GRID_LINE   0xAD55
#define COLOR_GRID_MAJOR  0x4208
#define COLOR_FILLED      0x18C3
#define COLOR_CROSS       0xD185
#define COLOR_TEXT_MUTED  0x8410
#define COLOR_ACCENT      0x2D7F
#define COLOR_ACCENT_ACT  0xFDE0
#define COLOR_SUCCESS     0x2EA5

// Colores Pocket Dungeon
#define COLOR_DUNGEON_BG     0x10A2
#define COLOR_CARD_BG        0x2145
#define COLOR_CARD_BORDER    0xC586
#define COLOR_DICE_YELLOW    0xFDE0
#define COLOR_DICE_GREEN     0x36E6
#define COLOR_DICE_BLUE      0x34BF
#define COLOR_DICE_HEROIC    0x2965
#define COLOR_HP_RED         0xF904
#define COLOR_POTION_CYAN    0x1E5F
#define COLOR_XP_PURPLE      0xB9DF

#endif // CONFIG_H
