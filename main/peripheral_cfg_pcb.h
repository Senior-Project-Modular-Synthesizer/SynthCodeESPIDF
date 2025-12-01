#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"

#define SPI_HOST SPI2_HOST

// Wires for SPI
#define PIN_NUM_MOSI GPIO_NUM_6
#define PIN_NUM_MISO GPIO_NUM_7
#define PIN_NUM_SCLK GPIO_NUM_5

// Wires for CODEC
#define PIN_NUM_CODEC_CS GPIO_NUM_14
#define PIN_NUM_CODEC_RX GPIO_NUM_13 // I2S Input (CODEC to ESP32)
#define PIN_NUM_CODEC_TX GPIO_NUM_12 // I2S Output (ESP32 to CODEC)
#define PIN_NUM_CODEC_WS GPIO_NUM_11  // I2S Word select
#define PIN_NUM_CODEC_BCLK GPIO_NUM_10 // I2S Bit Clock
#define PIN_NUM_CODEC_MCLK GPIO_NUM_9 // Master Clock
#define PIN_NUM_CODEC_PDN GPIO_NUM_21 // Power Down

#define FREQ_96KHZ 0 // Select between 48kHz and 96kHz

#define I2S_SAMPLE_RATE 48000

// Wires for Screen
#define PIN_NUM_SCREEN_CS GPIO_NUM_15
#define PIN_NUM_SCREEN_DC GPIO_NUM_16  // Data/Command
#define PIN_NUM_SCREEN_RST GPIO_NUM_4 // Reset
#define PIN_NUM_TOUCH_CS GPIO_NUM_2

#define SPI_TFT_CLOCK_SPEED_HZ  (40*1000*1000)
#define SPI_TFT_SPI_MODE 0

// Screen defs
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
//but less overhead for setting up / finishing transfers. Make sure SCREEN_WIDTH is dividable by this.
#define PARALLEL_LINES 96
// #define PARALLEL_LINES 1

const spi_bus_config_t SPI_BUS_CFG = {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = PIN_NUM_MISO,
    .sclk_io_num = PIN_NUM_SCLK,
    .quadwp_io_num = GPIO_NUM_NC,
    .quadhd_io_num = GPIO_NUM_NC,
    .data4_io_num = GPIO_NUM_NC,
    .data5_io_num = GPIO_NUM_NC,
    .data6_io_num = GPIO_NUM_NC,
    .data7_io_num = GPIO_NUM_NC,
    .max_transfer_sz = PARALLEL_LINES * SCREEN_HEIGHT * 2 + 8,
    .flags = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MISO |
                SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MASTER,
    .intr_flags = ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM
};