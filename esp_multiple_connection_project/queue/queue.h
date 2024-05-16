/* **********************************************************************
 *	Queue Header File
 *
 *
 *
 *
 *
 *	Date         : 13.01.2024
 *	Author       : Furkan YAYLA
 *	e-mail       : yaylafurkan@protonmail.com
 *
 * **********************************************************************
*/

#ifndef TCP_CLIENT_FIRMWARE_QUEUE_QUEUE_H_
#define TCP_CLIENT_FIRMWARE_QUEUE_QUEUE_H_

#include <stdio.h>
#include <string.h>

#define QUEUE_SIZE	500

typedef enum
{
	QUEUE_FAIL,
	QUEUE_SUCCESS
}queue_return_type_e;

typedef enum
{
	QUEUE_IS_NOT_EMPTY,
	QUEUE_IS_EMPTY
}queue_size_return_type_e;

typedef struct
{
	unsigned int 	head;
	unsigned int 	tail;
	unsigned int 	index;
	unsigned char	buffer[QUEUE_SIZE];
}queue_t;


queue_return_type_e queue_create(queue_t * self);
queue_return_type_e queue_clean_up(queue_t * self);
queue_size_return_type_e queue_is_empty(queue_t * self);
unsigned int queue_get_unreaden_size(queue_t * self);
void queue_write_data(queue_t * self,
					  unsigned char* in_data,
					  unsigned int in_data_length);
void queue_read_data(queue_t * self,
					 unsigned char* out_data,
					 unsigned int out_data_length);

#endif /* TCP_CLIENT_FIRMWARE_QUEUE_QUEUE_H_ */
