#include "AK4619VN_buffers.hpp"

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

static SemaphoreHandle_t bufReady = NULL;

volatile int bufFlag;


/*
* Listener task
*/
static void interruptTriggerTask( void * pvParameters )
{

}


/*
* Function that handles buffer writing within a semaphore.
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
        if (!bufFlag) {
            // process_buf(buf1)
        } else {
            // process_buf(buf2)
        }
    }

    vTaskDelete( NULL );
}

/*
* FreeRTOS start protocol -- starts the buffers, controls them with Task assignment and semaphores.
*/
void start() {
    // Create semaphores to sync DMA completion
    bufReady = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore( 
        bufFunction,
        "ReadIntoBuf",
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
* FreeRTOS stop protocol -- stops the buffers and tasks used to control them.
*/
void stop() {
    

}