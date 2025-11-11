#include "touchscreen.hpp"
#include "lvgl.h"

#include "xpt2046.hpp"
#include "peripheral_cfg.h"
#include "esp_log.h"

static XPT2046* touch;

void init_touchscreen() {
    touch = new XPT2046(SPI_HOST, PIN_NUM_TOUCH_CS);
    esp_err_t ret = touch->init();
    if (ret != ESP_OK) {
        ESP_LOGE("TOUCHSCREEN", "Failed to initialize touchscreen: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI("TOUCHSCREEN", "Touchscreen initialized successfully");
    }
}

void touchscreen_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    if (!touch) {
        return;
    }
    XPT2046::TouchData touch_data = touch->readTouch();
    const int16_t x_min = 280;
    const int16_t x_max = 3860;
    const int16_t y_min = 340;
    const int16_t y_max = 3860;
    const int16_t screen_width = 480;
    const int16_t screen_height = 320;
    // Swap X and Y and invert both
    int16_t x = (touch_data.y - x_min) * screen_width / (x_max - x_min);
    x = screen_width - x;
    int16_t y = (touch_data.x - y_min) * screen_height / (y_max - y_min);
    y = screen_height - y;
    if (touch_data.z > 0 && x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
        last_x = x;
        last_y = y;
    } else {
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_RELEASED;
    }
    ESP_LOGI("TOUCHSCREEN", "Touch: X=%u Y=%u Z=%u", last_x, last_y, touch_data.z);

}