#include "debug.hpp"

#include <string>

#include "Processor.hpp"
#include "processor_factory.hpp"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "peripheral_cfg.h"
#include "AK4619VN.hpp"
#include "xpt2046.hpp"

#include "lvgl.h"

#include "screen.hpp"
#include "Buffers.hpp"

#include "processors/basics.hpp"

#define TAG "MAIN"


Processor *processor = nullptr;
QuadInputBuffer *input_buffer = nullptr;
QuadOutputBuffer *output_buffer = nullptr;

TaskHandle_t processor_task_handle = nullptr;

void init_devices() {
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
    input_buffer = new SampleInputBuffer(codec->rx_chan);
    output_buffer = new SampleOutputBuffer(codec->tx_chan);

}

static void start_processor(void* pvParameters) {
    if (processor == nullptr) {
        printf("No processor to start\n");
        return;
    }
    processor->process(*input_buffer, *output_buffer);
}

void new_processor(std::string name) {
    ESP_LOGI(TAG, "Creating processor '%s'", name.c_str());
    if (processor_task_handle != nullptr) {
        vTaskDelete(processor_task_handle);
        processor_task_handle = nullptr;
    }
    ESP_LOGI(TAG, "Debug Point 1");
    if (input_buffer != nullptr) {
        input_buffer->stop();
    }
    ESP_LOGI(TAG, "Debug Point 2");
    if (output_buffer != nullptr) {
        output_buffer->stop();
    }
    ESP_LOGI(TAG, "Debug Point 3");
    if (processor != nullptr) {
        delete processor;
        processor = nullptr;
    }
    ESP_LOGI(TAG, "Debug Point 4");
    processor = ProcessorFactory::instance().createProcessor(name).release();
    if (processor == nullptr) {
        printf("Processor '%s' not found\n", name.c_str());
    } else {
        printf("Processor '%s' created\n", name.c_str());
    }
    auto ui = processor->getUIType();
    /// TODO: Handle UI
    input_buffer->start();
    output_buffer->start();
    xTaskCreatePinnedToCore(start_processor, "processor_task", 4096, nullptr, 1, &processor_task_handle, 1);
}

void main_task_wrapper(void* pvParameters) {
    ESP_LOGI(TAG, "Starting main task");
    new_processor("Filter");
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void)
{
    init_devices();
    registerBasicProcessors();
    // Start the main task on core 0
    new_processor("Filter");
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
