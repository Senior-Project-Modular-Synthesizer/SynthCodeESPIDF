#pragma once

#define USE_PCB 0

#if USE_PCB
    #include "peripheral_cfg_pcb.h"
#else
    #include "peripheral_cfg_prototype.h"
#endif

// Wires for Screen
#define PIN_NUM_SCREEN_CS GPIO_NUM_16
#define PIN_NUM_SCREEN_DC GPIO_NUM_7  // Data/Command
#define PIN_NUM_SCREEN_RST GPIO_NUM_15 // Reset

#define SPI_TFT_CLOCK_SPEED_HZ  (40*1000*1000)

// Screen defs
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
//but less overhead for setting up / finishing transfers. Make sure SCREEN_WIDTH is dividable by this.
#define PARALLEL_LINES 16


// Buffer Waiting Bits
#define INPUT_BUF1_READY = (1 << 0)
#define INPUT_BUF1_WAIT  = (1 << 1)
#define INPUT_BUF2_READY = (1 << 2)
#define INPUT_BUF2_WAIT  = (1 << 3)
#define OUTPUT_BUF1_READY = (1 << 4)
#define OUTPUT_BUF1_WAIT  = (1 << 5)
#define OUTPUT_BUF2_READY = (1 << 6)
#define OUTPUT_BUF2_WAIT  = (1 << 7)


const spi_bus_config_t SPI_BUS_CFG = {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = PIN_NUM_MISO,
    .sclk_io_num = PIN_NUM_SCLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = PARALLEL_LINES * SCREEN_HEIGHT * 2 + 8
};