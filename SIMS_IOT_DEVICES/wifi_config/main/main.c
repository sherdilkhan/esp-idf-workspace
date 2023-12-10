#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "sdkconfig.h"

void app_main(void)
{
        printf("\nSSID %s\n", CONFIG_ESP_WIFI_SSID);
        printf("Password %s\n", CONFIG_ESP_WIFI_PASSWORD);
}
