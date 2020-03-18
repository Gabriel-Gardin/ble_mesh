/*********************************************************************************
 * Código fonte do projeto rastreador utilizando beacons.
 * Framework esp-mdf version:
 * 
 * 
*/

#include "ibeacon_api.h"
#include "mesh_wifi.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "mdf_common.h"
#include "mesh_mqtt_handle.h"
#include "mwifi.h"
#include "mdf_common.h"
#include "esp_err.h"
#include "driver/gpio.h"


static const char* DEMO_TAG = "IBEACON_DEMO";
static const char *TAG = "mqtt_examples";

#define BUILT_IN_LED 22
#define MAX_LAST_SENDED_TIME 1000
#define MAX_ARRAY_SIZE 300
#define SENDED_BEACON_ARRAY_TERMINATOR_ADDR "cococo"

typedef struct{  //Estrutura de dados que marca o endereço do beacon e o momento em que foi enviado.

    esp_bd_addr_t addr;
    int     sended_time;

}bt_addr_time;

typedef struct {
    int beacon_rssi;
    int major;
    int minor;
    int bat_level;
    int mesh_deep;
    esp_bd_addr_t addr;

} bledata;

QueueHandle_t ble_data_queue;

static TaskHandle_t xTaskToNotify = NULL;


static TaskHandle_t sender_task_handle = NULL;
static TaskHandle_t beacon_sended_controller_handle = NULL;



int currently_array_size = 0;
bt_addr_time sended_beacons[MAX_ARRAY_SIZE];


void sender_task(void *pVparamenters);
void receive_ble_data(esp_ble_gap_cb_param_t *ble_adv_data);
void delete_beacon_from_array(int index);
void beacon_sended_controller(void *pVparameters);
void update_array();
void add_data_to_arr(bt_addr_time *data, int time, esp_bd_addr_t addr, bool new_beacon);
esp_err_t set_led_state(bool state);
bool is_beacon_recently_sended(esp_bd_addr_t addr);
void disconected_led();
bool array_is_equal(uint8_t *arr1, uint8_t *arr2, size_t tamanho);


void app_main()
{
    ble_data_queue = xQueueCreate(10, sizeof(bledata)); //Queue para enviar dados a serem enviados pela rede mesh.
    init_wifi_mesh();
    xTaskCreate(sender_task, "Sender_task", 8096, NULL, 6, &sender_task_handle);
    xTaskCreate(beacon_sended_controller, "controller", 8096, NULL, 6, &beacon_sended_controller_handle);
    vTaskDelay(10000);
    ble_ibeacon_init();
}


void sender_task(void *pVparamenters)
{
    gpio_pad_select_gpio(BUILT_IN_LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);
    set_led_state(false);


    bledata data;
    mdf_err_t ret = MDF_OK;
    size_t size   = 0;
    char *str_data    = NULL;
    mwifi_data_type_t data_type     = {0x0};
    uint8_t sta_mac[MWIFI_ADDR_LEN] = {0};
    esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac);


    for(;;)
    {
        int mesh_layer = esp_mesh_get_layer();
        if (!mwifi_is_connected() || !mwifi_get_root_status())
        {
            disconected_led();
        }

        else if(xQueueReceive(ble_data_queue, &data, pdMS_TO_TICKS(1000)) == true)
        {
            size = asprintf(&str_data, "{\"mac\": \"%02x%02x%02x%02x%02x%02x\", \"layer\":%d, \"bt_addr\": \"%02x%02x%02x%02x%02x%02x\", \"bt_major\":%d, \"rssi\":%d, \"mesh_deep\":%d}",
                        MAC2STR(sta_mac), mesh_layer, MAC2STR(data.addr), data.major, data.beacon_rssi, data.mesh_deep);

            ret = mwifi_write(NULL, &data_type, str_data, size, true);
            MDF_FREE(str_data);
            MDF_ERROR_CONTINUE(ret != MDF_OK, "<%s> mwifi_write", mdf_err_to_name(ret));
        }

        else
        {
            if(mesh_layer == 1)
            {
                set_led_state(true);
            }

            else if(mesh_layer != 1)
            {
                set_led_state(false);
            }
            vTaskDelay(1);
        }
    }
}



void beacon_sended_controller(void *pVparameters) //Essa task roda todo segundo atualizando o array global dos beacons recém enviados.
{
    for(;;)
    {
        printf("updatinggggggggggggggggggggg\n");
        //btdm_controller_scan_duplicate_list_clear();
        esp_ble_scan_dupilcate_list_flush();
        //update_array();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/*
void update_array()
{
    uint8_t terminating_str[6] = SENDED_BEACON_ARRAY_TERMINATOR_ADDR;
    uint32_t ulNotificationValue = ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
    if(ulNotificationValue == 1)
    {
        printf("Task is free to run\n");
        
        for(int i = 0; i < 300; i++)
        {
            currently_array_size += i;
            if((esp_timer_get_time() - sended_beacons[i].sended_time) > MAX_LAST_SENDED_TIME)
            {
                delete_beacon_from_array(i);
            }
            if(array_is_equal(sended_beacons[i].addr, terminating_str, 6)) //Se chegou no último elemento do array, para tudo.
            {
                break;
            }
        }
    }
}


void delete_beacon_from_array(int index)
{
    for(int i = index; i < MAX_ARRAY_SIZE -1; i++)
    {
        sended_beacons[i] = sended_beacons[i + 1]; //Desloca todos os ítens subsequentes do ítem a ser deletado para a para a esquerda
    }
    currently_array_size--;
}


void add_data_to_arr(bt_addr_time *data, int time, esp_bd_addr_t addr, bool new_beacon)
{
    if(currently_array_size < 299)
    {
        if(new_beacon)
        {
            sended_beacons[currently_array_size + 1] = sended_beacons[currently_array_size]; //Desloca o elemento que marca o fim do array para a direita.
            memcpy(data->addr, addr, 6);
            //currently_array_size ++;
        }
    data->sended_time = time;
    currently_array_size ++;
    }
    else
    {
        printf("PASSOU DE TREZENTOS!!! OVERFLOW\n");
    }
    
}


bool is_beacon_recently_sended(esp_bd_addr_t addr)
{
    bool result = true;
    
    //printf("aeeeeeeeee\n");
    for(int i = 0; i < MAX_ARRAY_SIZE -1; i++)
    {
        if(array_is_equal((uint8_t*)&sended_beacons[i].addr, (uint8_t*)&addr, 6))
        {
            if((esp_timer_get_time() - sended_beacons[i].sended_time) > MAX_LAST_SENDED_TIME)
            {
                printf("TEMPO É MAIOR QUE O PERMITIDO!!!\n");
                result = false;
                add_data_to_arr(&sended_beacons[i], esp_timer_get_time(), sended_beacons[i].addr, false); //Se o beacon já está no array e o tempo é maior que o máximo, atualiza o tempo do msm na lista.
            }
        }
        else //Se o beacon não estiver no array cria uma nova posição para ele no último local do array...
        {
            result = false;
            printf("ADICIONANDO BEACON A LISTA!!! array size = %i\n", currently_array_size);
            add_data_to_arr(&sended_beacons[currently_array_size], esp_timer_get_time(), addr, true);
        }
            
    }
    xTaskNotifyGive(beacon_sended_controller_handle); //Terminou de verificar se o beacon foi recentemente enviado, libera o semaforo.
    
    return result;
}


bool array_is_equal(uint8_t *arr1, uint8_t *arr2, size_t tamanho)
{
    bool result = true;
    for(int i = 0; i < tamanho; i++)
    {
        printf("%u,  %u\n", arr1[i], arr2[i]);
        if(arr1[i] != arr2[i])
        {
            result = false;
            break;
        }
    }
    return result;
}

*/
void receive_ble_data(esp_ble_gap_cb_param_t *ble_adv_data)
{
    //printf("itens na fila = %i\n", uxQueueMessagesWaiting(ble_data_queue));
    printf("Profundidade = %i\n", esp_mesh_get_layer());
    printf("Minímo memória livre = %i\n", xPortGetMinimumEverFreeHeapSize());
    bledata send_msg;
    esp_ble_ibeacon_t *ibeacon_data = (esp_ble_ibeacon_t*)(ble_adv_data->scan_rst.ble_adv);
    esp_log_buffer_hex("BT address:", ble_adv_data->scan_rst.bda, ESP_BD_ADDR_LEN );
//    esp_log_buffer_hex("Proximity UUID:", ibeacon_data->ibeacon_vendor.proximity_uuid, ESP_UUID_LEN_128);
    uint16_t major = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.major);
    uint16_t minor = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.minor);
    ESP_LOGI(DEMO_TAG, "RSSI of packet:%d dbm", ble_adv_data->scan_rst.rssi);
    
 //   if(!is_beacon_recently_sended(ble_adv_data->scan_rst.bda))
 //   {
        
        printf("ó eu aqui óoooooooooooo\n");
        send_msg.bat_level = 100;
        send_msg.beacon_rssi = ble_adv_data->scan_rst.rssi;
        send_msg.major = ibeacon_data->ibeacon_vendor.minor;
        send_msg.major = ibeacon_data->ibeacon_vendor.major;
        send_msg.mesh_deep = esp_mesh_get_layer();
        memcpy(&send_msg.addr, ble_adv_data->scan_rst.bda, 6);
        xQueueSend(ble_data_queue, (void*)&send_msg, pdMS_TO_TICKS(50));
 //   }
}


void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) //Callback das funcionalidades bluetooth. Registrada no arquivo ibeacon_api.c
{
    esp_err_t err;

    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second, 0 means scan permanently
        uint32_t duration = 0;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if ((err = param->scan_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(DEMO_TAG, "Scan start failed: %s", esp_err_to_name(err));
        }
        break;
        
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            /* Search for BLE iBeacon Packet */            
            if (ble_packet_valid(scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len))
            {
                receive_ble_data(scan_result); //Envia o pacotoe de dados cru.
            }

            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if ((err = param->scan_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(DEMO_TAG, "Scan stop failed: %s", esp_err_to_name(err));
        }
        else {
            ESP_LOGI(DEMO_TAG, "Stop scan successfully");
        }
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(DEMO_TAG, "Adv stop failed: %s", esp_err_to_name(err));
        }
        else {
            ESP_LOGI(DEMO_TAG, "Stop adv successfully");
        }
        break;

    default:
        break;
    }
}

esp_err_t set_led_state(bool state)
{
    esp_err_t ret = gpio_set_level(BUILT_IN_LED, !state);
    return ret;
}


void disconected_led()
{
    set_led_state(true);
    vTaskDelay(250 / portTICK_RATE_MS);
    set_led_state(false);
    vTaskDelay(250 / portTICK_RATE_MS);
}