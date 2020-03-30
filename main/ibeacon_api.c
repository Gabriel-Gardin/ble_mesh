#include "ibeacon_api.h"


esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_PASSIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x70, //Faz um scan 70 ms dps do ultimo scan acabar
    .scan_window            = 0x30, //Duração do scan.
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

static const char* DEMO_TAG = "IBEACON_DEMO";

void ble_ibeacon_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(DEMO_TAG, "register callback");

    //register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(DEMO_TAG, "gap register error: %s", esp_err_to_name(status));
        return;
    }
}


bool ble_packet_valid(uint8_t *adv_data, uint8_t adv_data_len)
{
    uint8_t my_uuid[] = {0xe2, 0x0a, 0x39, 0xf4, 0x73, 0xf5, 0x4b, 0xc4, 0xa1, 0x2f, 0x17, 0xd1, 0xad, 0x07, 0xa9, 0x61}; //iBeacon UUID
    bool result = false;
    if ((adv_data != NULL) && (adv_data_len == 0x1F))
    {
        result = true;
        for(int i = 9; i <= 24; i++)
        {
            if(my_uuid[i - 9] != adv_data[i]) //Compara o uuid do pacote com o nosso uuid.
            {
                result = false;
            }
        }
    }
    return result;
}


void ble_ibeacon_init(void)
{

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();

    esp_bluedroid_enable();
    ble_ibeacon_appRegister();
    esp_ble_gap_set_scan_params(&ble_scan_params);
}
