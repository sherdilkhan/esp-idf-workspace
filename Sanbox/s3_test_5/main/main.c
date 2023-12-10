#include <stdio.h>
#include "esp_log.h"

#define TAG "My TAG"

void app_main(void)
{
        printf("Hello from app_main!\n");
        ESP_LOGI(TAG, "Message");
}
