/* **********************************************************************
 *	Main Header File
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


#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hardware_init.h"
#include "ble_app.h"

void main_systick_timer(void);
void main_uart_irq(unsigned char * rx_data, unsigned int rx_data_length);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
