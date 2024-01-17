#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_gatt_common_api.h"

#define GATTS_TAG "GATTS_LED"

#define PROFILE_NUM 1
#define PROFILE_APP_IDX 0
#define ESP_APP_ID 0x55

#define LED_PIN GPIO_NUM_2 // Change this to the GPIO pin connected to your LED

static const uint16_t GATTS_SERVICE_UUID_TEST_A = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A = 0xFF01;

static const uint8_t SERVICE_UUID[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    GATTS_SERVICE_UUID_TEST_A & 0xFF,
    GATTS_SERVICE_UUID_TEST_A >> 8,
};

static const uint8_t CHAR_UUID[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    GATTS_CHAR_UUID_TEST_A & 0xFF,
    GATTS_CHAR_UUID_TEST_A >> 8,
};

static const char *device_name = "ESP_LED_Control";

static uint8_t char1_str[] = {0x11, 0x22, 0x33};

static uint8_t adv_config_done = 0;

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(SERVICE_UUID),
    .p_service_uuid = SERVICE_UUID,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        // advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(GATTS_TAG, "Advertising start failed");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(GATTS_TAG, "Advertising stop failed");
        }
        else
        {
            ESP_LOGI(GATTS_TAG, "Stop adv successfully");
        }
        break;
    default:
        break;
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
        esp_ble_gap_set_device_name(device_name);
        esp_ble_gap_config_adv_data(&adv_data);
        break;
    case ESP_GATTS_READ_EVT:
        break;
    case ESP_GATTS_WRITE_EVT:
        if (!param->write.is_prep)
        {
            if (param->write.len == 1 && param->write.handle == gl_profile_tab[PROFILE_APP_IDX].char_handle)
            {
                if (param->write.value[0] == 0x01)
                {
                    gpio_set_level(LED_PIN, 1);
                }
                else if (param->write.value[0] == 0x00)
                {
                    gpio_set_level(LED_PIN, 0);
                }
            }
        }
        break;
    case ESP_GATTS_EXEC_WRITE_EVT:
    case ESP_GATTS_MTU_EVT:
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        break;
    case ESP_GATTS_OPEN_EVT:
        break;
    case ESP_GATTS_CANCEL_OPEN_EVT:
        break;
    case ESP_GATTS_CLOSE_EVT:
        break;
    case ESP_GATTS_LISTEN_EVT:
        break;
    case ESP_GATTS_CONGEST_EVT:
        break;
    default:
        break;
    }
}

static void init_led()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
}

static void gatts_led_task(void *pvParameter)
{
    init_led();

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    esp_event_loop_create_default();

    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_profile_event_handler));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(ESP_APP_ID));

    esp_ble_gatts_app_register(ESP_APP_ID);

    esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, 1, SVC_INST_ID);

    xTaskCreate(gatts_led_task, "gatts_led_task", 2048, NULL, 10, NULL);

    esp_ble_gatts_app_register(ESP_APP_ID);
}
