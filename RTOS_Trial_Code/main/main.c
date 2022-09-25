#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

int count1 = 0;
int count2 = 0;

//----------Initialize Task Handle--------------//

TaskHandle_t task1_handle;
TaskHandle_t task2_handle;

//--------------------Task1---------------------//

void task1(){
	for(;;){
		printf("Task1 count is %d\n",count1);
		count1++;
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

//--------------------Task2---------------------//

void task2(){
	for(;;){
		printf("Task2 count is %d\n",count2);
		count2++;
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}


//-------------------Super Important Task--------------//

void superimportant(){
	vTaskSuspendAll();
// Mission Critical Code
	xTaskResumeAll();
}




void app_main(void)
{



    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);
    int level = 0;

//----------Starting Task1-------------//

     xTaskCreate(task1, "Task 1",2048,NULL,1,&task1_handle);

//----------Starting Task2-------------//

     xTaskCreate(task2, "Task 2",2048,NULL,1,&task2_handle);

    while (true) {
        gpio_set_level(GPIO_NUM_8, level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);

        if (count1 > 3 && task1_handle != NULL){
        	vTaskSuspend(task1_handle);
        }

        if (count2 >9){
        	vTaskSuspend(task2_handle);
        }
    }
}

