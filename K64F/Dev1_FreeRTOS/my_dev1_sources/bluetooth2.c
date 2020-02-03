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

///	TODO:	check if connected and receiving characters
/// TODO:	send data via queue to bluetooth task
	for(;;)
	{
		if(GPIO_PinRead(BOARD_BT2_STATUS_GPIO, BOARD_BT2_STATUS_PIN) == 1)
			GPIO_PinWrite(BOARD_GREEN_LED_GPIO, BOARD_GREEN_LED_PIN, 0);
		else
			GPIO_PortToggle(BOARD_GREEN_LED_GPIO, 1 << BOARD_GREEN_LED_PIN);

		// handle requests from Bluetooth 2
		if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(UART3)) {
			charReceived = UART_ReadByte(UART3);
			switch(charReceived) {
				case DEV2_ALARM_STOP: 		xQueueSend(phoneBTReceiveQ, (void*)&charReceived, pdMS_TO_TICKS(0)); break;
				case SYSTEM_DATE_REQUEST: 	PRINTF("Date: %s", getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct), 9);
											sendDataToPhone(getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct), 9);
											break;
				case SYSTEM_TIME_REQUEST: 	PRINTF("Time: %s", getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct), 7); break;
											sendDataToPhone(getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct), 7);
											break;
				case SYSTEM_TIME_CHANGE:	PRINTF("System time change\n\r"); break;
				case REQUEST_RECORDS:		PRINTF("Request records\n\r"); break;
				default:					PRINTF("Invalid request\n\r"); charReceived = '\0'; break;
			}
			//if(charReceived == DEV2_ALARM_STOP || charReceived == SYSTEM_TIME_REQUEST) { // coming from the phone
//				int num = 12345;
//				char snum[6];
//				itoa(num, snum, 10);
//				PRINTF("%s", snum);
			//}
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

void sendDataToPhone(char* data, int arraySize) {
	int i = 0;
	char dataArray[arraySize];

	strcpy(dataArray, data);

	while(dataArray[i] != '\0')
	{
		UART_WriteByte(UART3, dataArray[i]);
		i++;
	}

//	for(int i = 0; i < sizeof(data); i++)
//	{
//		UART_WriteByte(UART3, data[i]);
//	}
}
