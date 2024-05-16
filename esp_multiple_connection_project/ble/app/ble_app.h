/* ****************************************************************************
 * @Project Name : ESP BLE Operation
 *
 *
 *      @Purpose :ESP BLE APP Header File
 *      @Date    : 15.02.2024
 *      @Author  : Furkan YAYLA
 *      @e-mail  : furkan.yayla@saykal.com
 *
 * ****************************************************************************
 **/

#ifndef TCP_CLIENT_FIRMWARE_ESP_APP_APP_H_
#define TCP_CLIENT_FIRMWARE_ESP_APP_APP_H_

#include "hardware_init.h"
#include "ble_api.h"
#include <string.h>

#define SEND_BLE_MESSAGE_TIMEOUT 3000

typedef struct
{
	unsigned char connection;
	unsigned char reset;
}esp_ble_app_flag_t;

typedef struct
{
	esp_ble_t		_ble;
	queue_t			queue;
	unsigned int		tx_timer;
	esp_ble_app_flag_t	flag;
        unsigned char           total_received_data[QUEUE_SIZE];
}esp_ble_app_t;

void esp_ble_app_init(void);
void esp_ble_app_1_ms_timer(void);
void esp_ble_app_polling(void);
void esp_ble_get_received_data(unsigned char * rx_data, unsigned int rx_data_length);

#endif /* TCP_CLIENT_FIRMWARE_ESP_APP_APP_H_ */
