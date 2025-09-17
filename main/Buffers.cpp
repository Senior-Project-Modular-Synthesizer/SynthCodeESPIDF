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