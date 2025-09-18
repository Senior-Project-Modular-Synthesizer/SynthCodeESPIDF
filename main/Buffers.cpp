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

#include "Buffers.hpp"
#include "AK4619VN.hpp"

// TODO: DECIDE MAX TIMEOUT/BLOCK TIME WHEN READING/WRITING
//       IN START METHODS

void SampleInputBuffer::simple_loop( void* pvParameters ) {

}

/*
* Returns the number of samples in the current block.
* 
* - The returned value is always a power of 2.
* - This represents the number of frames (each containing 4 samples, one per channel).\
* - This is useful if the processor buffers a block and takes a while to process
* - It allows us to not skip over any samples as long as each sample is on average processed in less time than the sample rate
*/
int SampleInputBuffer::size() const {
    return SAMPLE_COUNT;
}

/*
* Retrieves a sample from the buffer.
* 
* - Samples are in floating-point format in the range [-1.0, 1.0]
*/
QuadSample SampleInputBuffer::nextSample(const QuadSample& next) {
    return QuadSample();
}

/*
* Retrieves an int sample from the buffer.
* 
* Sample format is unspecified because it is up to the actual chip
* As such, this format has no garunteed bit width beacuse it is up to the actual chip
*/
QuadIntSample SampleInputBuffer::nextIntSample() {
    return QuadIntSample();
}

/*
* Starts the buffer.
* This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
* It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
*/
void SampleInputBuffer::start() {
    // esp_err_t ret;  
    // #define BUF_SIZE (SAMPLE_COUNT * 3)
    // *buf1 = (uint8_t *)calloc(1, BUF_SIZE);
    // assert(buf1); // Check for buf1 allocation
    // size_t buf1_bytes = 0;
    
 
    xTaskCreatePinnedToCore( simple_loop_wrapper,
        "ReadIntoBuf",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL,
        0);
}

static void simple_loop_wrapper(void* pvParameters) {
    SampleInputBuffer* buf = static_cast<SampleInputBuffer*>(pvParameters);
    buf->simple_loop(pvParameters);
}

/*
* Stops the buffer and whatever tasks it's running.
*/
void SampleInputBuffer::stop() {
    
}

/*
* Returns true if the buffer ran into an unrecoverable error and must be restarted.
*/
bool SampleInputBuffer::errored() const {
    return false;
}


/*
* Returns the number of samples the block can buffer.
*
* - The returned value is always a power of 2.
* - This represents the number of leeway (each containing 4 samples, one per channel).
*/
int SampleOutputBuffer::size() const {
    return SAMPLE_COUNT;
}

/*
* Pushes a sample to the buffer.
* 
* - Samples are in floating-point format in the range [-1.0, 1.0]
* - This function does not perform bounds checking.
* - This function may block if the buffer is full.
*/
void SampleOutputBuffer::pushSample(QuadSample sample) {
    
}

/*
* Pushes an int sample to the buffer.
* 
* Sample format is unspecified because it is up to the actual chip
* As such, this format has no garunteed bit width beacuse it is up to the actual chip
*/
void SampleOutputBuffer::pushIntSample(QuadIntSample sample) {
    
}

/*
* Starts the buffer.
* This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
* It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
*/
void SampleOutputBuffer::start() {
    
}

/*
* Stops the buffer and whatever tasks it's running.
*/
void SampleOutputBuffer::stop() {
    
}

/*
* Returns true if the buffer ran into an unrecoverable error and must be restarted.
*/
bool SampleOutputBuffer::errored() const {
    return false;
}