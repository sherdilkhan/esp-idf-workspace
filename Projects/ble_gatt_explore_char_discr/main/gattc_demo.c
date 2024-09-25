#include <string.h> // Add this line for memcpy
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "nvs_flash.h"

#define TAG "BLE_GATT_CLIENT"

// Replace this with the actual MAC address of your BLE device
//static esp_bd_addr_t server_mac = {0xa0, 0x6c, 0x65, 0xe2, 0x09, 0x7b}; // Replace XX with your device MAC address

uint8_t device_mac[6]  = {0xa0, 0x6c, 0x65, 0xe2, 0x09, 0x7b}; // Replace XX with your device MAC address
//static esp_gatt_if_t gattc_if;
uint16_t gattc_if = 0; // You'll get this from your app registration


// Callback function for GATT client events
static void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(TAG, "GATT client registered, attempting to connect to device");
        // Specify the address type: ESP_LE_ADDR_PUBLIC or ESP_LE_ADDR_RANDOM
    	esp_bd_addr_t server_mac;
    	memcpy(server_mac, device_mac, sizeof(server_mac));
    
        esp_ble_gattc_open(gattc_if, server_mac, BLE_ADDR_TYPE_PUBLIC, true);  // Modified to pass all required arguments
        break;
    case ESP_GATTC_SEARCH_RES_EVT:
        //ESP_LOGI(TAG, "Service found with UUID: %04x", param->search_res.srvc_id.uuid.uuid.uuid16);
        if (param->search_res.srvc_id.uuid.len == ESP_UUID_LEN_16) {
    		ESP_LOGI(TAG, "Service found with UUID: %04x", param->search_res.srvc_id.uuid.uuid.uuid16);
		} else if (param->search_res.srvc_id.uuid.len == ESP_UUID_LEN_128) {
    		uint8_t *uuid128 = param->search_res.srvc_id.uuid.uuid.uuid128;
    		ESP_LOGI(TAG, "Service found with UUID: %02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
             uuid128[15], uuid128[14], uuid128[13], uuid128[12],
             uuid128[11], uuid128[10],
             uuid128[9], uuid128[8],
             uuid128[7], uuid128[6],
             uuid128[5], uuid128[4], uuid128[3], uuid128[2], uuid128[1], uuid128[0]);
		}
        break;

    case ESP_GATTC_CONNECT_EVT:
    	ESP_LOGI(TAG, "Connected to the server, conn_id: %d", param->connect.conn_id);
    	esp_ble_gattc_search_service(gattc_if, param->connect.conn_id, NULL);
    	break;
	case ESP_GATTC_DISCONNECT_EVT:
    	ESP_LOGI(TAG, "Disconnected from the server, reason: 0x%x", param->disconnect.reason);
    	// Optionally try to reconnect after a delay
    	vTaskDelay(pdMS_TO_TICKS(2000));  // Delay for 2 seconds
    	esp_ble_gattc_open(gattc_if, server_mac, BLE_ADDR_TYPE_PUBLIC, true);  // Attempt to reconnect
    	break;	
    default:
        break;
    }
}

void app_main(void) {
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize the Bluetooth controller and enable BLE
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    ESP_ERROR_CHECK(ret);
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    ESP_ERROR_CHECK(ret);

    // Initialize Bluedroid stack
    ret = esp_bluedroid_init();
    ESP_ERROR_CHECK(ret);
    ret = esp_bluedroid_enable();
    ESP_ERROR_CHECK(ret);

    // Register GATT client callback and initialize GATT client
    ret = esp_ble_gattc_register_callback(gattc_event_handler);
    ESP_ERROR_CHECK(ret);

    // Register GATT client app (you can register multiple client apps for different services)
    ret = esp_ble_gattc_app_register(0);  // Register with the application ID of 0
    ESP_ERROR_CHECK(ret);
}
