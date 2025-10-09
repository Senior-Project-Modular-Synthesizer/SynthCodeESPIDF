#pragma once

#include "driver/spi_master.h"

#include <array>
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "Processor.hpp"


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
        
        // Below this line is only for development/debugging
        void simple_loop( void* pvParameters );
        //void input_task();
        //void output_task();

    private:
        // Esp Idf SPI
        spi_device_handle_t spi;
        i2s_chan_handle_t rx_chan;
        i2s_chan_handle_t tx_chan;

        uint16_t writeRegister(uint8_t reg, uint8_t data);
        uint16_t readRegister(uint8_t reg);
        void update_cache();
        uint16_t write_setting(uint8_t reg, uint8_t value, uint8_t width, uint8_t pos);
        void configure_codec();
        void init_i2s();
        void init_spi();

        std::array<uint8_t, 21> registerCache;

        // Below this line is only for development/debugging
        uint8_t *r_buf1;
        volatile bool r_buf1_ready = false;
        uint8_t *r_buf2;
        volatile bool r_buf2_ready = false;
        uint8_t *t_buf1;
        uint8_t *t_buf2;
};
