#pragma once

#include "driver/spi_master.h"

#include <array>
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"


enum class ADCDigitalFilterType {
    SharpRollOff = 0b000,
    SlowRollOff = 0b001,
    ShortDelaySharpRollOff = 0b010,
    ShortDelaySlowRollOff = 0b011,
    Voice = 0b100
};

enum class DACDigitalFilterType {
    SharpRollOff = 0b00,
    SlowRollOff = 0b01,
    ShortDelaySharpRollOff = 0b10,
    ShortDelaySlowRollOff = 0b11
};

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

        std::array<uint8_t, 21> registerCache;
};
