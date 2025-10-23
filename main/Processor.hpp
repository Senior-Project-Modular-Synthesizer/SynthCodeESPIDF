#pragma once

#include <cstdint>
#define SAMPLE_COUNT 64

/*
 * This file defines the core interfaces for a quadraphonic audio processor system.
 *
 * The system is designed to process four-channels of audio in real-time,
 * with separate input and output buffers. 
 * 
 * Key Design Considerations:
 * 
 * - Dual Buffering:
 *   - Both the input and output buffers manage their own storage independently.
 *   - This design allows integration of different data sources, such as
 *     direct memory access (DMA) for high-speed transfers and discrete sample input
 *     methods like I2C or SPI.
 *   - Importantly with DMA, there might still be some conversion that needs to be done
 *     before the data is ready for processing, so that can be done in the input buffer 
 *     where it is abstracted away from the processor.
 *   - The processor operates asynchronously, continuously reading from the input buffer,
 *     processing the samples, and writing to the output buffer.
 * 
 * - Quadraphonic Audio:
 *   - The system is designed for four-channel audio, where each sample consists of
 *     four floating-point values corresponding to the four channels.
 *   - All processing components must assume a fixed channel count of four.
 * 
 * - Buffer Constraints and Guarantees:
 *   - Buffers must support block sizes that are powers of 2
 *   - Input buffers should ensure that all samples are within the range [-1.0, 1.0].
 *   - Thread safety is a fundamental requirement, allowing concurrent read and write 
 *     operations.
 *   - The buffers operate asynchronously and may block if they are empty (input) or full (output).
 * 
 * - Processor Behavior:
 *   - A processor operates on blocks of samples, with the block size also being a power of 2.
 *   - The processor is expected to consume data in chunks, ensuring real-time constraints 
 *     are met while preventing data loss.
 *   - The processing function takes an input buffer and an output buffer, ensuring that 
 *     transformations can be applied efficiently without modifying the input data directly.
 *
 * - Error Handling:
 *   - Both input and output buffers provide an `errored()` function to indicate 
 *     unrecoverable failures.
 *   - It is the responsibility of the system to restart or handle errors appropriately.
 * 
 * This interface provides a robust framework for real-time audio processing applications,
 * ensuring high-performance operation while maintaining flexibility for different hardware
 * and software implementations.
 */

typedef float sample_t;

struct QuadSample {
    sample_t channels[4];
};

typedef uint32_t uint_sample_t;

struct QuadIntSample {
    uint_sample_t channels[4];
};

/*
 * Represents a buffer containing input audio samples for processing.
 * 
 * - The buffer holds multiple channels of audio data.
 * - The implementation must support arbitrary buffer sizes that are powers of 2.
 * - Channel count is fixed at 4 (quadraphonic audio).
 * - It is the responsibility of the implementation to ensure the following
 *   - The buffer's internal storage is large enough to hold the requested number of samples.
 *   - The buffer only contains valid samples.
 *   - Every sample is in the range [-1.0, 1.0].
 *   - The buffer may block on a call to `nextSample()` if no samples are available.
 *   - Ensure thread safety
 *     - The buffer will allow the caller to write and read samples concurrently 
 * When we call `start()`, it will begin running asynchronously and will continously fill the buffer with samples.
 */
class QuadInputBuffer {
public:
    virtual ~QuadInputBuffer() = default;

    /*
    * Returns the number of samples in the current block.
    * 
    * - The returned value is always a power of 2.
    * - This represents the number of frames (each containing 4 samples, one per channel).\
    * - This is useful if the processor buffers a block and takes a while to process
    * - It allows us to not skip over any samples as long as each sample is on average processed in less time than the sample rate
    */
    virtual int size() const = 0;
    
    /*
     * Retrieves a sample from the buffer.
     * 
     * - Samples are in floating-point format in the range [-1.0, 1.0]
     */
    virtual QuadSample nextSample() = 0;

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
    virtual bool errored() const = 0;
};

/*
* Represents a buffer used for outputting processed audio samples.
*
* - The buffer holds multiple channels of audio data.
* - The implementation must support arbitrary block sizes that are powers of 2.
* - It is the responsibility of the implementation to ensure the following
*   - The buffer's internal storage is large enough to hold the requested number of samples.
*   - Ensure thread safety
*     - The buffer will allow the caller to write and read samples concurrently
* When we call `start()`, it will begin running asynchronously and will continously read the buffer and output the samples.
*/
class QuadOutputBuffer {
public:
    virtual ~QuadOutputBuffer() = default;

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
    

/*
 * A processor is any object that will process audio.
 * 
 * Each processor specifies a block size.
 * The block size is the number of samples that the processor will process at a time.
 */
class Processor {
public:
    virtual ~Processor() = default;
    /*
     * Processes the input buffer and writes the result to the output buffer.
     * 
     * `input` is the input buffer to process.
     * `output` is the output buffer to write the result to.
     * 
     * - This function will loop forever, processing samples from the input buffer and writing the result to the output buffer.
     * - This function is allowed to block if the input buffer is empty or the output buffer is full.
     */ 
    virtual void process(QuadInputBuffer& input, QuadOutputBuffer& output) = 0;
    /*
     * Returns the block size of the processor.
     *
     * This function is a hint to the system about how many samples the system might have to hold while the processor is running.
     *
     * - The block size is the number of samples that the processor will process at a time.
     * - The block size is always a power of 2.
     *   - This is used to determine the size of the input and output buffers.
     */
    virtual int blockSize() const = 0;
};
