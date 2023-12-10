#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h> // Include the header for esp_timer_get_time
#include <stdio.h>


void app_main() {

	while(1){

	printf("PortTickrate Period in msec is: %lu\n", (unsigned long)portTICK_PERIOD_MS);
    printf("Time Snap 1: %lld MiliSeconds\n", esp_timer_get_time()/1000);
    vTaskDelay(3000 / portTICK_PERIOD_MS);  //Number for systicks to add non blocking delay
    printf("Time Snap 2: %lld MiliSeconds\n", esp_timer_get_time()/1000);
	}

}
