#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "peripheral_cfg.h"
#include "AK4619VN.hpp"
#include "xpt2046.hpp"
#include "ui/ui.h"
#include "ui/screens/home/home_gen.h"
#include "ui/screens/effect/effect_gen.h"
#include "ui/screens/home/home_gen.h"
#include "lvgl.h"

#include "screen.hpp"
#include "touchscreen.hpp"
#include "Buffers.hpp"

#include "math.h"

static const char* TAG = "MAIN";

void loopback_main() {
        // Sleep to allow time for serial monitor to connect
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Starting ESP-Synth Application");
    ESP_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());
    
    // Initialize SPI bus (shared by multiple devices)
    ESP_LOGI(TAG, "Initializing SPI bus");
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &SPI_BUS_CFG, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "SPI bus initialized successfully");
    // Test AK4619VN codec initialization
    ESP_LOGI(TAG, "Testing AK4619VN codec initialization...");
    
    // Create codec instance on the stack - no exceptions needed
    AK4619VN* codec = new AK4619VN();
    SampleInputBuffer* inputBuffer = new SampleInputBuffer(codec->rx_chan);
    SampleOutputBuffer* outputBuffer = new SampleOutputBuffer(codec->tx_chan);
    inputBuffer->start();
    outputBuffer->start();
    int32_t sample_count = 0;
    float running_average = 0;
    ESP_LOGI(TAG, "Entering main processing loop");
    while (true) {
        QuadIntSample sample = inputBuffer->nextIntSample();
        float channel0_float = (float)(sample.channels[0]) / (float)(0x7FFFFF);
        float alpha = 0.01f;
        //float a = sinf(sample_count * 0.1f); // Varies between 0 and 1
        // Triangle wave
        //float a = triangle_wave(sample_count * 0.01f);
        running_average = (running_average * (1.0 - alpha) + channel0_float * alpha);
        //sample.channels[0] = (int32_t)(a * 0x3FFFFF);
        sample.channels[0] = (int32_t)(running_average * 0x7FFFFF);
        sample_count += 1;
        outputBuffer->pushIntSample(sample);
    }
    
    // Cleanup
    ESP_LOGI(TAG, "Cleaning up SPI bus");
    spi_bus_free(SPI_HOST);
    
    ESP_LOGE(TAG, "Application ended unexpectedly"); 
}

void screen_main() {
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &SPI_BUS_CFG, SPI_DMA_CH_AUTO);
    initialize_display();
    init_touchscreen();
    lv_init();

    lv_tick_set_cb(esp_log_early_timestamp);

    lv_display_t * display = lv_display_create(480, 320);

    const size_t buf_size = 480 * 320 / 10 * 3;

    uint8_t* buf = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);

    if (buf == NULL) {
        ESP_LOGE("SCREEN", "Failed to allocate LVGL buffer. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
        return;
    }


    lv_display_set_buffers(display, buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(display, ili9488_flush_cb);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchscreen_cb);


    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFF0000), LV_PART_MAIN);
    
    static lv_style_t st;
    lv_style_init(&st);
    lv_style_set_text_font(&st, &lv_font_montserrat_48);
    
    
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label , &st, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0xffff00), LV_PART_MAIN);
    lv_label_set_text(label, "Hello LVGL!");
    
    // Add slider for fun
    lv_obj_t * slider = lv_slider_create(lv_screen_active());
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 100);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
    /* Make LVGL periodically execute its tasks */
    while(1) {
        /* Provide updates to currently-displayed Widgets here. */
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void synth_gui(){
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &SPI_BUS_CFG, SPI_DMA_CH_AUTO);
    initialize_display();
    init_touchscreen();
    lv_init();

    gui_init();

    lv_tick_set_cb(esp_log_early_timestamp);

    lv_display_t * display = lv_display_create(480, 320);

    const size_t buf_size = 480 * 320 / 10 * 3;

    uint8_t* buf = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);

    if (buf == NULL) {
        ESP_LOGE("SCREEN", "Failed to allocate LVGL buffer. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
        return;
    }


    lv_display_set_buffers(display, buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(display, ili9488_flush_cb);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchscreen_cb);

    lv_obj_t * effect_screen = effect_create();
    lv_obj_set_parent(effect_screen, lv_screen_active());
}

void touchscreen_test() {
    
    vTaskDelay(500 / portTICK_PERIOD_MS);
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &SPI_BUS_CFG, SPI_DMA_CH_AUTO);
    XPT2046 touch(SPI_HOST, PIN_NUM_TOUCH_CS);
    touch.init();

    while (true) {
        XPT2046::TouchData touch_data = touch.readTouch();
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
            ESP_LOGI(TAG, "Touch: X=%u Y=%u Z=%u", x, y, touch_data.z);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}