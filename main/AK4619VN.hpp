#pragma once

#include "driver/spi_master.h"

#include "peripheral_cfg.h"
#include "driver/i2s_tdm.h"

class AK4619VN {
    public:
        AK4619VN();
        ~AK4619VN();
    private:
        // Esp Idf SPI
        spi_device_handle_t spi;
        i2s_chan_handle_t tx_chan;
        i2s_chan_handle_t rx_chan;

        void writeRegister(uint8_t reg, uint8_t data);
        uint8_t readRegister(uint8_t reg);
};