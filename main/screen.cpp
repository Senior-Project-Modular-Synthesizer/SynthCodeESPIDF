#include "screen.hpp"

#include "peripheral_cfg.h"
#include "ili9488.hpp"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_heap_caps.h"

static const unsigned int DISPLAY_REFRESH_HZ = 40000000;
static const int DISPLAY_COMMAND_BITS = 8;
static const int DISPLAY_PARAMETER_BITS = 8;

static esp_lcd_panel_io_handle_t lcd_io_handle = NULL;
static esp_lcd_panel_handle_t lcd_handle = NULL;

// Reduce this from SCREEN_WIDTH * 25 to much smaller to save heap
static const size_t LV_BUFFER_SIZE = SCREEN_WIDTH * PARALLEL_LINES; // smaller draw buffer

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



void initialize_display() {
    ESP_LOGI("SCREEN", "Free heap before display init: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);

    const esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_SCREEN_CS,
        .dc_gpio_num = PIN_NUM_SCREEN_DC,
        .spi_mode = 0,
        .pclk_hz = DISPLAY_REFRESH_HZ,
        .trans_queue_depth = PARALLEL_LINES,
        .on_color_trans_done = NULL,
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
}
#include "buf.h"

void screen_thingy() {
    initialize_display();

    const int rows = PARALLEL_LINES;
    const int cols = 320;
    size_t buf_size = cols * rows * sizeof(uint8_t) * 3;
    ESP_LOGI("SCREEN", "Allocating screen buffer of %u bytes", (unsigned)buf_size);
    //esp_lcd_panel_swap_xy(lcd_handle, true);
    esp_lcd_panel_mirror(lcd_handle, false, false);
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
                ESP_LOGI("SCREEN", "Logging the first 48 bytes of the screen buffer:");
                for (int i = 0; i < 48; i+=3) {
                    ESP_LOGI("SCREEN", "Pixel %d: R=%02X G=%02X B=%02X", i/3, screen_buffer[i], screen_buffer[i+1], screen_buffer[i+2]);
                }
                ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(lcd_handle, 0, yt, cols, yt + rows, screen_buffer));
                vTaskDelay(35 / portTICK_PERIOD_MS);
            }
        }
    }
    // size_t buf_size = cols * rows * sizeof(uint16_t);
    // ESP_LOGI("SCREEN", "Allocating screen buffer of %u bytes", (unsigned)buf_size);
    // uint16_t *screen_buffer = (uint16_t *)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    // if (screen_buffer == NULL) {
    //     ESP_LOGE("SCREEN", "Failed to allocate screen buffer. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    // } else {
    //     for(int j = 0; j < SCREEN_HEIGHT/rows; j++) {
    //         for (int i = 0; i < buf_size; i+=2) {
    //             int screen_color = i / 2 + j * cols * rows;
    //             // RGB565 format
    //             screen_buffer[i/2] = (uint16_t)i/2 + (uint16_t)(j * cols * rows); // Just a gradient for testing
    //         }
    //         ESP_LOGI("SCREEN", "Logging the first 24 uint16_t values of the screen buffer:");
    //         for (int i = 0; i < 48; i+=2) {
    //             ESP_LOGI("SCREEN", "Pixel %d: RGB565=%04X", i/2, screen_buffer[i/2]);
    //         }
    //         ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(lcd_handle, 0, rows * j, cols, rows * (j + 1), screen_buffer));
    //     }
    // }   

    heap_caps_free(screen_buffer);
    ESP_LOGI("SCREEN", "Draw complete. Free heap now: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("SCREEN", "Doing nothing in screen loop. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    }
}
    // size_t buf_size = cols * rows * sizeof(uint8_t) * 3;
    // ESP_LOGI("SCREEN", "Allocating screen buffer of %u bytes", (unsigned)buf_size);

    // uint8_t *screen_buffer = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    // if (screen_buffer == NULL) {
    //     ESP_LOGE("SCREEN", "Failed to allocate screen buffer. Free heap: %u", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    // } else {
    //     for(int j = 0; j < SCREEN_HEIGHT/rows; j++) {
    //         for (int i = 0; i < buf_size; i+=3) {
    //             int screen_color = i / 2 + j * cols * rows;
    //             // screen_buffer[i] = (screen_color & ((1 << 6) - 1)) << 2;
    //             // screen_buffer[i+1] = ((screen_color >> 6) & ((1 << 6) - 1)) << 2;
    //             // screen_buffer[i+2] = ((screen_color >> 12) & ((1 << 6) - 1)) << 2;
    //             screen_buffer[i+2] = (uint8_t)(screen_color & 0x3F) << 2;
    //             screen_buffer[i+1] = (uint8_t)((screen_color >> 6) & 0x3F) << 2;
    //             screen_buffer[i] = (uint8_t)((screen_color >> 12) & 0x3F) << 2;
    //         }
    //         ESP_LOGI("SCREEN", "Logging the first 48 bytes of the screen buffer:");
    //         for (int i = 0; i < 48; i+=3) {
    //             ESP_LOGI("SCREEN", "Pixel %d: R=%02X G=%02X B=%02X", i/3, screen_buffer[i], screen_buffer[i+1], screen_buffer[i+2]);
    //         }
    //         ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(lcd_handle, 0, rows * j, cols, rows * (j + 1), screen_buffer));
    //     }
    // }
