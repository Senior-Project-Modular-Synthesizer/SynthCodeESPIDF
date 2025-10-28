#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "peripheral_cfg.h"
#include "AK4619VN.hpp"

#include "lvgl.h"

#include "screen.hpp"
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
        float alpha = sin(sample_count * 0.0001f) * 0.5f + 0.5f; // Varies between 0 and 1
        if (sample_count++ % 10000 == 0) {
            ESP_LOGI(TAG, "Channel 0 Sample: %d, Float: %.6f, Running Average: %.6f", sample.channels[0], channel0_float, running_average);
        }
        running_average = (running_average * (1.0 - alpha) + channel0_float * alpha);
        sample.channels[0] = (int32_t)(running_average * 0x7FFFFF);
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

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFF0000), LV_PART_MAIN);
    
    static lv_style_t st;
    lv_style_init(&st);
    lv_style_set_text_font(&st, &lv_font_montserrat_48);
    
    
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label , &st, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(0xffff00), LV_PART_MAIN);
    lv_label_set_text(label, "Hello LVGL!");
    

    /* Make LVGL periodically execute its tasks */
    while(1) {
        /* Provide updates to currently-displayed Widgets here. */
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

extern "C" void app_main(void)
{
    loopback_main();
}
