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

class AK4619VN : public QuadInputBuffer {
    private:
        uint8_t buf1[3 * SAMPLE_COUNT * 4]; // * 4 - quad channel 
                                            // * 3 - 24 bit integers
        uint8_t buf2[3 * SAMPLE_COUNT * 4];

        size_t capacity = 1024;
        size_t head = 0;
        size_t tail = 0;

    public:
        AK4619VN();
        ~AK4619VN();
        
        // Below this line is only for development/debugging
        void simple_loop();
        void input_task();
        void output_task();

        // QuadInputBuffer

        /*
        * Returns the number of samples the block can buffer.
        *
        * - The returned value is always a power of 2.
        * - This represents the number of leeway (each containing 4 samples, one per channel).
        */
        int size() const;

        /*
        * Pushes a sample to the buffer.
        * 
        * - Samples are in floating-point format in the range [-1.0, 1.0]
        * - This function does not perform bounds checking.
        * - This function may block if the buffer is full.
        */
        QuadSample nextSample(const QuadSample& next);

        /*
        * Pushes an int sample to the buffer.
        * 
        * Sample format is unspecified because it is up to the actual chip
        * As such, this format has no garunteed bit width beacuse it is up to the actual chip
        */
        QuadIntSample nextIntSample();

        /*
        * Starts the buffer.
        * This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
        * It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
        */
        void start();

        /*
        * Stops the buffer and whatever tasks it's running.
        */
        void stop();

        /*
        * Returns true if the buffer ran into an unrecoverable error and must be restarted.
        */
        bool errored();

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
