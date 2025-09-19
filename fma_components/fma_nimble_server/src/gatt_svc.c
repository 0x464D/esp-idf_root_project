/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Includes */
#include "gatt_svc.h"
#include "common.h"


#define GATT_SERVICE            0xCAFE
#define GATT_WRITE_COMMAND      0xFEED
#define GATT_READ_ASKED_DATA    0xFACE

// This depends og the amount of MTU Maximum Transfer Unit
// The amount can be set as prefered on menuconfig
// But you can Request MTU setting MTU between 23 and 517 as BLE protocol convention
#define STR_ANSWER_BUFFER_SIZE 512
static char write_data[STR_ANSWER_BUFFER_SIZE] = "This characteristic is read- and writeable!";

#define MAX_KEY_LEN 128
#define MAX_VALUE_LEN 128

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} ble_command;

// Get key value pair
ble_command process_command(const char *command) {
    ble_command cmd;
    memset(&cmd, 0, sizeof(cmd));

    char *sep = strchr(command, '=');
    if (sep != NULL) {
        size_t key_len = sep - command;
        size_t value_len = strlen(sep + 1);

        if (key_len < MAX_KEY_LEN) {
            strncpy(cmd.key, command, key_len);
            cmd.key[key_len] = '\0';
        } else {
            printf("Key too long: %zu\n", key_len);
        }

        if (value_len < MAX_VALUE_LEN) {
            strncpy(cmd.value, sep + 1, value_len);
            cmd.value[value_len] = '\0';
        } else {
            printf("Value too long: %zu\n", value_len);
        }

        printf("Key: %s, Value: %s\n", cmd.key, cmd.value);
    } else {
        printf("Not valid command: %s\n", command);
    }

    return cmd;
}

// Write data to ESP32 defined as server
static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    // printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);

    // char * data = (char *)ctxt->om->om_data;
    // ESP_LOGI(TAG, "Lenght: %d", ctxt->om->om_len);
    // ESP_LOGI(TAG, "Data: %s", data);
    

    uint16_t om_len;
    om_len = OS_MBUF_PKTLEN(ctxt->om);
    if (om_len > STR_ANSWER_BUFFER_SIZE) {
        ESP_LOGW(TAG, "om_len exceeds the maximum allowed: %d (%d)", om_len, STR_ANSWER_BUFFER_SIZE);
        om_len = STR_ANSWER_BUFFER_SIZE;
    }
    ESP_LOGI(TAG, "om_len: %d", om_len);

    //  --------------------------------------------
    /* read sent data */
    int rc = ble_hs_mbuf_to_flat(ctxt->om, &write_data,
                            sizeof write_data, &om_len);
    /* we need to null-terminate the received string */
    write_data[om_len] = '\0';

    ESP_LOGI(TAG, "Recovered: %s", write_data);
   
    // ---------------------------------------------
    // get key, value pair
    ble_command cmd = process_command(write_data);
  
    return rc;
}

// Read data from ESP32 defined as server
static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}

/* GATT services table */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    // Servicio para mandar comandos al esp32
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(GATT_SERVICE), // Define UUID for device type
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(GATT_READ_ASKED_DATA), // Define UUID for reading
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_read},
         {.uuid = BLE_UUID16_DECLARE(GATT_WRITE_COMMAND), // Define UUID for writing
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {0} // No more services
};

/*
 *  Handle GATT attribute register events
 *      - Service register event
 *      - Characteristic register event
 *      - Descriptor register event
 */
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    /* Local variables */
    char buf[BLE_UUID_STR_LEN];

    /* Handle GATT attributes register events */
    switch (ctxt->op)
    {

    /* Service register event */
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;

    /* Characteristic register event */
    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG,
                 "registering characteristic %s with "
                 "def_handle=%d val_handle=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;

    /* Descriptor register event */
    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    /* Unknown event */
    default:
        assert(0);
        break;
    }
}

/*
 *  GATT server initialization
 *      1. Initialize GATT service
 *      2. Update NimBLE host GATT services counter
 *      3. Add GATT services to server
 */
int gatt_svc_init(void)
{
    /* Local variables */
    int rc;

    /* 1. GATT service initialization */
    ble_svc_gatt_init();

    /* 2. Update GATT services counter */
    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0)
    {
        return rc;
    }

    /* 3. Add GATT services */
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0)
    {
        return rc;
    }

    return 0;
}
