#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"
#include "nvs_flash.h"


#define SCAN_DURATION 10 // Duration for scanning (in seconds)
#define TAG "BLE_SCAN"

// Function to extract the device name from the advertising data
static void print_device_name(const uint8_t *adv_data, uint8_t adv_data_len) {
    uint8_t len;
    uint8_t *adv_name = esp_ble_resolve_adv_data(adv_data, ESP_BLE_AD_TYPE_NAME_CMPL, &len);
    if (adv_name != NULL && len > 0) {
        ESP_LOGI(TAG, "Device name: %.*s", len, adv_name);
    } else {
        ESP_LOGI(TAG, "No name available");
    }
}

static void gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    if (event == ESP_GAP_BLE_SCAN_RESULT_EVT) {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            // Print the MAC address of the detected device
            ESP_LOGI(TAG, "Found device with MAC address: %02x:%02x:%02x:%02x:%02x:%02x",
                     scan_result->scan_rst.bda[0], scan_result->scan_rst.bda[1],
                     scan_result->scan_rst.bda[2], scan_result->scan_rst.bda[3],
                     scan_result->scan_rst.bda[4], scan_result->scan_rst.bda[5]);

            // Print the device name (if available)
            print_device_name(scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len);
            break;
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            ESP_LOGI(TAG, "Scan complete");
            break;
        default:
            break;
        }
    }
}

void app_main() {
	
    esp_err_t ret;
    
        // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase()); // Erase NVS if it is not initialized properly
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret); // Check for errors
    

    // Initialize BT controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "Failed to initialize BT controller: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "Failed to enable BT controller: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize Bluedroid stack
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "Failed to initialize Bluedroid: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Failed to enable Bluedroid: %s", esp_err_to_name(ret));
        return;
    }

    // Register GAP callback function
    esp_ble_gap_register_callback(gap_callback);

    // Start scanning for BLE devices
    esp_ble_gap_start_scanning(SCAN_DURATION);
}
