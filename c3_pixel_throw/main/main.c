#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define g 21   //RGB LED on board

int i = 0;
int level = 0;

void app_main(void)
{
	gpio_set_direction (g, GPIO_MODE_OUTPUT);


	 while (true)
	 {
		 printf("interrupt Triggered \n");
		 for(i=0; i<=1000;i++)
		 	{
		 		gpio_set_level(g,level);
		 		ets_delay_us(1.6);
		 		level = !level;
		 		gpio_set_level(g,level);
		 		ets_delay_us(1.6);
		 		level = !level;
		 	}
		 //vTaskDelay(1000 / portTICK_RATE_MS);
	 }

}
