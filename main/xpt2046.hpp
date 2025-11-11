#pragma once
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include <cstdint>

class XPT2046 {
public:
    struct TouchData {
        uint16_t x;
        uint16_t y;
        uint16_t z;
    };

    XPT2046(spi_host_device_t host,
            gpio_num_t cs_pin,
            int spi_clock_hz = 2000000)
        : host_(host),
          cs_pin_(cs_pin),
          spi_clock_hz_(spi_clock_hz),
          handle_(nullptr) {}

    esp_err_t init();
    TouchData readTouch();

private:
    static constexpr const char* TAG = "XPT2046";

    static constexpr uint8_t CMD_X  = 0xD0; // 1101 0000
    static constexpr uint8_t CMD_Y  = 0x90; // 1001 0000
    static constexpr uint8_t CMD_Z1 = 0xB0; // 1011 0000
    static constexpr uint8_t CMD_Z2 = 0xC0; // 1100 0000

    spi_host_device_t host_;
    gpio_num_t cs_pin_;
    int spi_clock_hz_;
    spi_device_handle_t handle_;

    uint16_t readValue(uint8_t command);
};
    