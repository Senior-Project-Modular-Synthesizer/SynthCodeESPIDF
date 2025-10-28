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

#define BUF_SIZE (3 * 4 * SAMPLE_COUNT)

class SampleInputBuffer : public QuadInputBuffer {
    private:
        i2s_chan_handle_t rx_chan;
        int read_ptr = 0;

        TaskHandle_t writeBuf_handle;

        EventGroupHandle_t xHandle;
        EventBits_t uxBits;

    public:
        SampleInputBuffer(i2s_chan_handle_t rx_chan);
        ~SampleInputBuffer();

        void read ();


        uint8_t* buf1; // * 4 - quad channel 
                                            // * 3 - 24 bit integers
        uint8_t* buf2;

        void default_i2s_stream_init();

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
        QuadSample nextSample();

        /*
        * Retrieves an int sample from the buffer.
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


class SampleOutputBuffer : public QuadOutputBuffer {
    private:
        i2s_chan_handle_t tx_chan;
        int read_ptr = 0;

        TaskHandle_t writeBuf_handle;

        EventGroupHandle_t xHandle;
        EventBits_t uxBits;

    public:
        SampleOutputBuffer(i2s_chan_handle_t tx_chan);
        ~SampleOutputBuffer();

        void write ();

        uint8_t* buf1; // * 4 - quad channel 
                       // * 3 - 24 bit integers
        uint8_t* buf2;

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
        void pushSample(QuadSample sample);

        /*
        * Pushes an int sample to the buffer.
        * 
        * Sample format is unspecified because it is up to the actual chip
        * As such, this format has no garunteed bit width beacuse it is up to the actual chip
        */
        void pushIntSample(QuadIntSample sample);
        
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