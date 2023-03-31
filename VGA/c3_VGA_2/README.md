ESP-IDF template app
====================

This is a template application to be used with [Espressif IoT Development Framework](https://github.com/espressif/esp-idf).

Please check [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for getting started instructions.

*Code in this repository is in the Public Domain (or CC0 licensed, at your option.)
Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.*

#include "driver/gpio.h"


#define hsync GPIO_NUM_20  //hsync from VGA Controller
#define vsync GPIO_NUM_21  //vsync from VGA Controller



static void IRAM_ATTR gpio_isr_handler(void* arg)
{

//ISR Code

}

/*----------------Interrupt Config)------------*/

void config_hsync_interrupt()
{

gpio_config_t io_conf;
io_conf.intr_type = hsync_INT_ANYEDGE;
io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;


}



void app_main (void){


gpio_set_direction (GPIO_NUM_hsync,GPIO_MODE_INPUT);
gpio_set_direction (GPIO_NUM_vsync,GPIO_MODE_INPUT);

esp_intr_alloc(hsync);





}