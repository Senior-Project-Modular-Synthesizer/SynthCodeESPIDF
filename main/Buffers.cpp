#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "freertos/semphr.h"
#include "esp_check.h"
#include "esp_log.h"

#include <array>
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "Processor.hpp"

#include "Buffers.hpp"
#include "AK4619VN.hpp"
#include "math.h"

#define SLEEP_MS 10

#define WAIT_FOR_READ 1
#define WAIT_FOR_WRITE 1

bool QuadInputBuffer::errored() const {
    return false;
}


SampleInputBuffer::SampleInputBuffer(i2s_chan_handle_t rx_chan) {
    this->rx_chan = rx_chan;
    this->xHandle = xEventGroupCreate();

    buf1 = (uint8_t*)heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    buf2 = (uint8_t*)heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);

    xEventGroupClearBits(xHandle, INPUT_BUF1_READY | INPUT_BUF2_READY);
    xEventGroupSetBits(xHandle, INPUT_BUF1_WAIT | INPUT_BUF2_WAIT);
}

SampleInputBuffer::~SampleInputBuffer() {
    if (buf1) heap_caps_free(buf1);
    if (buf2) heap_caps_free(buf2);
    buf1 = nullptr;
    buf2 = nullptr;
    if (xHandle) vEventGroupDelete(xHandle);
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
QuadSample SampleInputBuffer::nextSample() {
    QuadSample next_sample = {
        .channels = {0.0f, 0.0f, 0.0f, 0.0f}
    };
    // Disable floating point for now
    return next_sample;
}

/*
* Retrieves an int sample from the buffer.
* 
* Sample format is unspecified because it is up to the actual chip
* As such, this format has no guaranteed bit width beacuse it is up to the actual chip
*/
QuadIntSample SampleInputBuffer::nextIntSample() {
    QuadIntSample next_sample = {
        .channels = {0, 0, 0, 0}
    };
    if (this->read_ptr < SAMPLE_COUNT) {
        //while (!this->buf1_ready)
        //    vTaskDelay(pdMS_TO_TICKS(SLEEP_MS));
        xEventGroupWaitBits(xHandle, INPUT_BUF1_READY, pdFALSE, pdTRUE, portMAX_DELAY);
        for (int i = 0; i < 4; i++) {
            uint32_t unsigned_sample = 
            (buf1[this->read_ptr * 12 + i * 3 + 0] << 16) |
            (buf1[this->read_ptr * 12 + i * 3 + 1] << 8)  |
            (buf1[this->read_ptr * 12 + i * 3 + 2]);
            uint32_t signed_sample = unsigned_sample;
            if (unsigned_sample & 0x800000) { // If sign bit is set
                signed_sample |= 0xFF000000; // Sign extend to 32 bits
            }
            next_sample.channels[i] = signed_sample;
        }

        this->read_ptr += 1;

        if (this->read_ptr >= SAMPLE_COUNT) {
            xEventGroupClearBits(xHandle, INPUT_BUF1_READY);
#if WAIT_FOR_READ
            xEventGroupSetBits(xHandle, INPUT_BUF1_WAIT);
#endif
        }

        return next_sample;
    } else {
        xEventGroupWaitBits(xHandle, INPUT_BUF2_READY, pdFALSE, pdTRUE, portMAX_DELAY);

        for (int i = 0; i < 4; i++) {
            uint32_t unsigned_sample = 
            (buf2[(this->read_ptr - SAMPLE_COUNT) * 12 + i * 3 + 0] << 16) +
            (buf2[(this->read_ptr - SAMPLE_COUNT) * 12 + i * 3 + 1] << 8)  +
            (buf2[(this->read_ptr - SAMPLE_COUNT) * 12 + i * 3 + 2]);
            uint32_t signed_sample = unsigned_sample;
            if (unsigned_sample & 0x800000) { // If sign bit is set
                signed_sample |= 0xFF000000; // Sign extend to 32 bits
            }
            next_sample.channels[i] = signed_sample;
        }

        this->read_ptr += 1;

        if (this->read_ptr >= (2 * SAMPLE_COUNT)) {
            this->read_ptr = 0;
            xEventGroupClearBits(xHandle, INPUT_BUF2_READY);
#if WAIT_FOR_READ
            xEventGroupSetBits(xHandle, INPUT_BUF2_WAIT);
#endif
        }

        return next_sample;
    }
}


void SampleInputBuffer::read( ) {
    int num_read = 0;
    esp_err_t ret;  
    size_t buf1_bytes_read = 0;
    size_t buf2_bytes_read = 0;
 
    // Input and print out
    for ( ;; ) {
        //while (this->buf1_ready)
        //    vTaskDelay(pdMS_TO_TICKS(SLEEP_MS));
#if WAIT_FOR_READ
        xEventGroupWaitBits(xHandle, INPUT_BUF1_WAIT, pdFALSE, pdTRUE, portMAX_DELAY);
#endif
        ret = i2s_channel_read(rx_chan, buf1, BUF_SIZE, &buf1_bytes_read, 1000);

        ESP_ERROR_CHECK(ret);
        //this->buf1_ready = true;
#if WAIT_FOR_READ
        xEventGroupClearBits(xHandle, INPUT_BUF1_WAIT);
#endif
        xEventGroupSetBits(xHandle, INPUT_BUF1_READY);
    
#if WAIT_FOR_READ
        xEventGroupWaitBits(xHandle, INPUT_BUF2_WAIT, pdFALSE, pdTRUE, portMAX_DELAY);
#endif

        ret = i2s_channel_read(rx_chan, buf2, BUF_SIZE, &buf2_bytes_read, 1000);
        ESP_ERROR_CHECK(ret);

        //this->buf2_ready = true;
#if WAIT_FOR_READ
        xEventGroupClearBits(xHandle, INPUT_BUF2_WAIT);
#endif
        xEventGroupSetBits(xHandle, INPUT_BUF2_READY);

    }
}

static void read_wrapper(void* pvParameters) {
    SampleInputBuffer* buf = static_cast<SampleInputBuffer*>(pvParameters);
    
    buf->read();
}

/*
* Starts the buffer.
* This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
* It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
*/
void SampleInputBuffer::start() {
    xTaskCreatePinnedToCore( read_wrapper,
        "ReadIntoBuf",
        configMINIMAL_STACK_SIZE + 4096,
        this,
        configMAX_PRIORITIES - 1,
        &writeBuf_handle,
        0);
}


/*
* Stops the buffer and whatever tasks it's running.
*/
void SampleInputBuffer::stop() {
    // Should this be a taskHandle_t?
    vTaskDelete(writeBuf_handle);
}

/*
* Returns true if the buffer ran into an unrecoverable error and must be restarted.
*/
bool SampleInputBuffer::errored() const {
    return false;
}


SampleOutputBuffer::SampleOutputBuffer(i2s_chan_handle_t tx_chan) {
    this->tx_chan = tx_chan;
    this->xHandle = xEventGroupCreate();

    buf1 = (uint8_t*)heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    buf2 = (uint8_t*)heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);    

    xEventGroupSetBits(xHandle, OUTPUT_BUF1_READY | OUTPUT_BUF2_READY);
    xEventGroupClearBits(xHandle, OUTPUT_BUF1_WAIT | OUTPUT_BUF2_WAIT);

    // if (xHandle != NULL) {
    //     printf("Create SUCCESS\n");
    //     xTaskCreate(vSetBitTask, "Set Bits", 2048, NULL, 1, NULL);
    // }
    // else {
    //     printf("Create FALSE\n");
    // }
}

SampleOutputBuffer::~SampleOutputBuffer() {
    if (buf1) heap_caps_free(buf1);
    if (buf2) heap_caps_free(buf2);
    buf1 = nullptr;
    buf2 = nullptr;
    if (xHandle) vEventGroupDelete(xHandle);
}


void SampleOutputBuffer::write ( ) {
    esp_err_t ret;
    size_t buf2_bytes_written;
    size_t buf1_bytes_written;

    for ( ;; ) {
        //while (!this->buf1_ready)
        //   vTaskDelay(pdMS_TO_TICKS(SLEEP_MS));
#if WAIT_FOR_WRITE
        xEventGroupWaitBits(xHandle, OUTPUT_BUF1_WAIT, pdFALSE, pdTRUE, portMAX_DELAY);
#endif
        // Send buffer to I2S
        size_t bytes_written;
        
        ret = i2s_channel_write(tx_chan, buf1, BUF_SIZE, &buf1_bytes_written, 1000);
        ESP_ERROR_CHECK(ret);

        //this->buf1_ready = false;
#if WAIT_FOR_WRITE
        xEventGroupClearBits(xHandle, OUTPUT_BUF1_WAIT);
#endif
        xEventGroupSetBits(xHandle, OUTPUT_BUF1_READY);

        // while (!this->buf2_ready)
        //     vTaskDelay(pdMS_TO_TICKS(SLEEP_MS));
#if WAIT_FOR_WRITE
        xEventGroupWaitBits(xHandle, OUTPUT_BUF2_WAIT, pdFALSE, pdTRUE, portMAX_DELAY);
#endif
        
        ret = i2s_channel_write(tx_chan, buf2, BUF_SIZE, &buf2_bytes_written, 1000);
        ESP_ERROR_CHECK(ret);

        //this->buf2_ready = false;
#if WAIT_FOR_WRITE
        xEventGroupClearBits(xHandle, OUTPUT_BUF2_WAIT);
#endif
        xEventGroupSetBits(xHandle, OUTPUT_BUF2_READY);
    }
}

        
static void write_wrapper ( void* pvParameters) {
    SampleOutputBuffer* buf = static_cast<SampleOutputBuffer*>(pvParameters);
    buf->write();
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
    return; // Disable floating point for now
}

/*
* Pushes an int sample to the buffer.
* 
* Sample format is unspecified because it is up to the actual chip
* As such, this format has no guaranteed bit width beacuse it is up to the actual chip
*/
void SampleOutputBuffer::pushIntSample(QuadIntSample sample) {
    // Commit guaranteeing comment
    if (this->read_ptr < SAMPLE_COUNT) {
        xEventGroupWaitBits(xHandle, OUTPUT_BUF1_READY, pdFALSE, pdTRUE, portMAX_DELAY);
        for (int i = 0; i < 4; i++) {
            uint32_t uint_sample = sample.channels[i];
            //         sample indexing       int indexing
            this->buf1[this->read_ptr * 12 + (i * 3) + 0] = (uint8_t) ((uint_sample >> 16) & 0xFF); // MSB
            this->buf1[this->read_ptr * 12 + (i * 3) + 1] = (uint8_t) ((uint_sample >> 8) & 0xFF);
            this->buf1[this->read_ptr * 12 + (i * 3) + 2] = (uint8_t) ((uint_sample) & 0xFF); // LSB
        }

        this->read_ptr += 1;

        if (this->read_ptr >= SAMPLE_COUNT) {
            //this->buf1_rebuf1ady = true;
            xEventGroupClearBits(xHandle, OUTPUT_BUF1_READY);
#if WAIT_FOR_WRITE
            xEventGroupSetBits(xHandle, OUTPUT_BUF1_WAIT);
#endif
        }

    } else {
        xEventGroupWaitBits(xHandle, OUTPUT_BUF2_READY, pdFALSE, pdTRUE, portMAX_DELAY);

        for (int i = 0; i < 4; i++) {
            uint32_t uint_sample_2 = sample.channels[i];
            //          sample indexing                    int indexing
            this->buf2[(this->read_ptr - SAMPLE_COUNT) * 12 + (i * 3) + 0] = (uint8_t) ((uint_sample_2 >> 16) & 0xFF); // MSB
            this->buf2[(this->read_ptr - SAMPLE_COUNT) * 12 + (i * 3) + 1] = (uint8_t) ((uint_sample_2 >> 8) & 0xFF);
            this->buf2[(this->read_ptr - SAMPLE_COUNT) * 12 + (i * 3) + 2] = (uint8_t) ((uint_sample_2) & 0xFF); // LSB
        }

        this->read_ptr += 1;

        if (this->read_ptr >= (2 * SAMPLE_COUNT)) {
            this->read_ptr = 0;
            //this->buf2_ready = true;
            xEventGroupClearBits(xHandle, OUTPUT_BUF2_READY);
#if WAIT_FOR_WRITE
            xEventGroupSetBits(xHandle, OUTPUT_BUF2_WAIT);
#endif
        }
    }
}

/*
* Starts the buffer.
* This is a non-blocking operation which allows the UI to continue running while the buffer fills itself.
* It will be called once so it is the responsibility of the implementation to ensure that the buffer is continued to be filled.
*/
void SampleOutputBuffer::start() {
    xTaskCreatePinnedToCore( write_wrapper,
        "ReadIntoBuf",
        configMINIMAL_STACK_SIZE,
        this,
        configMAX_PRIORITIES - 1,
        &writeBuf_handle,
        0);
}

/*
* Stops the buffer and whatever tasks it's running.
*/
void SampleOutputBuffer::stop() {
    vTaskDelete(writeBuf_handle);
}

/*
* Returns true if the buffer ran into an unrecoverable error and must be restarted.
*/
bool SampleOutputBuffer::errored() const {
    return false;
}