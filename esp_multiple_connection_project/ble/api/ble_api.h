/* ****************************************************************************
 * @Project Name : ESP BLE Operation
 *
 *
 *      @Purpose :ESP BLE API Header File
 *      @Date    : 15.02.2024
 *      @Author  : Furkan YAYLA
 *      @e-mail  : furkan.yayla@saykal.com
 *
 * ****************************************************************************
 **/

#ifndef TCP_CLIENT_FIRMWARE_ESP_API_API_H_
#define TCP_CLIENT_FIRMWARE_ESP_API_API_H_


#include "queue.h"
#include <stdio.h>

#define BLE_RESET_INT_VALUE       ((int)0)
#define BLE_UART_SYNC_TIMEOUT    ((int)10) // in_ms
#define BLE_STATE_DELAY_TIMEOUT ((int)200) // in_ms


#define BLE_S_BUFF_SIZE  20
#define BLE_L_BUFF_SIZE  50
#define BLE_XL_BUFF_SIZE 100
#define BLE_TX_BUFF_SIZE 500

#define BLE_MAX_RX_DATA_LENGTH 37

typedef struct
{
   void (* ble_device_detectable)   (void * ble_obj, void * reference_obj);
   void (* ble_connection_success)  (void * ble_obj, void * reference_obj);
   void (* ble_connection_fail)     (void * ble_obj, void * reference_obj);
   void (* ble_reset_waiting)       (void * ble_obj, void * reference_obj);
   void (* ble_transmit_data)       (void * ble_obj, void * reference_obj, const unsigned char * tx_data, unsigned int tx_data_length);
   void (* ble_received_data)       (void * ble_obj, void * reference_obj, const unsigned char * rx_data, unsigned int rx_data_length);
}esp_ble_cb;

typedef enum
{
  ESP_BLE_FAIL,
  ESP_BLE_SUCCESS,
  ESP_BLE_CREATE_SUCCESS,
  ESP_BLE_LOOP,
  ESP_BLE_PARAMETERS_MISSING,
  ESP_BLE_INIT_FAIL             = 92,
  ESP_BLE_ADVDATAEX_FAIL        = 93,
  ESP_BLE_BLEGATTSSRVCRE_FAIL   = 94,
  ESP_BLE_BLEGATTSSRVSTART_FAIL = 95,
  ESP_BLE_BLEADVSTART_FAIL      = 96,
  ESP_BLE_BLEGATTSIND_FAIL      = 97,
  ESP_BLE_TRANSMIT_FAIL         = 98,
  ESP_BLE_HARD_FAIL				= 99
}esp_ble_return_type_e;

typedef enum
{
  AT_INIT,
  AT_ADVDATAEX,
  AT_BLEGATTSSRVCRE,
  AT_BLEGATTSSRVSTART,
  AT_BLEADVSTART,
  AT_BLEGATTSIND,
  BLE_LOOP_STATE,
  BLE_TRANSMIT_STATE,
  BLE_DELAY_STATE,
  BLE_RESET_STATE       /**@Brief: This state called "ble_reset_waiting" callback function**/
}esp_ble_at_state_e;

typedef enum
{
  BLE_COMMAND_TRANSMITTING,
  BLE_COMMAND_RESPONSE_RECEIVING,
  BLE_COMMAND_ATTEMPT_CONTROL
}esp_ble_command_state_e;

typedef struct
{
   unsigned char ble_name[BLE_S_BUFF_SIZE];
   unsigned char manufacturer_data[BLE_S_BUFF_SIZE];
}esp_ble_parameters_t;

typedef struct
{
   esp_ble_command_state_e    state;
   unsigned char            * tx_data;
   unsigned char            * expected_data;
   unsigned int               number_of_attempt;
   unsigned int               timer;
}esp_ble_command_t;

typedef struct
{
   void                  * reference_obj;

   esp_ble_cb            * callback;
   esp_ble_at_state_e      state;
   esp_ble_at_state_e      next_state;
   esp_ble_command_t       command;
   esp_ble_parameters_t    parameter;

   unsigned int            delay_timer;

   queue_t                 rx_queue;
   unsigned int            rx_timer;
   queue_t                 tx_queue;
   unsigned char           tx_buffer[BLE_TX_BUFF_SIZE];
}esp_ble_t;

esp_ble_return_type_e esp_ble_create(esp_ble_t  * self,
                                     esp_ble_cb * callback,
                                     void       * reference_obj);
void esp_ble_1_ms_timer(esp_ble_t * self);
esp_ble_return_type_e esp_ble_polling(esp_ble_t * self);
void esp_set_ble_name(esp_ble_t * self, char * ble_name);
void esp_set_manufacturer_data(esp_ble_t * self, char * manufacturer_data);


#endif /* TCP_CLIENT_FIRMWARE_ESP_API_API_H_ */
