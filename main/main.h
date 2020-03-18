static const char* DEMO_TAG = "IBEACON_DEMO";
static const char *TAG = "mqtt_examples";

#define BUILT_IN_LED 22
#define MAX_LAST_SENDED_TIME 1
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
static TaskHandle_t array_controler_handle = NULL;



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