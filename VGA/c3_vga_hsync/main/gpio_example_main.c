
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output
 * GPIO19: output
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Test:
 * Connect GPIO18 with GPIO4
 * Connect GPIO19 with GPIO5
 * Generate pulses on GPIO18/19, that triggers interrupt on GPIO4/5
 *
 */

#define green    19
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<green)
#define hsync     18
#define GPIO_INPUT_PIN_SEL  (1ULL<<hsync)
#define ESP_INTR_FLAG_DEFAULT 1

//static xQueueHandle gpio_evt_queue = NULL;

int i;
int level = 0;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
	printf("interrupt Triggered \n");
			 for(i=0; i<=10;i++)
			 	{
			 		gpio_set_level(green,level);
			 		ets_delay_us(2);
			 		level = !level;
			 		gpio_set_level(green,level);
			 		ets_delay_us(1);
			 		level = !level;
			 	}
   // uint32_t gpio_num = (uint32_t) arg;
  //  xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}


void app_main(void)
{
	gpio_pad_select_gpio(green);
	gpio_set_direction (green, GPIO_MODE_OUTPUT);

    gpio_config_t io_conf = {};
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);
    //change gpio intrrupt type for one pin
    gpio_set_intr_type(hsync, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    //gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(hsync, gpio_isr_handler, (void*) hsync);
    //remove isr handler for gpio number.
    gpio_isr_handler_remove(hsync);
    //hook isr handler for specific gpio pin again
    //gpio_isr_handler_add(hsync, gpio_isr_handler, (void*) hsync);

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    int cnt = 0;
    while(1) {
    	printf("cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
