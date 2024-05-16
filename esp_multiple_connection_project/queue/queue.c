/* **********************************************************************
 *	Queue Source File
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

#include "queue.h"

/*
 * @Brief: Create queue function
 * */
queue_return_type_e queue_create(queue_t * self)
{
	if(!self) {return QUEUE_FAIL;}

	queue_clean_up(self);

	return QUEUE_SUCCESS;
}

/*
 * @Brief: Cleans the whole queue buffer.
 * */
queue_return_type_e queue_clean_up(queue_t * self)
{
	if(!self) {return QUEUE_FAIL;}

	self->head 	= 0;
	self->index 	= 0;
	self->tail 	= 0;

	for (int index = 0; index < QUEUE_SIZE; index++)
	{
		self->buffer[index] = '\0';
	}

	return QUEUE_SUCCESS;
}


/*
 * @Brief: Gives the info about the queue. If the queue is empty it returns 1 otherwise 0.
 * */
queue_size_return_type_e queue_is_empty(queue_t * self)
{
	if(self->head == self->tail) {return QUEUE_IS_EMPTY;}

	return QUEUE_IS_NOT_EMPTY;
}

/*
 * @Brief: Gives the info about the current queue size.
 * */
unsigned int queue_get_unreaden_size(queue_t * self)
{
	int unreaden_size = (int)self->head - self->tail;

	if(unreaden_size < 0)
	{
		unreaden_size = (int)self->head + (QUEUE_SIZE - self->tail);
	}

	if(self->head == self->tail) {return 0;}

	return unreaden_size;
}


/*
 * @Brief: Helps write data to queue buffer. This buffer is circular.
 * */
void queue_write_data(queue_t * self,
					  unsigned char* in_data,
					  unsigned int in_data_length)
{
	if(self == NULL || in_data == NULL || in_data_length < 1) {return;}

	do
	{
		self->buffer[self->head] = in_data[self->index];

		self->head++;

		if(self->head > (QUEUE_SIZE - 1)) {self->head = 0;}

		self->index++;
	}while(self->index < in_data_length);

	self->index = 0;
}

/*
 * @Brief: Helps read data from queue buffer.
 * */
void queue_read_data(queue_t* self,
					 unsigned char* out_data,
					 unsigned int out_data_length)
{
	if(self == NULL || out_data_length < 1) {return;}

	do
	{
		if(!queue_is_empty(self))
		{
			*(out_data + self->index) = self->buffer[self->tail];

			self->tail++;

			if(self->tail > (QUEUE_SIZE - 1)) {self->tail = 0;}

			self->index++;

			if (self->index > (QUEUE_SIZE - 1)) {
				self->index = 0;}
		}

		else
		{
			break;
		}

	}while(self->index < out_data_length);

	self->index = 0;
}
