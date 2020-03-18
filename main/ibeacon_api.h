#ifndef IBEACON_API_H
#define IBEACON_API_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"


/* Because current ESP IDF version doesn't support scan and adv simultaneously,
 * so iBeacon sender and receiver should not run simultaneously */
#define IBEACON_SENDER      0
#define IBEACON_RECEIVER    1
#define IBEACON_MODE IBEACON_SENDER

#define BEACON_UUID "e20a39f473f54bc4a12f17d1ad07a961"


//e20a39f473f54bc4a12f17d1ad07a961 uuid

/* Major and Minor part are stored in big endian mode in iBeacon packet,
 * need to use this macro to transfer while creating or processing
 * iBeacon data */
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))

typedef struct {
    uint8_t flags[3];
    uint8_t length;
    uint8_t type;
    uint16_t company_id;
    uint16_t beacon_type;
}__attribute__((packed)) esp_ble_ibeacon_head_t;

typedef struct {
    uint8_t proximity_uuid[16];
    uint16_t major;
    uint16_t minor;
    int8_t measured_power;
}__attribute__((packed)) esp_ble_ibeacon_vendor_t;


typedef struct {
    esp_ble_ibeacon_head_t ibeacon_head;
    esp_ble_ibeacon_vendor_t ibeacon_vendor;
}__attribute__((packed)) esp_ble_ibeacon_t;


/* Constant part of iBeacon data */
extern esp_ble_ibeacon_head_t ibeacon_common_head;


/************************************************************************************
***************************Functions declartions**********************************
*************************************************************************************/
bool esp_ble_is_ibeacon_packet (uint8_t *adv_data, uint8_t adv_data_len);
esp_err_t esp_ble_config_ibeacon_data (esp_ble_ibeacon_vendor_t *vendor_config, esp_ble_ibeacon_t *ibeacon_adv_data);
bool ble_packet_valid(uint8_t *adv_data, uint8_t adv_data_len);
void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void ble_ibeacon_init(void);


#endif