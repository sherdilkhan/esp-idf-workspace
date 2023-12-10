#include <stdio.h>


#define LED_PIN GPIO_NUM_2
#define BUTTON01 GPIO_NUM_3


void task_1(void *pvParameter){
	while(1){
		if (BUTTON01 == 1){
			print("Button01 is Pressed\n");
		}
		else {
			print ("Button01 is Released\n");
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}


void main_app(){
	gpio_config_t button01_config = {
			.pin_bit_mask = (1ULL << BUTTON01),
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
	};
	gpio_config_t led_pin_config = {
			.pin_bit_mask = (1ULL << LED_PIN),
			.mode = GPIO_MODE_OUTPUT,

	};


}
