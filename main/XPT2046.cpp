#include "XPT4026.hpp"
#include "driver/spi_master.h"
#include "peripheral_cfg_prototype.h"

XPT2046::XPT2046() {

}

XPT2046::XPT2046(uint8_t cs, uint8_t irq) {
    _cs = cs;
    _irq = irq;

    spi_slave_interface_config_t config = {
        .spics_io_num =
    };
}

XPT2046::XPT2046(uint8_t cs) {
    _cs = cs;
}

void XPT2046::begin() {

    spi_device_interface_config_t devcfg = {
        .mode = 0,                 // SPI mode 0
        .clock_speed_hz = 10000, // 10 kHz
        .spics_io_num = PIN_NUM_CODEC_CS,
        .queue_size = 7,
    };
}

uint8_t XPT2046::getInput() {
    // AK4619VN SPI read format: [CMD][ADDR_HIGH][ADDR_LOW] -> [DATA]
    // CMD=0x43, ADDR_HIGH=0x00, ADDR_LOW=register_address
    // Need 4-byte transaction due to SPI shift register behavior
    uint8_t tx_data[4] = {READ_COMMAND_CODE, 0x00, reg, 0x00};
    uint8_t rx_data[4] = {0};

    spi_transaction_t trans = {
        .length = 16,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };

    esp_err_t ret = spi_bus_add_device(SPI_HOST, &devcfg, &spi);

    ret = spi_device_transmit(this->spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI write failed for register 0x%02X: %s", reg, esp_err_to_name(ret));
        return 0xFFFF; // Return error value
    }

    return 0;
}

void XPT2046::setCallback(void (*cb)()) {
    irq_callback = cb;
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