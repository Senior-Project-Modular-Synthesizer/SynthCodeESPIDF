#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "freertos/semphr.h"
#include "esp_check.h"

#include <array>
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "Processor.hpp"
#include "AK4619VN.hpp"

class SampleInputBuffer : public QuadInputBuffer {
    private:
        


    public:
        static void simple_loop_wrapper( void* pvParameters);

        void simple_loop ( void* pvParameters);


        uint8_t* buf1[3 * 4 * SAMPLE_COUNT]; // * 4 - quad channel 
                                            // * 3 - 24 bit integers

        /*
        * Returns the number of samples in the current block.
        * 
        * - The returned value is always a power of 2.
        * - This represents the number of frames (each containing 4 samples, one per channel).\
        * - This is useful if the processor buffers a block and takes a while to process
        * - It allows us to not skip over any samples as long as each sample is on average processed in less time than the sample rate
        */
        int size() const;
        
        /*
        * Retrieves a sample from the buffer.
        * 
        * - Samples are in floating-point format in the range [-1.0, 1.0]
        */
        virtual QuadSample nextSample(const QuadSample& next);

        /*
        * Retrieves an int sample from the buffer.
        * 
        * Sample format is unspecified because it is up to the actual chip
        * As such, this format has no garunteed bit width beacuse it is up to the actual chip
        */
        virtual QuadIntSample nextIntSample() = 0;

        /*
        * Starts the buffer.
        * This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
        * It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
        */
        virtual void start() = 0;

        /*
        * Stops the buffer and whatever tasks it's running.
        */
        virtual void stop() = 0;

        /*
        * Returns true if the buffer ran into an unrecoverable error and must be restarted.
        */
        virtual bool errored() const;
};


class SampleOutputBuffer : public QuadOutputBuffer {
    private:


    public:
        uint8_t buf2[3 * 4 * SAMPLE_COUNT]; // * 4 - quad channel 
                                            // * 3 - 24 bit integers

        /*
        * Returns the number of samples the block can buffer.
        *
        * - The returned value is always a power of 2.
        * - This represents the number of leeway (each containing 4 samples, one per channel).
        */
        virtual int size() const = 0;
        
        /*
        * Pushes a sample to the buffer.
        * 
        * - Samples are in floating-point format in the range [-1.0, 1.0]
        * - This function does not perform bounds checking.
        * - This function may block if the buffer is full.
        */
        virtual void pushSample(QuadSample sample) = 0;

        /*
        * Pushes an int sample to the buffer.
        * 
        * Sample format is unspecified because it is up to the actual chip
        * As such, this format has no garunteed bit width beacuse it is up to the actual chip
        */
        virtual void pushIntSample(QuadIntSample sample) = 0;
        
        /*
        * Starts the buffer.
        * This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
        * It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
        */
        virtual void start() = 0;

        /*
        * Stops the buffer and whatever tasks it's running.
        */
        virtual void stop() = 0;

        /*
        * Returns true if the buffer ran into an unrecoverable error and must be restarted.
        */
        virtual bool errored() const = 0;
};  