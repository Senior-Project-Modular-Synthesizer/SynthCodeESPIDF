#pragma once

#include "driver/spi_master.h"

#include <array>
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "Processor.hpp"

class AK4619VN_input : public QuadInputBuffer {
    private:
        uint8_t buf1[3 * SAMPLE_COUNT * 4]; // * 4 - quad channel 
                                            // * 3 - 24 bit integers
        uint8_t buf2[3 * SAMPLE_COUNT * 4];

        //size_t capacity = 1024;
        //size_t head = 0;
        //size_t tail = 0;
    public:
        /*
        * Returns the number of samples the block can buffer.
        *
        * - The returned value is always a power of 2.
        * - This represents the number of leeway (each containing 4 samples, one per channel).
        */
        int size();

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
        bool errored() const;
};