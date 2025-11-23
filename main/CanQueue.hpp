/* push, pop from CanQueue */
/* Use task-safe FreeRTOS queue */
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/twai.h"

class CanQueue {
	public:
		CanQueue();
		~CanQueue();		

		void start();

		void stop();
		
		twai_message_t* getMessage();
		
		void enableMIDI(bool enabled);

	private:
		
		QueueHandle_t xMessageQueue;

};
