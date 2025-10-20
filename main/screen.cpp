#include "screen.hpp"

#include "peripheral_cfg.h"
#include "ili9488.hpp"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_heap_caps.h"
#include "freertos/event_groups.h"

#include "lvgl.h"

static const unsigned int DISPLAY_REFRESH_HZ = 40000000;
static const int DISPLAY_COMMAND_BITS = 8;
static const int DISPLAY_PARAMETER_BITS = 8;

static esp_lcd_panel_io_handle_t lcd_io_handle = NULL;
static esp_lcd_panel_handle_t lcd_handle = NULL;

// Reduce this from SCREEN_WIDTH * 25 to much smaller to save heap
static const size_t LV_BUFFER_SIZE = SCREEN_WIDTH * PARALLEL_LINES; // smaller draw buffer

EventGroupHandle_t xHandle = NULL;

#include "esp_heap_caps.h"

void dump_all_capabilities(const char *tag)
{
    size_t dflt = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t dma = heap_caps_get_free_size(MALLOC_CAP_DMA);
    size_t spiram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t eightbit = heap_caps_get_free_size(MALLOC_CAP_8BIT); // if your headers accept numeric name use MALLOC_CAP_8BIT
    ESP_LOGI(tag, "HEAP free sizes: DEFAULT=%u INTERNAL=%u DMA=%u SPIRAM=%u 8BIT=%u",
             (unsigned)dflt, (unsigned)internal, (unsigned)dma, (unsigned)spiram, (unsigned)eightbit);

    heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
    heap_caps_print_heap_info(MALLOC_CAP_DMA);
    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
}

bool color_trans_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    // This callback is called when a color data transmission is done
    // We can use this to signal that we can send the next chunk of data
    // For now, just log and return
    xEventGroupSetBits(xHandle, BIT0);
    return true; // Return true to indicate success
}

void initialize_display() {
    xHandle = xEventGroupCreate();

    ESP_LOGI("SCREEN", "Free heap before display init: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);

    const esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_SCREEN_CS,
        .dc_gpio_num = PIN_NUM_SCREEN_DC,
        .spi_mode = 0,
        .pclk_hz = DISPLAY_REFRESH_HZ,
        .trans_queue_depth = PARALLEL_LINES,
        .on_color_trans_done = color_trans_done,
        .user_ctx = NULL,
        .lcd_cmd_bits = DISPLAY_COMMAND_BITS,
        .lcd_param_bits = DISPLAY_PARAMETER_BITS,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .flags =
        {
            .dc_low_on_data = 0,
            .octal_mode = 0,
            .sio_mode = 0,
            .lsb_first = 0,
            .cs_high_active = 0
        }
    };

    const esp_lcd_panel_dev_config_t lcd_config = {
        .reset_gpio_num = PIN_NUM_SCREEN_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 24,
        .flags =
        {
            .reset_active_high = 0,
        },
        .vendor_config = NULL
    };

    // Log heap right before the calls that allocate internal queues
    ESP_LOGI("SCREEN", "Free heap just before esp_lcd_new_panel_io_spi: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    dump_all_capabilities("SCREEN");
    esp_err_t err = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI_HOST, &io_config, &lcd_io_handle);
    if (err != ESP_OK) {
        ESP_LOGE("SCREEN", "esp_lcd_new_panel_io_spi failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI("SCREEN", "Free heap after esp_lcd_new_panel_io_spi: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

    err = esp_lcd_new_panel_ili9488(lcd_io_handle, &lcd_config, LV_BUFFER_SIZE, &lcd_handle);
    if (err != ESP_OK) {
        ESP_LOGE("SCREEN", "esp_lcd_new_panel_ili9488 failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_ERROR_CHECK(esp_lcd_panel_reset(lcd_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(lcd_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(lcd_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(lcd_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(lcd_handle, 0, 0));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_handle, true));

    ESP_LOGI("SCREEN", "Display init complete. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

    esp_lcd_panel_swap_xy(lcd_handle, true);

}
#include "buf.h"

void screen_thingy() {
    initialize_display();

    if (xHandle != NULL) {
        ESP_LOGI("SCREEN", "Create SUCCESS");
    }
    else {
        ESP_LOGE("SCREEN", "Failed to create event group");
    }

    const int rows = PARALLEL_LINES;
    const int cols = 320;
    size_t buf_size = cols * rows * sizeof(uint8_t) * 3;
    ESP_LOGI("SCREEN", "Allocating screen buffer of %u bytes", (unsigned)buf_size);
    //esp_lcd_panel_mirror(lcd_handle, false, false);
    uint8_t *screen_buffer = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    if (screen_buffer == NULL) {
        ESP_LOGE("SCREEN", "Failed to allocate screen buffer. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    } else {
        while(true) {
            for(int yt = 0; yt < SCREEN_HEIGHT; yt+=rows) {
                for (int y = yt; y < yt + rows; y++) {
                    for (int x = 0; x < cols; x++) {
                        screen_buffer[(y - yt) * cols * 3 + x * 3 + 0] = image_data[y][x][2];
                        screen_buffer[(y - yt) * cols * 3 + x * 3 + 1] = image_data[y][x][1];
                        screen_buffer[(y - yt) * cols * 3 + x * 3 + 2] = image_data[y][x][0];
                    }
                }
                ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(lcd_handle, 0, yt, cols, yt + rows, screen_buffer));
                ESP_LOGI("SCREEN", "Submitted rows %d to %d for drawing", yt, yt + rows);
                //xEventGroupWaitBits(xHandle, BIT0, pdTRUE, pdTRUE, portMAX_DELAY);
                while (!xEventGroupGetBits(xHandle) & BIT0) {
                    ESP_LOGI("SCREEN", "Waiting for draw to complete for rows %d to %d", yt, yt + rows);
                    ESP_LOGI("SCREEN", "Group Bits: %u", (unsigned)xEventGroupGetBits(xHandle));
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
                xEventGroupClearBits(xHandle, BIT0);
                ESP_LOGI("SCREEN", "Drawn rows %d to %d", yt, yt + rows);
            }
        }
    }  

    heap_caps_free(screen_buffer);
    ESP_LOGI("SCREEN", "Draw complete. Free heap now: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("SCREEN", "Doing nothing in screen loop. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    }
}

void ili9488_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf) {
    int32_t x1 = area->x1;
    int32_t y1 = area->y1;
    int32_t x2 = area->x2;
    int32_t y2 = area->y2;
    esp_lcd_panel_draw_bitmap(lcd_handle, x1, y1, x2 + 1, y2 + 1, px_buf);
    // Wait for the drawing to complete
    xEventGroupWaitBits(xHandle, BIT0, pdTRUE, pdTRUE, portMAX_DELAY);
    lv_display_flush_ready(disp);
}
