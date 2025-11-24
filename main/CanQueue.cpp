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
#include "esp_log.h"

#define QUEUE_SIZE 128
#define RX_GPIO_PIN 43
#define TX_GPIO_PIN 44
// TODO: Implement ISR handling also?

static const char* TWAI = "TWAIDRIVER";

// Initializer list here, because we want config responsibility to fall on the caller
// (Largely in case of difference in operation)
CanQueue::CanQueue(twai_handle_t twai_handle, twai_general_config_t twai_config) 
	: twai_hdl(twai_handle), twai_cfg(twai_config) {
	
	xMessageQueue = xQueueCreate(QUEUE_SIZE, sizeof(twai_message_t *));

	esp_err_t ret = twai_driver_install(&twai_config, nullptr, nullptr);

	if (ret != ESP_OK) {
		ESP_LOGE(TWAI, "TWAI Driver install failed: %s", esp_err_to_name(ret));
	}
}

CanQueue::~CanQueue() {
    if (twai_hdl) heap_caps_free(twai_hdl);
    twai_hdl = nullptr;

    if (xMessageQueue) vQueueDelete(xMessageQueue);
}

void CanQueue::start() {
    twai_start();
}

void CanQueue::stop() {
	twai_stop();
}

size_t CanQueue::getMessage(const twai_message_t* out) {	
	if (xQueueReceive(xMessageQueue, &out, (TickType_t) 10) == pdPASS) {}
	else {
		ESP_LOGE(TWAI, "Waited for 10 ticks but did not receive message from TWAI Bus");
		return 0;
	}

	return out->data_length_code;
}

void CanQueue::receiveMessage() {
	twai_message_t message;
	
	if (twai_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK) {
		xQueueSendToBack( xMessageQueue, (void *) &message, (TickType_t) 0 );
	}
}

void CanQueue::enableMIDI(bool enabled) {
	
}

esp_err_t CanQueue::getStatusInfo(twai_status_info_t status_info) {
	return twai_get_status_info(&status_info);
}
