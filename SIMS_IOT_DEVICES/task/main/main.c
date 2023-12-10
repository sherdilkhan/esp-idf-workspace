#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_timer.h" // Include the header for esp_timer_get_time


void vtask01(){
	for(size_t i = 0; i<=5; i++){
		printf("\nTask 01 Iteration #:  %zu : Time Snap in MS: %lld", (size_t)i, esp_timer_get_time() / 1000);
		vTaskDelay(1000/ portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

void vtask02 (){
	for (size_t i = 0; i<=5; i++){
		printf("\nTask 02 Iteration #: %zu Time Snap in MS: %lld", (size_t)i, esp_timer_get_time()/1000);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

void app_main(void)

{
	xTaskCreate(vtask01, "My First Task 01", 4096, NULL, 5, NULL);
	xTaskCreate(vtask02, "My 2nd Task 02", 4096, NULL, 8, NULL);
	printf("\nStart of main program: @ timer value:  %lld", esp_timer_get_time() / 1000);
	printf("\nProgram is running_1\n");
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	printf("\nProgram is running_1\n");
	vTaskDelay(1000 / portTICK_PERIOD_MS);

}
