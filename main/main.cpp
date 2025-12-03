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

#include "ui/ui.h"

#include "screen.hpp"
#include "Buffers.hpp"
#include "touchscreen.hpp"

#include "processors/basics.hpp"

#define TAG "MAIN"


Processor *processor = nullptr;
QuadInputBuffer *input_buffer = nullptr;
QuadOutputBuffer *output_buffer = nullptr;
AK4619VN* codec = nullptr;

TaskHandle_t processor_task_handle = nullptr;

void init_screen() {
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

    gui_init();
}

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
    codec = new AK4619VN();
    input_buffer = new SampleInputBuffer(codec->rx_chan);
    output_buffer = new SampleOutputBuffer(codec->tx_chan);

    init_screen();
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
        // Stop the processor task before deleting
        if (processor_task_handle) {
            vTaskDelete(processor_task_handle);
            processor_task_handle = nullptr;
        }
        delete processor;
        processor = nullptr;
    }
    ESP_LOGI(TAG, "Debug Point 4");

    esp_err_t ret;

    vTaskDelay(100 / portTICK_PERIOD_MS); // Wait for tasks to stop
    ESP_LOGI(TAG, "Debug Point 5");
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
    init_devices();
    new_processor("Filter");
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void)
{
    // init_devices();
    // registerBasicProcessors();
    // // Start the main task on core 0
    
    // while (true) {
    //     new_processor("LowPass");
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    //     new_processor("HighPass");
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }

    synth_gui();
    while (1) {

    }
}
