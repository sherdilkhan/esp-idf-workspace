#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include <esp_timer.h> // Include the header for esp_timer_get_time


void vfunction01(){

	for (size_t i = 0; i <=5; i++){
		printf("\nFunction 01 Iteration #:  %d  : Time Snap in MS:   %lld", i, esp_timer_get_time()/1000);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

void vfunction02(){

	for (size_t i = 0; i <=5; i++){
		printf("\nFunction 02 Iteration #:  %d  : Time Snap in MS:   %lld", i, esp_timer_get_time()/1000);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
	vfunction01();
	vfunction02();
}
