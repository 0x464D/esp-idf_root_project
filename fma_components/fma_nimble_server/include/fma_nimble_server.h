#pragma once
/*
* Based on:
*    https://github.com/ashus3868/BLE-Connect
*    https://gitlab2.cip.ifi.lmu.de/riedlbe/RIOT/-/blob/4b5dcf60c4646d65d4a21f675e70c7f8ebe7b0f3/examples/nimble_gatt/main.c
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"

extern void run_nimble_thread();