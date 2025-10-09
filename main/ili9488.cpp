/**
 * @file ili9488.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "ili9488.hpp"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "peripheral_cfg.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*********************
 *      DEFINES
 *********************/
 #define TAG "ILI9488"

/**********************
 *      TYPEDEFS
 **********************/

/*The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct. */
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ili9488_set_orientation(uint8_t orientation);

static void ili9488_send_cmd(uint8_t cmd);
static void ili9488_send_data(void * data, uint16_t length);
static void ili9488_send_color(void * data, uint16_t length);

/**********************
 *  STATIC VARIABLES
 **********************/
static spi_device_handle_t ili9488_spi;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
// From github.com/jeremyjh/ESP32_TFT_library
// From github.com/mvturnho/ILI9488-lvgl-ESP32-WROVER-B
void ili9488_init(void)
{
	lcd_init_cmd_t ili_init_cmds[]={
                {ILI9488_CMD_SLEEP_OUT, {0x00}, 0x80},
		{ILI9488_CMD_POSITIVE_GAMMA_CORRECTION, {0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F}, 15},
		{ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION, {0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F}, 15},
		{ILI9488_CMD_POWER_CONTROL_1, {0x17, 0x15}, 2},
		{ILI9488_CMD_POWER_CONTROL_2, {0x41}, 1},
		{ILI9488_CMD_VCOM_CONTROL_1, {0x00, 0x12, 0x80}, 3},
		{ILI9488_CMD_MEMORY_ACCESS_CONTROL, {(0x20 | 0x08)}, 1},
		{ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET, {0x66}, 1},
		{ILI9488_CMD_INTERFACE_MODE_CONTROL, {0x00}, 1},
		{ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL, {0xA0}, 1},
		{ILI9488_CMD_DISPLAY_INVERSION_CONTROL, {0x02}, 1},
		{ILI9488_CMD_DISPLAY_FUNCTION_CONTROL, {0x02, 0x02}, 2},
		{ILI9488_CMD_SET_IMAGE_FUNCTION, {0x00}, 1},
		{ILI9488_CMD_WRITE_CTRL_DISPLAY, {0x28}, 1},
		{ILI9488_CMD_WRITE_DISPLAY_BRIGHTNESS, {0x7F}, 1},
		{ILI9488_CMD_ADJUST_CONTROL_3, {0xA9, 0x51, 0x2C, 0x02}, 4},
		{ILI9488_CMD_DISPLAY_ON, {0x00}, 0x80},
		{0, {0}, 0xff},
	};
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1 << PIN_NUM_SCREEN_DC;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

#if ILI9488_USE_RST
	gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1 << PIN_NUM_SCREEN_RST;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

	//Reset the display
	gpio_set_level(PIN_NUM_SCREEN_RST, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(PIN_NUM_SCREEN_RST, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
#endif

	ESP_LOGI(TAG, "ILI9488 initialization.");

	// Initialize SPI device
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz = 40 * 1000 * 1000, // 40 MHz
		.mode = 0,
		.spics_io_num = PIN_NUM_SCREEN_CS,
		.queue_size = 7,
		.flags = 0,
	};
	esp_err_t ret = spi_bus_add_device(SPI_HOST, &devcfg, &ili9488_spi);
	assert(ret == ESP_OK);

	// Exit sleep
	ili9488_send_cmd(0x01);	/* Software reset */
	vTaskDelay(100 / portTICK_RATE_MS);

	//Send all the commands
	uint16_t cmd = 0;
	while (ili_init_cmds[cmd].databytes!=0xff) {
		ili9488_send_cmd(ili_init_cmds[cmd].cmd);
		ili9488_send_data(ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes&0x1F);
		if (ili_init_cmds[cmd].databytes & 0x80) {
			vTaskDelay(100 / portTICK_RATE_MS);
		}
		cmd++;
	}

    ili9488_set_orientation(CONFIG_LV_DISPLAY_ORIENTATION);
}

// Flush function based on mvturnho repo
void ili9488_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{
    uint32_t size = lv_area_get_width(area) * lv_area_get_height(area);

    lv_color16_t *buffer_16bit = (lv_color16_t *) color_map;
    uint8_t *mybuf;
    do {
        mybuf = (uint8_t *) heap_caps_malloc(3 * size * sizeof(uint8_t), MALLOC_CAP_DMA);
        if (mybuf == NULL)  ESP_LOGW(TAG, "Could not allocate enough DMA memory!");
    } while (mybuf == NULL);

    uint32_t LD = 0;
    uint32_t j = 0;

    for (uint32_t i = 0; i < size; i++) {
        LD = buffer_16bit[i].full;
        mybuf[j] = (uint8_t) (((LD & 0xF800) >> 8) | ((LD & 0x8000) >> 13));
        j++;
        mybuf[j] = (uint8_t) ((LD & 0x07E0) >> 3);
        j++;
        mybuf[j] = (uint8_t) (((LD & 0x001F) << 3) | ((LD & 0x0010) >> 2));
        j++;
    }

	/* Column addresses  */
	uint8_t xb[] = {
	    (uint8_t) (area->x1 >> 8) & 0xFF,
	    (uint8_t) (area->x1) & 0xFF,
	    (uint8_t) (area->x2 >> 8) & 0xFF,
	    (uint8_t) (area->x2) & 0xFF,
	};

	/* Page addresses  */
	uint8_t yb[] = {
	    (uint8_t) (area->y1 >> 8) & 0xFF,
	    (uint8_t) (area->y1) & 0xFF,
	    (uint8_t) (area->y2 >> 8) & 0xFF,
	    (uint8_t) (area->y2) & 0xFF,
	};

	/*Column addresses*/
	ili9488_send_cmd(ILI9488_CMD_COLUMN_ADDRESS_SET);
	ili9488_send_data(xb, 4);

	/*Page addresses*/
	ili9488_send_cmd(ILI9488_CMD_PAGE_ADDRESS_SET);
	ili9488_send_data(yb, 4);

	/*Memory write*/
	ili9488_send_cmd(ILI9488_CMD_MEMORY_WRITE);

	ili9488_send_color((void *) mybuf, size * 3);
	heap_caps_free(mybuf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void ili9488_send_cmd(uint8_t cmd)
{
    disp_wait_for_pending_transactions();
    gpio_set_level(PIN_NUM_SCREEN_DC, 0);	 /*Command mode*/
    disp_spi_send_data(&cmd, 1);
}

static void ili9488_send_data(void * data, uint16_t length)
{
    disp_wait_for_pending_transactions();
    gpio_set_level(PIN_NUM_SCREEN_DC, 1);	 /*Data mode*/
    disp_spi_send_data(data, length);
}

static void ili9488_send_color(void * data, uint16_t length)
{
    disp_wait_for_pending_transactions();
    gpio_set_level(PIN_NUM_SCREEN_DC, 1);   /*Data mode*/
    disp_spi_send_colors(data, length);
}

void disp_wait_for_pending_transactions() {
    // For blocking SPI transmits, no pending transactions to wait for.
}

void disp_spi_send_data(const void *data, size_t length) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = length * 8; // bits
    t.tx_buffer = data;
    esp_err_t ret = spi_device_transmit(ili9488_spi, &t);
    assert(ret == ESP_OK);
}

void disp_spi_send_colors(const void *data, size_t length) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = length * 8; // bits
    t.tx_buffer = data;
    esp_err_t ret = spi_device_transmit(ili9488_spi, &t);
    assert(ret == ESP_OK);
}

static void ili9488_set_orientation(uint8_t orientation)
{
    // ESP_ASSERT(orientation < 4);

    const char *orientation_str[] = {
        "PORTRAIT", "PORTRAIT_INVERTED", "LANDSCAPE", "LANDSCAPE_INVERTED"
    };

    ESP_LOGI(TAG, "Display orientation: %s", orientation_str[orientation]);

#if defined (CONFIG_LV_PREDEFINED_DISPLAY_NONE)
    uint8_t data[] = {0x48, 0x88, 0x28, 0xE8};
#endif

    ESP_LOGI(TAG, "0x36 command value: 0x%02X", data[orientation]);

    ili9488_send_cmd(0x36);
    ili9488_send_data((void *) &data[orientation], 1);
}