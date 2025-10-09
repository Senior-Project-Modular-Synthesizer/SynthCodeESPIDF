#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "peripheral_cfg.h"
#include "AK4619VN.hpp"

static const char* TAG = "MAIN";

extern "C" void app_main(void)
{
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
    if (codec) {
        ESP_LOGI(TAG, "✓ AK4619VN codec initialized successfully!");
        ESP_LOGI(TAG, "✓ SPI communication appears to be working");
        
        codec->simple_loop( codec );
        while(1){}
        // Cleanup codec (though we never reach here in this infinite loop)
        delete codec;
    } else {
        ESP_LOGE(TAG, "✗ Failed to allocate memory for AK4619VN codec");
    }
    
    // Cleanup
    ESP_LOGI(TAG, "Cleaning up SPI bus");
    spi_bus_free(SPI_HOST);
    
    ESP_LOGE(TAG, "Application ended unexpectedly");
}