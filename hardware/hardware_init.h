/* **********************************************************************
 *	Hardware Header File
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

#ifndef HARDWARE_HARDWARE_H_
#define HARDWARE_HARDWARE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

typedef enum
{
	HARDWARE_FAIL,
	HARDWARE_SUCCESS,

	HARDWARE_INIT_FAIL,
	HARDWARE_INIT_SUCCESS,

	HARDWARE_UART_TX_FAIL,
	HARDWARE_UART_TX_SUCCESS,

	HARDWARE_GPIO_FAIL,
	HARDWARE_GPIO_SUCCESS,

	HARDWARE_HARD_FAIL
}hardware_return_type_e;



#define WIFI_BL_UART_TX_Pin GPIO_PIN_2
#define WIFI_BL_UART_TX_GPIO_Port GPIOA
#define WIFI_BL_UART_RX_Pin GPIO_PIN_3
#define WIFI_BL_UART_RX_GPIO_Port GPIOA
#define WIFI_MODULE_ENABLE_Pin GPIO_PIN_2
#define WIFI_MODULE_ENABLE_GPIO_Port GPIOB


hardware_return_type_e hardware_init(void);
hardware_return_type_e hardware_transmit_w_uart(const uint8_t *tx_data, uint16_t tx_data_length);
void hardware_gpio_enable(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void hardware_gpio_disable(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void SystemClock_Config(void);
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_HARDWARE_H_ */
