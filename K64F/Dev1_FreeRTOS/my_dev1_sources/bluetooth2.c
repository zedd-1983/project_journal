/*
 * bluetooth2.c
 *
 *  Created on: 20 Jan 2020
 *      Author: zedd
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_uart.h"
#include "fsl_debug_console.h"
#include "queue.h"
#include "bluetooth2.h"
#include "peripherals.h"
#include "helperFunctions.h"

extern QueueHandle_t phoneBTReceiveQ;
extern QueueHandle_t dataForThePhoneQ;

void phoneBTTask(void *pvParameters)
{
	uint8_t charReceived = '\0';
	char* data = "";

///	TODO:	check if connected and receiving characters
/// TODO:	send data via queue to bluetooth task
	for(;;)
	{
		if(GPIO_PinRead(BOARD_BT2_STATUS_GPIO, BOARD_BT2_STATUS_PIN) == 1)
			GPIO_PinWrite(BOARD_GREEN_LED_GPIO, BOARD_GREEN_LED_PIN, 0);
		else
			GPIO_PortToggle(BOARD_GREEN_LED_GPIO, 1 << BOARD_GREEN_LED_PIN);

//		int i = 0;
//		if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(UART3)) {
//			data[i] = UART_ReadByte(UART3);
//			i++;
//		}
//
//		for(int i = 0; i < strlen(data); i++)
//			PRINTF("%c", data[i]);

		// handle requests from Bluetooth 2
		if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(UART3)) {
			charReceived = UART_ReadByte(UART3);
			switch(charReceived) {
				case DEV2_ALARM_STOP: 		xQueueSend(phoneBTReceiveQ, (void*)&charReceived, pdMS_TO_TICKS(0)); break;
				case SYSTEM_TIME_REQUEST: 	PRINTF("\n\rTime: %s", getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											sendDataToPhone(getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											break;
				case SYSTEM_DATE_REQUEST: 	PRINTF("\n\rDate: %s", getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											sendDataToPhone(getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											break;
				case SYSTEM_TIME_CHANGE:	PRINTF("System time change\n\r"); break;
				case REQUEST_RECORDS:		PRINTF("Request records\n\r"); break;
				default:					PRINTF("Invalid request\n\r"); charReceived = '\0'; break;
			}
			UART_ClearStatusFlags(UART3,  kUART_RxDataRegFullFlag << 0);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}


/// TODO: 	if event happens, this task needs to be notified (an LED goes ON)
/// TODO: 	need to be able to request time and date of previous events from mald ster device
///			(these will probably be recorded in an array of strings at first)
///			(should include time and date, ACK status, maybe length and type of wake-up method used)
/// TODO: 	need to be able to stop pending alarm if event occurs, event still needs to be
///			recorded
/// TODO: 	allow for setting time and date through the phone application rather than through LCD and
/// 		keypad (maybe)
}

/// @brief This function sends data to phone via UART3 and Bluetooth module
/// @param data string representation of data
void sendDataToPhone(char* data)
{
	int i = 0;
	while(data[i] != '\0')
	{
		if (kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(UART3)) {
			UART_WriteByte(UART3, data[i]);
			i++;
		}
	}
}
