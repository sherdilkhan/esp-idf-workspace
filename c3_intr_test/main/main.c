
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define GPIO_OUTPUT_IO_0    19
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_IO_0)
#define GPIO_INPUT_IO_0    4
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0


static xQueueHandle gpio_evt_queue = NULL;

int i;
int level = 0;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{

    int btn = *(int*) arg;
    int current_level = gpio_get_level(btn);
    BaseType_t ContextSwitchRequest = pdFALSE;

    xQueueSendToBackFromISR(test_queue_handle,(void*)&current_level,&ContextSwitchRequest);

    if(ContextSwitchRequest){
        taskYIELD();
    }
	printf("interrupt Triggered");

/*	vTaskDelay(1000 / portTICK_RATE_MS);
			 for(i=0; i<=10;i++)
			 	{
			 		gpio_set_level(GPIO_OUTPUT_PIN_SEL,level);
			 		ets_delay_us(2);
			 		level = !level;
			 		gpio_set_level(GPIO_OUTPUT_PIN_SEL,level);
			 		ets_delay_us(1);
			 		level = !level;
			 	}
*/
}


static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}
void app_main(void)
{

	//zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gp io isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //remove isr handler for gpio number.
     gpio_isr_handler_remove(GPIO_INPUT_IO_0);
     //hook isr handler for specific gpio pin again
     gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());


    while(1) {
        printf(":) \n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
