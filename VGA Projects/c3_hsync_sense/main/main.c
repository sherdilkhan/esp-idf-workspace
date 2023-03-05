#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define g 21   //RGB LED on board
#define hsync 19  //pushbutton input
#define ESP_INTR_FLAG_DEFAULT   0

int i = 0;
int level = 0;


static void IRAM_ATTR gpio_isr_handler (void* arg)
{
	printf("interrupt Triggered \n");
	//ets_delay_us(500000);
	vTaskDelay(1000 / portTICK_RATE_MS);
	for(i=0; i<=200;i++)
	{
		gpio_set_level(g,level);
		ets_delay_us(1.6);
		level = !level;
		gpio_set_level(g,level);
		ets_delay_us(1.6);
		level = !level;
	}
}

void app_main(void)
{

	/***********Config Outputs*****************/

	gpio_set_direction (g, GPIO_MODE_OUTPUT);

	/***********Config Inputs*****************/

	gpio_config_t input = {};
	input.pin_bit_mask = (1ULL <<hsync);
	//interrupt of rising edge
	input.intr_type = GPIO_INTR_ANYEDGE;
	input.mode = GPIO_MODE_INPUT;
	input.pull_up_en = 0;
	input.pull_down_en = 0;
	//input.intr_type = 1;
	gpio_config (&input);

	/***************ISR Handler*******************/


	//change gpio intrrupt type for one pin
	gpio_set_intr_type(hsync, GPIO_INTR_ANYEDGE);   //4 - input low level trigger

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	//hook isr handler for specific gpio pin
	 gpio_isr_handler_add(hsync, gpio_isr_handler, (void*) hsync);

	 printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());


	 while (true)
	 {
		 vTaskDelay(1000 / portTICK_RATE_MS);

	 }

}


