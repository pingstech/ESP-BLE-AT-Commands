/* ****************************************************************************
 * @Project Name : ESP BLE Operation
 *
 *
 *      @Purpose :ESP BLE APP Source File
 *      @Date    : 15.02.2024
 *      @Author  : Furkan YAYLA
 *      @e-mail  : furkan.yayla@saykal.com
 *
 * ****************************************************************************
 **/

#include "ble_app.h"

static void device_detectable_fp	(void * ble_obj, void * reference_obj);
static void ble_connection_success_fp	(void * ble_obj, void * reference_obj);
static void ble_connection_fail_fp	(void * ble_obj, void * reference_obj);
static void ble_reset_waiting_fp	(void * ble_obj, void * reference_obj);
static void ble_transmit_data_fp	(void * ble_obj, void * reference_obj, const unsigned char * tx_data, unsigned int tx_data_length);
static void ble_received_data_fp	(void * ble_obj, void * reference_obj, const unsigned char * rx_data, unsigned int rx_data_length);
/* ---------------------------------------------------------------------------------------------------------------------------------------------- */

unsigned char conn_flag = 0;
unsigned char conn_success = 0;

static esp_ble_cb _ble_cb=
{
   &device_detectable_fp,
   &ble_connection_success_fp,
   &ble_connection_fail_fp,
   &ble_reset_waiting_fp,
   &ble_transmit_data_fp,
   &ble_received_data_fp,
};

static esp_ble_app_t _ble_app;

void esp_ble_app_init(void)
{
   hardware_gpio_enable(GPIOB, GPIO_PIN_2);

   queue_create(&_ble_app.queue);

   esp_ble_create(&_ble_app._ble, &_ble_cb, &_ble_app);

   esp_set_ble_name(&_ble_app._ble, "SYKL_BLE");
   esp_set_manufacturer_data(&_ble_app._ble, "123456");
}

void esp_ble_app_1_ms_timer(void)
{
   esp_ble_1_ms_timer(&_ble_app._ble);

   _ble_app.tx_timer++;
}

static unsigned int counter = 0;

void esp_ble_app_polling(void)
{
   esp_ble_polling(&_ble_app._ble);

   if(_ble_app.flag.connection == 1 && _ble_app.tx_timer >= SEND_BLE_MESSAGE_TIMEOUT)
   {
      _ble_app.tx_timer = 0;
      
      counter++;
      
      unsigned char test_text[255] = {0};
      
      snprintf((char *)test_text,
      sizeof(test_text),
      "The journey of life is filled with twists and turns, leading us down paths unknown. In the depths of uncertainty, we find strength to face the challenges ahead.Test number %d\r\n",
      counter);

      queue_write_data(&_ble_app._ble.tx_queue, test_text, strlen((char *)test_text));
   }
}

void esp_ble_get_received_data(unsigned char * rx_data, unsigned int rx_data_length)
{
   queue_write_data(&_ble_app._ble.rx_queue, (unsigned char *)rx_data, rx_data_length);

   _ble_app._ble.rx_timer = BLE_RESET_INT_VALUE;
}


static void device_detectable_fp(void * ble_obj, void * reference_obj)
{

}
static void ble_connection_success_fp(void * ble_obj, void * reference_obj)
{
   _ble_app.tx_timer = 0;
   _ble_app.flag.connection = 1;
}
static void ble_connection_fail_fp(void * ble_obj, void * reference_obj)
{
   _ble_app.flag.connection = 0;
}
static void ble_reset_waiting_fp(void * ble_obj, void * reference_obj)
{

}
static void ble_transmit_data_fp(void * ble_obj, void * reference_obj, const unsigned char * tx_data, unsigned int tx_data_length)
{
   hardware_transmit_w_uart(tx_data, tx_data_length);
}
static void ble_received_data_fp(void * ble_obj, void * reference_obj, const unsigned char * rx_data, unsigned int rx_data_length)
{
   queue_write_data(&_ble_app.queue, (unsigned char *)rx_data, rx_data_length);
}
