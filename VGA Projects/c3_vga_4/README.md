#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "driver/gpio.h"

#define LED_PIN 8


void app_main(void)
{
     gpio_reset_pin(LED_PIN);                         // Pin reset 
     gpio_pad_select_gpio(LED_PIN);                   //GPIO Pin function selection 
     gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);   // Set the direction to output 
     while (1)
     {
        gpio_set_level(LED_PIN,1);                    // Set up LED_PIN High level 
        vTaskDelay(1000/portTICK_RATE_MS);
        gpio_set_level(LED_PIN,0);                    // Set up LED_PIN Low level 
        vTaskDelay(1000/portTICK_RATE_MS);
     }

     
}

