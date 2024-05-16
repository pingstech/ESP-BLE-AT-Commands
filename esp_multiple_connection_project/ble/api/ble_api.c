/* ****************************************************************************
 * @Project Name : ESP BLE Operation
 *
 *
 *      @Purpose :ESP BLE API Source File
 *      @Date    : 15.02.2024
 *      @Author  : Furkan YAYLA
 *      @e-mail  : furkan.yayla@saykal.com
 *
 * ****************************************************************************
 **/


#include "ble_api.h"

/* ************************************************************************** */
static esp_ble_return_type_e esp_ble_command_manager(esp_ble_t    * self,
                                                     char         * command,
                                                     char         * expected_response,
                                                     unsigned int   attempt_number,
                                                     unsigned int   timeout);
/* ************************************************************************** */
static void esp_ble_state_delay(esp_ble_t * self, esp_ble_at_state_e next_state);
/* ************************************************************************** */
static unsigned int length_calculator(const char * string);
/* ************************************************************************** */
static int string_to_int_converter(const char *string) ;
/* ************************************************************************** */
static void esp_ble_receive_handler(esp_ble_t * self);
/* ************************************************************************** */
static void esp_ble_transmit_handler(esp_ble_t * self);
/* ************************************************************************** */

esp_ble_return_type_e esp_ble_create(esp_ble_t  * self,
                                     esp_ble_cb * callback,
                                     void       * reference_obj)
{
   if(!self || !reference_obj) {return ESP_BLE_FAIL;}

   self->callback = (esp_ble_cb *)callback;
   self->reference_obj = reference_obj;

   queue_create(&self->tx_queue);
   queue_create(&self->rx_queue);
   self->rx_timer = BLE_RESET_INT_VALUE;

   self->state = AT_INIT;
   self->next_state = AT_INIT;

   self->command.state = BLE_COMMAND_TRANSMITTING;

   return ESP_BLE_CREATE_SUCCESS;
}

void esp_ble_1_ms_timer(esp_ble_t * self)
{
   if(!self) {return;}

   if(self->command.state == BLE_COMMAND_RESPONSE_RECEIVING)
   {
      self->command.timer++;
   }

   if(self->state == BLE_DELAY_STATE)
   {
      self->delay_timer++;
   }

   self->rx_timer++;
}

void esp_set_ble_name(esp_ble_t * self, char * ble_name)
{
	snprintf((char *)self->parameter.ble_name, sizeof(self->parameter.ble_name), ble_name);
}

void esp_set_manufacturer_data(esp_ble_t * self, char * manufacturer_data)
{
	snprintf((char *)self->parameter.manufacturer_data, sizeof(self->parameter.manufacturer_data), manufacturer_data);
}

esp_ble_return_type_e esp_ble_polling(esp_ble_t * self)
{
  if(!self) {return ESP_BLE_FAIL;}

  switch (self->state)
  {
    case AT_INIT:{
       esp_ble_return_type_e f_return = esp_ble_command_manager(self,
                                                                (char *)"AT+BLEINIT=2\r\n",
                                                                (char *)"OK\r\n",
                                                                3, 3000);

       if(f_return == ESP_BLE_SUCCESS)
       {
          esp_ble_state_delay(self, AT_ADVDATAEX);
       }
       else if (f_return == ESP_BLE_HARD_FAIL)
       {
          self->state = BLE_RESET_STATE;
          return ESP_BLE_INIT_FAIL;
       }
    break;}

    case AT_ADVDATAEX:{
       if(self->parameter.ble_name[0] == '\0' || self->parameter.manufacturer_data[0] == '\0')
       {
          return ESP_BLE_PARAMETERS_MISSING;
       }

       unsigned char advdastaex_buffer[BLE_XL_BUFF_SIZE] = {0};

       snprintf((char *)advdastaex_buffer,
                sizeof(advdastaex_buffer),
                "AT+BLEADVDATAEX=\"%s\",\"2A00\",\"%s\",0\r\n",
                self->parameter.ble_name,
                self->parameter.manufacturer_data);

       esp_ble_return_type_e f_return = esp_ble_command_manager(self,
                                                                (char *)advdastaex_buffer,
                                                                (char *)"OK\r\n",
                                                                3, 3000);

       if(f_return == ESP_BLE_SUCCESS)
       {
          esp_ble_state_delay(self, AT_BLEGATTSSRVCRE);
       }
       else if (f_return == ESP_BLE_HARD_FAIL)
       {
          self->state = BLE_RESET_STATE;
          return ESP_BLE_ADVDATAEX_FAIL;
       }
    break;}

    case AT_BLEGATTSSRVCRE:{
       esp_ble_return_type_e f_return = esp_ble_command_manager(self,
                                                                (char *)"AT+BLEGATTSSRVCRE\r\n",
                                                                (char *)"OK\r\n",
                                                                3, 3000);

       if(f_return == ESP_BLE_SUCCESS)
       {
          esp_ble_state_delay(self, AT_BLEGATTSSRVSTART);
       }
       else if (f_return == ESP_BLE_HARD_FAIL)
       {
          self->state = BLE_RESET_STATE;
          return ESP_BLE_BLEGATTSSRVCRE_FAIL;
       }
    break;}

    case AT_BLEGATTSSRVSTART:{
       esp_ble_return_type_e f_return = esp_ble_command_manager(self,
                                                                (char *)"AT+BLEGATTSSRVSTART\r\n",
                                                                (char *)"OK\r\n",
                                                                3, 3000);

       if(f_return == ESP_BLE_SUCCESS)
       {
          esp_ble_state_delay(self, AT_BLEADVSTART);
       }
       else if (f_return == ESP_BLE_HARD_FAIL)
       {
          self->state = BLE_RESET_STATE;
          return ESP_BLE_BLEGATTSSRVSTART_FAIL;
       }
    break;}

    case AT_BLEADVSTART:{
       esp_ble_return_type_e f_return = esp_ble_command_manager(self,
                                                                (char *)"AT+BLEADVSTART\r\n",
                                                                (char *)"OK\r\n",
                                                                3, 3000);

       if(f_return == ESP_BLE_SUCCESS)
       {
          self->callback->ble_device_detectable(self, self->reference_obj);

          esp_ble_state_delay(self, BLE_LOOP_STATE);
       }
       else if (f_return == ESP_BLE_HARD_FAIL)
       {
          self->state = BLE_RESET_STATE;
          return ESP_BLE_BLEADVSTART_FAIL;
       }
    break;}

    case AT_BLEGATTSIND:{
       unsigned char blegattsind_buffer[BLE_XL_BUFF_SIZE] = {0};

       snprintf((char *)blegattsind_buffer,
                sizeof(blegattsind_buffer),
                "AT+BLEGATTSIND=0,1,7,%d\r\n",
                length_calculator((char *)self->tx_buffer));

       esp_ble_return_type_e f_return = esp_ble_command_manager(self,
                                                                (char *)blegattsind_buffer,
                                                                (char *)">",
                                                                3, 3000);

       if(f_return == ESP_BLE_SUCCESS)
       {
          self->state = BLE_TRANSMIT_STATE;
       }
       else if (f_return == ESP_BLE_HARD_FAIL)
       {
          self->state = BLE_RESET_STATE;
          return ESP_BLE_BLEGATTSIND_FAIL;
       }
    break;}

    case BLE_LOOP_STATE:{
       if(!queue_is_empty(&self->rx_queue) && self->rx_timer > BLE_UART_SYNC_TIMEOUT)
       {
          self->rx_timer = 0;
          esp_ble_receive_handler(self);
       }

       if(!queue_is_empty(&self->tx_queue))
       {
          esp_ble_transmit_handler(self);
       }
    break;}

    case BLE_TRANSMIT_STATE:{
       esp_ble_return_type_e f_return = esp_ble_command_manager(self,
                                                                (char *)self->tx_buffer,
                                                                (char *)"OK\r\n",
                                                                3, 3000);

       if(f_return == ESP_BLE_SUCCESS)
       {
    	   esp_ble_state_delay(self, BLE_LOOP_STATE);
       }
       else if (f_return == ESP_BLE_HARD_FAIL)
       {
          self->state = BLE_RESET_STATE;
          return ESP_BLE_TRANSMIT_FAIL;
       }
    break;}

    case BLE_DELAY_STATE:{
       if(self->delay_timer >= BLE_STATE_DELAY_TIMEOUT)
       {
          self->state = self->next_state;
       }
    break;}

    case BLE_RESET_STATE:{
       self->callback->ble_reset_waiting(self, self->reference_obj);
    break;}
  }

    return ESP_BLE_FAIL;
}


/*
 * @Brief: Function for sending ESP BLE commands.
 * @Param: Pointer to ESP BLE object.
 * @Param: Command to be sent.
 * @Param: Expected response from ESP.
 * @Param: Timeout duration.
 * @Param: Number of attempts.
 * @Return: ESP_BLE_SUCCESS if successful, ESP_BLE_FAIL otherwise.
 * */
static esp_ble_return_type_e esp_ble_command_manager(esp_ble_t    * self,
                                                     char         * command,
                                                     char         * expected_response,
                                                     unsigned int   attempt_number,
                                                     unsigned int   timeout)
{

   if(!self                 ||
      !command              ||
      !expected_response    ||
      attempt_number <= 0   ||
      timeout <= 0)
   {
      return ESP_BLE_HARD_FAIL;
   }

   switch (self->command.state)
   {
      case BLE_COMMAND_TRANSMITTING:{
         self->command.timer = BLE_RESET_INT_VALUE;
         self->command.tx_data = (unsigned char *)command;
         self->command.number_of_attempt++;
         self->command.expected_data = (unsigned char *)expected_response;
         self->command.state = BLE_COMMAND_RESPONSE_RECEIVING;

         self->callback->ble_transmit_data(self, self->reference_obj,
                                           (unsigned char *)command,
										   length_calculator((char *)command));
      break;}

      case BLE_COMMAND_RESPONSE_RECEIVING:{
         if(!queue_is_empty(&self->rx_queue) && self->rx_timer >= BLE_UART_SYNC_TIMEOUT && self->command.timer <= timeout)
         {
        	self->rx_timer = 0;

            unsigned char tx_buffer[BLE_TX_BUFF_SIZE] = {0};

            queue_read_data(&self->rx_queue, tx_buffer, queue_get_unreaden_size(&self->rx_queue));

            if(strstr((char *)tx_buffer, expected_response))
            {
               self->command.state = BLE_COMMAND_TRANSMITTING;
               self->command.number_of_attempt = BLE_RESET_INT_VALUE;
               return ESP_BLE_SUCCESS;
            }
         }
         else if(self->command.timer > timeout)
         {
            self->command.state = BLE_COMMAND_ATTEMPT_CONTROL;
         }
      break;}

      case BLE_COMMAND_ATTEMPT_CONTROL:{
         if(self->command.number_of_attempt >= attempt_number)
         {
            return ESP_BLE_HARD_FAIL;
         }

         self->command.state = BLE_COMMAND_TRANSMITTING;
      break;}
   }

   return ESP_BLE_FAIL;
}

/*
 * @Brief: Creating delay for the next step
 * @Param: ESP API object
 * @Param: Next AT state
 * */
static void esp_ble_state_delay(esp_ble_t * self, esp_ble_at_state_e next_state)
{
  if(!self) {return;}

  self->state = BLE_DELAY_STATE;
  self->next_state = next_state;

  self->delay_timer = BLE_RESET_INT_VALUE;
}

/*
 * @Brief: Calculating the length of string
 * */
static unsigned int length_calculator(const char * string)
{
  int index = 0;

  while (*string)
  {
      index++;
      string++;
  }

  return index;
}

/*
 * @Brief: String to integer converter
 * */
static int string_to_int_converter(const char * string)
{
    int result = 0; int flag = 0;
    while (*string) {
        if (*string >= '0' && *string <= '9') {
            result = result * 10 + (*string - '0');
            flag = 1;
        }
        else{
            if(flag == 1){break;}
        }
        string++;
    }
    return result;
}

/*
 * @Brief: ESP received data handler
 * */
static void esp_ble_receive_handler(esp_ble_t * self)
{
   if(!self) {return;}

   unsigned char rx_buffer[BLE_TX_BUFF_SIZE] = {0};

   queue_read_data(&self->rx_queue, rx_buffer, queue_get_unreaden_size(&self->rx_queue));

   if(strstr((char *)rx_buffer, "+WRITE:"))
   {
      unsigned int comma_counter = BLE_RESET_INT_VALUE;
      unsigned int index = BLE_RESET_INT_VALUE;
      unsigned int size = BLE_RESET_INT_VALUE;
      char * hold = NULL;

      char * end_ptr = strstr((char *)rx_buffer, "\r\n");
      if(end_ptr != NULL)
      {
    	  *end_ptr = '\0';
      }

      while(comma_counter <= 3 && index < BLE_MAX_RX_DATA_LENGTH)
      {
         if(rx_buffer[index] == ',')
         {
            rx_buffer[index] = 'x';
            comma_counter++;

            if(comma_counter == 3)
            {
				hold = strstr((char *)rx_buffer, ",");
				size = string_to_int_converter(hold);
            }
         }
         index++;
      }

      if(comma_counter == 4 && hold)
      {
         char * hold2 = strstr(hold, ",");
         if(hold2)
         {
            self->callback->ble_received_data(self, self->reference_obj, (unsigned char *)hold2 + 1, size);
            return;
         }
      }
   }

   else if(strstr((char *)rx_buffer, "+BLECONN:"))
   {
      self->callback->ble_connection_success(self, self->reference_obj);
   }

   else if(strstr((char *)rx_buffer, "+BLEDISCONN:"))
   {
	  self->state = AT_BLEADVSTART;
      self->callback->ble_connection_fail(self, self->reference_obj);
   }
}

/*
 * @Brief: MCU to ESP transmit handler
 * */
static void esp_ble_transmit_handler(esp_ble_t * self)
{
   if(!self) {return;}

   memset((char *)self->tx_buffer, 0, sizeof(self->tx_buffer));
   unsigned int index = BLE_RESET_INT_VALUE;
   unsigned char data;

   do
   {
	  queue_read_data(&self->tx_queue, (unsigned char *)&data, 1);
      self->tx_buffer[index] = data;

      if(index >= 19 || (self->tx_buffer[index] == '\n' && self->tx_buffer[index -1] == '\r'))
      {
         self->state = AT_BLEGATTSIND;

         return;
      }

      index++;
   }while(!queue_is_empty(&self->tx_queue));
}
