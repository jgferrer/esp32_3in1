#include "gotchi/gotchi_app.h"
#include <lvgl.h>
#include "hal/display.h"
#include "hal/buzzer.h"
#include "hal/led.h"
#include "gotchi/wifi_manager.h"
#include "gotchi/tamagotchi.h"
#include "gotchi/gotchi_ui.h"

#define LV_BUF_LINES 40
static lv_disp_draw_buf_t draw_buf;
static lv_color_t* buf = nullptr;

static void gotchi_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

static void gotchi_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);

    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void gotchi_init() {
    tft.setRotation(0);
    tft.setBrightness(255);

    status_led_init();
    status_led_set(LED_COLOR_YELLOW);

    buzzer_init();
    buzzer_play_happy();

    lv_init();

    if (!buf) {
        buf = (lv_color_t*)malloc(SCREEN_WIDTH * LV_BUF_LINES * sizeof(lv_color_t));
    }

    // Inicializar buffer de render
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * LV_BUF_LINES);

    // Inicializar driver de pantalla
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = gotchi_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Inicializar driver táctil
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = gotchi_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // WiFi, NTP, NVS y UI
    wifi_manager_init();
    pet.init();
    ui_init();

    status_led_set(LED_COLOR_GREEN);
}

void gotchi_loop() {
    lv_timer_handler();
    pet.update();
    wifi_manager_update();
    ui_update();
    delay(5);
}
