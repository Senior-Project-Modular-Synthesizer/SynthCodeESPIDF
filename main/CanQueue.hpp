/* push, pop from CanQueue */
/* Use task-safe FreeRTOS queue */
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_log.h"

class CanQueue {
	public:
		CanQueue(twai_handle_t twai_hdl, twai_general_config_t twai_config);

		~CanQueue();		

		void start();

		void stop();
		
		size_t getMessage(const twai_message_t* out);

		void receiveMessage();
		
		void enableMIDI(bool enabled);

		esp_err_t getStatusInfo(twai_status_info_t status_info);

	private:
		QueueHandle_t xMessageQueue;

		twai_handle_t twai_hdl;

		twai_general_config_t twai_cfg;
};