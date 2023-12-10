#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_timer.h" // Include the header for esp_timer_get_time
#include "freertos/task.h"
#include "freertos/semphr.h"

int sharedVar = 0;

SemaphoreHandle_t mutex;

void vTask01(){
	xSemaphoreTake(mutex, portMAX_DELAY);
	sharedVar++;
	xSemaphoreGive(mutex);
	printf("Variable Edited by Task 01 --> %d\n", sharedVar);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	vTaskDelete(NULL);
}

void vTask02(){
	xSemaphoreTake(mutex, portMAX_DELAY);
	sharedVar--;
	xSemaphoreGive(mutex);
	printf("Variable Edited by Task 02 --> %d\n", sharedVar);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	vTaskDelete(NULL);
}

void app_main(void)
{
	mutex = xSemaphoreCreateMutex();
	//printf("\n Lets Begin! @ : %lld MS\n", esp_timer_get_time()/1000);
	xTaskCreate(vTask01, "Task01", 2048, NULL, 5, NULL);
	xTaskCreate(vTask02, "Task02", 2048, NULL, 5, NULL);

}
