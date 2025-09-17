#include "AK4619VN_input.hpp"

#include "driver/gpio.h"
#include "freertos/task.h"
#include "esp_check.h"

#include "driver/spi_master.h"
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "AK4619VN_constants.h"
#include "esp_log.h"
#include "math.h"

#define STREAM_BUFFER_LENGTH_BYTES      ( (size_t) SAMPLE_COUNT )

// Core 0 for all buffering
/*
* Returns the number of samples the block can buffer.
*
* - The returned value is always a power of 2.
* - This represents the number of leeway (each containing 4 samples, one per channel).
*/
int AK4619VN_input::size() {
    return SAMPLE_COUNT;
}

/*
* Pushes a sample to the buffer.
* 
* - Samples are in floating-point format in the range [-1.0, 1.0]
* - This function does not perform bounds checking.
* - This function may block if the buffer is full.
*/
QuadSample AK4619VN_input::nextSample(const QuadSample& next) {
    /*
    task () {
        while(1) {
            start read buf1;
            wait buf1 interrupt;
            mark buf1 ready;
            repeat for buf2;
        }
    }
    Next sample:
    if reading buf1 {
        wait buf1 ready;
        return buf1[next sample]
    } else {
        repeat for buf2;
    }
    */
    return QuadSample();
}

/*
* Pushes an int sample to the buffer.
* 
* Sample format is unspecified because it is up to the actual chip
* As such, this format has no garunteed bit width beacuse it is up to the actual chip
*/
QuadIntSample AK4619VN_input::nextIntSample() {
    return QuadIntSample();
}

/*
* Starts the buffer.
* This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
* It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
*/
void AK4619VN_input::start() {
    // Free RTOS start protocol
    xAK4619VN_input = xStreamBufferCreate(
        
    )

}

/*
* Stops the buffer and whatever tasks it's running.
*/
void AK4619VN_input::stop() {
    // Free RTOS stop protocol
}

/*
* Returns true if the buffer ran into an unrecoverable error and must be restarted.
*/
bool AK4619VN_input::errored() const {
    // try-catch (?)
    // Catching race conditions (?)
    return false;
}