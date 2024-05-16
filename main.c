/* **********************************************************************
 *	Main Source File
 *
 *
 *	Project Name : ESP TCP client connection driver
 *
 *
 *	Date         : 13.01.2024
 *	Author       : Furkan YAYLA
 *	e-mail       : yaylafurkan@protonmail.com
 *
 * **********************************************************************
*/


#include "main.h"

void main_systick_timer(void)
{
	esp_ble_app_1_ms_timer();
}

int main(void)
{
	hardware_init();

	HAL_Delay(1000);

	esp_ble_app_init();

  while (1)
  {
	  esp_ble_app_polling();
  }
}

void main_uart_irq(unsigned char * rx_data, unsigned int rx_data_length)
{
	esp_ble_get_received_data(rx_data, rx_data_length);
}
