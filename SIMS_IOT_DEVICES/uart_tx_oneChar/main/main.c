// Transmitting one character via RS232 ("A" = 01000001 and "A"+parity bit = 010000010)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"

static const int RX_BUF_SIZE = 1024;

#define UART_0_TX 43
#define UART_0_RX 44

void init()
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE, // 0
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_0_TX, UART_0_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}

static void tx_task()
{
    char *data = "A";

    while (1)
    {
        int len = strlen(data);
        int txBytes = uart_write_bytes(UART_NUM_0, data, len);
        printf("%d", txBytes);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    printf("Send data:\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    init();
    xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
}
