#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"
#include "esp_log.h"
#include "led.h"

#define TAG "LED"

/* Local variable definitions */
static rmt_encoder_handle_t led_encoder;

void led_initialize(void) {
    gpio_config_t io_conf = {0};; // Zero-initialized GPIO configuration struct
    led_strip_encoder_config_t encoder_config = {
        .resolution = 10000000, // Set encoder resolution to 10 MHz (example value)
    };
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder)); // Create and initialize a new LED strip encoder

    // Configure LED GPIO
    io_conf.mode = GPIO_MODE_OUTPUT; // Set GPIO mode to output
    io_conf.pin_bit_mask = ((uint64_t)1 << CONFIG_LED_GPIO); // Set the GPIO pin for the LED
    ESP_ERROR_CHECK(gpio_config(&io_conf)); // Apply the GPIO configuration
    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LED_GPIO, false));  // Set the GPIO pin to low (turn off the LED initially)

    led_set_state(LED_STARTUP); // Set the initial state of the LED (e.g., during startup)
}

void led_set_state(led_state_t led_state) {
    gpio_config_t io_conf = {0};;   // Zero-initialized GPIO configuration struct
    
    // Configuration for the RMT transmission
    rmt_transmit_config_t tx_config = { 
        .loop_count = 5,  // Specifies how many times the transmission should repeat
    };
    uint8_t led_strip_pixels[3] = {0}; // Array for holding RGB values of the LED strip
    rmt_channel_handle_t led_chan; // Handle for the RMT channel
    
    // Configuration for the RMT TX (transmit) channel
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,  // Use default clock source for RMT
        .gpio_num = CONFIG_LED_GPIO,     // GPIO pin number for the LED strip
        .mem_block_symbols = 64,         // Number of memory block symbols (buffer size for transmission)
        .resolution_hz = 10000000,       // Set the RMT resolution to 10 MHz
        .trans_queue_depth = 4,          // Depth of the transmission queue
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));   // Create a new RMT TX channel and enable it
    ESP_ERROR_CHECK(rmt_enable(led_chan));

    switch (led_state) {
    case LED_STARTUP:
        led_strip_pixels[0] = led_strip_pixels[1] = led_strip_pixels[2] = 0x0F;
        break;
    case LED_OK:
        led_strip_pixels[0] = 0x0F;
        break;
    case LED_BLE_CONNECTED:
        led_strip_pixels[1] = 0x0F;
        break;
    case LED_RF_ACTIVE:
        led_strip_pixels[0] = led_strip_pixels[1] = 0x0F;
        break;
    case LED_ERROR:
        led_strip_pixels[2] = 0x0F;
        break;
    }

    ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config)); // Transmit the LED color data using the RMT channel
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(rmt_disable(led_chan));
    ESP_ERROR_CHECK(rmt_del_channel(led_chan));

     // Reconfigure the GPIO pin to be in output mode and set it low
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((uint64_t)1 << CONFIG_LED_GPIO);
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LED_GPIO, false));
}
