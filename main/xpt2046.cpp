#include "xpt2046.hpp"

esp_err_t XPT2046::init() {
    spi_device_interface_config_t devcfg;
    //     .clock_speed_hz = spi_clock_hz_,
    //     .mode = 0,
    //     .spics_io_num = cs_pin_,
    //     .queue_size = 1,
    // };
    memset(&devcfg, 0, sizeof(devcfg));
    devcfg.clock_speed_hz = spi_clock_hz_;
    devcfg.mode = 0;
    devcfg.spics_io_num = cs_pin_;
    devcfg.queue_size = 1;

    esp_err_t ret = spi_bus_add_device(host_, &devcfg, &handle_);
    if (ret == ESP_OK)
        ESP_LOGI(TAG, "XPT2046 device added to SPI bus");
    else
        ESP_LOGE(TAG, "Failed to add XPT2046: %s", esp_err_to_name(ret));

    return ret;
}

uint16_t XPT2046::readValue(uint8_t command) {
    uint8_t tx_data[3] = { command, 0x00, 0x00 };
    uint8_t rx_data[3] = { 0 };

    spi_transaction_t t = {
        .length = 8 * sizeof(tx_data),
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };

    if (handle_ == nullptr) {
        ESP_LOGE(TAG, "Invalid device handle in readValue");
        return 0;
    }

    esp_err_t r = spi_device_transmit(handle_, &t);
    if (r != ESP_OK) {
        ESP_LOGE(TAG, "spi_device_transmit failed: %s", esp_err_to_name(r));
        return 0;
    }

    uint16_t value = ((rx_data[1] << 8) | rx_data[2]) >> 3;
    return value & 0x0FFF;
}

XPT2046::TouchData XPT2046::readTouch() {
    TouchData data;
    uint16_t z1 = readValue(CMD_Z1);
    uint16_t z2 = readValue(CMD_Z2);
    data.x = readValue(CMD_X);
    data.y = readValue(CMD_Y);
    data.z = z1 == 0 ? 0 : data.x * z2 / z1;

    return data;
}
