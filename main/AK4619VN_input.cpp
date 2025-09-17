#include "AK4619VN_input.hpp"

#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "freertos/semphr.h"
#include "esp_check.h"

#include "driver/spi_master.h"
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "AK4619VN_constants.h"
#include "esp_log.h"
#include "math.h"

#define STREAM_BUFFER_LENGTH_BYTES      ( (size_t) SAMPLE_COUNT * 3 * 4) // Divided by 8?
#define STREAM_BUFFER_TRIGGER_LEVEL     ( (BaseType_t) 10)

static StreamBufferHandle_t xAK4619VN_input = NULL;

static SemaphoreHandle_t buf1ready, 
                         buf2ready          = NULL;

static SemaphoreHandle_t bufReady = NULL;
volatile int bufFlag;


/*
* Listener task
*/
static void interruptTriggerTask( void * pvParameters )
{

}


/*
* Function that handles 
*/
static void bufFunction( void * pvParameters ) 
{
    for( ;; )
    {
        // start read buf1
        // dma_read(buf1)
        
        // Wait for DMA to interrupt (buf1 ready)
        xSemaphoreTake(bufReady, portMAX_DELAY);
        
        // 0 = buf1, 1 = buf2
        // can cast like true or false in C++
        if (!bufFlag) {
            // process_buf(buf1)
        } else {
            // process_buf(buf2)
        }
    }

    vTaskDelete( NULL );
}

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
    xAK4619VN_input = xStreamBufferCreate(
        STREAM_BUFFER_LENGTH_BYTES,
        STREAM_BUFFER_TRIGGER_LEVEL
    );

    /* 
    if (currentBuffer == NULL || currentIndex >= BUFFER_LEN)
        // Signal from ISR
        xSemaphoreTake(bufReady, portMAX_DELAY)

        if (!bufFlag) {
            currentBuffer = buf1;
        } else if (readyFlag == 2) {
            currentBuffer = buf2; 
        }

        currentIndex = 0;
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
    // Create semaphores to sync DMA completion
    buf1ready = xSemaphoreCreateBinary();
    buf2ready = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore( 
        bufFunction,
        "ReadIntoBuf1",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL,
        0
    );
    
    // (If needed) Interrupt Task
    xTaskCreatePinnedToCore(
        interruptTriggerTask,
        "TrigInterr",
        configMINIMAL_STACK_SIZE,
        NULL,
        configMAX_PRIORITIES - 1,
        NULL,
        0
    );

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