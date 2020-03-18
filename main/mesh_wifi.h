#ifndef MESH_WIFI_H
#define MESH_WIFI_H
#include "esp_err.h"


esp_err_t init_wifi_mesh();
void root_write_task(void *arg);

#endif