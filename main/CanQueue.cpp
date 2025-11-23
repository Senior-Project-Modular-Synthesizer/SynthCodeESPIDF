#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_check.h"
#include "esp_log.h"

#include <array>
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "Processor.hpp"

#include "Buffers.hpp"
#include "AK4619VN.hpp"
#include "math.h"
#include "CanQueue.hpp"

CanQueue::CanQueue() {

}

CanQueue::~CanQueue() {

}

void CanQueue::start() {

}

void CanQueue::stop() {

}

twai_message_t* getMessage() {
	return nullptr;
}

void enableMIDI(bool enabled) {

}