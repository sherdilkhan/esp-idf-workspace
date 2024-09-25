#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_defs.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_bt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BLE_CLIENT";

// Replace with your BLE sensor service UUID and characteristic UUID
#define REMOTE_SERVICE_UUID        0x00FF
#define REMOTE_CHARACTERISTIC_UUID 0xFF01

static esp_gatt_if_t client_if;
static uint16_t conn_id;
static esp_bd_addr_t sensor_addr = {0xa0, 0x6c, 0x65, 0xe2, 0x09, 0x7b};  // Update with the actual address


static void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
    switch (event) {
        case ESP_GATTC_REG_EVT:
            ESP_LOGI(TAG, "ESP_GATTC_REG_EVT");
            esp_ble_gap_start_scanning(30);  // Start BLE scan for 30 seconds
            break;
        case ESP_GATTC_CONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTC_CONNECT_EVT, conn_id=%d", param->connect.conn_id);
            conn_id = param->connect.conn_id;
            esp_ble_gattc_search_service(gattc_if, conn_id, NULL);
            break;
        case ESP_GATTC_SEARCH_RES_EVT:
            ESP_LOGI(TAG, "ESP_GATTC_SEARCH_RES_EVT, found service UUID: %x", param->search_res.srvc_id.id.uuid.uuid.uuid16);
            if (param->search_res.srvc_id.id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
                ESP_LOGI(TAG, "Service found. UUID: 0x%04x", REMOTE_SERVICE_UUID);
            }
            break;
        case ESP_GATTC_SEARCH_CMPL_EVT:
            ESP_LOGI(TAG, "ESP_GATTC_SEARCH_CMPL_EVT");
            break;
        case ESP_GATTC_DISCONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTC_DISCONNECT_EVT, reason: %d", param->disconnect.reason);
            break;
        default:
            ESP_LOGI(TAG, "Event %d unhandled", event);
            break;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_RESULT_EVT");
            if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
                if (memcmp(param->scan_rst.bda, sensor_addr, sizeof(esp_bd_addr_t)) == 0) {
                    ESP_LOGI(TAG, "Sensor found, connecting...");
                    esp_ble_gap_stop_scanning();
                    esp_ble_gattc_open(client_if, sensor_addr, true);
                }
            }
            break;
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT");
            break;
        default:
            ESP_LOGI(TAG, "GAP event %d unhandled", event);
            break;
    }
}

void app_main(void) {
    esp_err_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed\n", __func__);
        return;
    }
    
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
        return;
    }
    
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed\n", __func__);
        return;
    }
    
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed\n", __func__);
        return;
    }

    esp_ble_gattc_register_callback(gattc_event_handler);
    esp_ble_gattc_app_register(0);
    esp_ble_gap_register_callback(gap_event_handler);

    esp_ble_gap_set_scan_params(&(esp_ble_scan_params_t){
        .scan_type              = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval          = 0x50,
        .scan_window            = 0x30,
        .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
    });
}