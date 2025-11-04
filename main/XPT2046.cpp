#include "XPT2046.hpp"
#include "driver/spi_master.h"
#include "peripheral_cfg.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

/*  XPT2046 touchscreen driver
    Adapted from: https://github.com/bonezegei/Bonezegei_XPT2046/blob/main/src/Bonezegei_XPT2046.cpp
*/

#define TAG "XPT2046"

XPT2046::XPT2046() {
    _cs = PIN_NUM_TOUCH_CS;
    _irq = 0xFF;
    spi = nullptr;
}

XPT2046::XPT2046(uint8_t cs, uint8_t irq) {
    _cs = cs;
    _irq = irq;

    spi = nullptr;
    this->devcfg = {
        .mode = 0,                 // SPI mode 0
        .clock_speed_hz = 1'000'000, // 1 MHz
        .spics_io_num = _cs,
        .queue_size = 7,
    };
}

XPT2046::XPT2046(uint8_t cs) {
    _cs = cs;
    _irq = 0xFF;
    spi = nullptr;
}

uint16_t spi_send_16(spi_device_handle_t spi, uint16_t bytes) {
    uint8_t tx_buf[2];
    uint8_t rx_buf[2];

    // Send MSB first
    tx_buf[0] = (uint8_t)((bytes >> 8) & 0xFF);
    tx_buf[1] = (uint8_t)(bytes & 0xFF);

    memset(&rx_buf, 0, sizeof(rx_buf));

    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    trans.length = 16; // bits
    trans.tx_buffer = tx_buf;
    trans.rx_buffer = rx_buf;

    esp_err_t ret = spi_device_polling_transmit(spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI transfer failed for value (0x%04X): %s", bytes, esp_err_to_name(ret));
        return 0xFFFF;
    }

    // Reconstruct received 16-bit value (MSB first)
    uint16_t recv = ((uint16_t)rx_buf[0] << 8) | (uint16_t)rx_buf[1];
    return recv;
}

// Read 12-bit value from XPT2046 using a single 24-bit transaction:
// Send: [CMD][0x00][0x00]
// Read:  rx[0] (echo), rx[1] (MSB part), rx[2] (LSB part)
// ADC result is 12-bit MSB-aligned within the 16 read bits -> ((rx1<<8)|rx2) >> 3
static uint16_t xpt_read12(spi_device_handle_t spi, uint8_t cmd) {
    if (!spi) return 0xFFFF;

    uint8_t tx_buf[3];
    uint8_t rx_buf[3];
    tx_buf[0] = cmd;
    tx_buf[1] = 0x00;
    tx_buf[2] = 0x00;
    memset(rx_buf, 0, sizeof(rx_buf));

    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    trans.length = 24; // bits
    trans.tx_buffer = tx_buf;
    trans.rx_buffer = rx_buf;

    esp_err_t ret = spi_device_polling_transmit(spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "xpt_read12: SPI transfer failed for cmd 0x%02X: %s", cmd, esp_err_to_name(ret));
        return 0xFFFF;
    }

    // Combine rx[1] and rx[2] to a 16-bit word then shift to get 12-bit value.
    uint16_t raw = ((uint16_t)rx_buf[1] << 8) | (uint16_t)rx_buf[2];
    uint16_t value = (uint16_t)((raw >> 3) & 0x0FFF);
    ESP_LOGD(TAG, "xpt_read12 cmd=0x%02X rx={0x%02X,0x%02X,0x%02X} raw=0x%04X val=%u", cmd, rx_buf[0], rx_buf[1], rx_buf[2], raw, value);
    return value;
}

void XPT2046::begin() {
    devcfg = {
        .mode = 0,                 // SPI mode 0
        .clock_speed_hz = 1'000'000, // 1 MHz
        .spics_io_num = _cs,
        .queue_size = 7,
    };
    esp_err_t ret = spi_bus_add_device(SPI_HOST, &devcfg, &spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device (CS=%d): %s", (int)_cs, esp_err_to_name(ret));
        spi = nullptr;
        return;
    }
    ESP_LOGI(TAG, "XPT2046 SPI device added (CS=%d)", (int)_cs);
}
/*
    TODO: Check for endianness
    If it's LSB first,
    byte = (byte >> 8) | (byte << 8);
*/
uint8_t XPT2046::getInput() {
    if (spi == nullptr) {
        ESP_LOGE(TAG, "SPI device not initialized (call begin()).");
        return 0;
    }
    // AK4619VN SPI read format: [CMD][ADDR_HIGH][ADDR_LOW] -> [DATA]
    // CMD=0x43, ADDR_HIGH=0x00, ADDR_LOW=register_address
    // Need 4-byte transaction due to SPI shift register behavior
    //uint8_t tx_data[4] = {READ_COMMAND_CODE, 0x00, reg, 0x00};
    //uint8_t rx_data[4] = {0};

    // spi_transaction_t trans = {
    //     .length = 1,
    //     .tx_buffer = tx_data,
    //     .rx_buffer = rx_data
    // };
    uint16_t z1 = xpt_read12(spi, 0xC1);
    if (z1 == 0xFFFF) { ESP_LOGE(TAG, "SPI error reading Z1"); return 0; }

    uint16_t z2 = xpt_read12(spi, 0x91);
    if (z2 == 0xFFFF) { ESP_LOGE(TAG, "SPI error reading Z2"); return 0; }

    uint16_t xv = xpt_read12(spi, 0xD0);
    if (xv == 0xFFFF) { ESP_LOGE(TAG, "SPI error reading X"); return 0; }
    x = xv;

    uint16_t yv = xpt_read12(spi, 0x00);
    if (yv == 0xFFFF) { ESP_LOGE(TAG, "SPI error reading Y"); return 0; }
    y = yv;

    z = (uint16_t)(z1 + 4095 - z2);
    ESP_LOGI(TAG, "X: %u, Y: %u, Z: %u", (unsigned)x, (unsigned)y, (unsigned)z);

    return (z > 500) ? 1 : 0;
}

void XPT2046::setCallback(void (*cb)()) {
    irq_callback = cb;
}

XPT2046::~XPT2046() {
    if (spi) {
        esp_err_t ret = spi_bus_remove_device(spi);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to remove SPI device: %s", esp_err_to_name(ret));
        }
        spi = nullptr;
    }
}

/*
DEBUG_CHECKPOINT("Starting SPI initialization");
    
    esp_err_t ret;
    spi_device_interface_config_t devcfg = {
        .mode = 0,                 // SPI mode 0
        .clock_speed_hz = 10000, // 10 kHz
        .spics_io_num = PIN_NUM_CODEC_CS,
        .queue_size = 7,
    };

    DEBUG_LOG("Adding SPI device to bus");
    ret = spi_bus_add_device(SPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    DEBUG_LOG("Sending SPI dummy command sequence");
    // Send dummy command to enable SPI interface mode
    // Sequence: 0xDE → 0xADDA → 0x7A (MSB first)
    uint8_t dummy_cmd[4] = {0xDE, 0xAD, 0xDA, 0x7A};
    spi_transaction_t dummy_trans = {
        .length = 32,  // 4 bytes * 8 bits
        .tx_buffer = dummy_cmd,
        .rx_buffer = NULL
    };
*/