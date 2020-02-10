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
extern QueueHandle_t recordsForThePhoneQ;
extern TaskHandle_t mainTaskHandle;

extern SemaphoreHandle_t recordsRequestSemphr;

void phoneBTTask(void *pvParameters)
{
	uint8_t charReceived = '\0';
	//char** recordsForPhoneBuffer = (char**) malloc(sizeof(char) * 300);
	char* recordsForPhoneBuffer[10] = {""};
	//char** recordsForPhoneBuffer;

///	TODO:	check if connected and receiving characters
/// TODO:	send data via queue to bluetooth task
	for(;;)
	{
		// status indicator
		if(GPIO_PinRead(BOARD_BT2_STATUS_GPIO, BOARD_BT2_STATUS_PIN) == 1)
			GPIO_PinWrite(BOARD_GREEN_LED_GPIO, BOARD_GREEN_LED_PIN, 0);
		else
			GPIO_PortToggle(BOARD_GREEN_LED_GPIO, 1 << BOARD_GREEN_LED_PIN);

		// handle requests from Bluetooth 2
		if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(UART3)) {
			charReceived = UART_ReadByte(UART3);
			switch(charReceived) {
				case DEV2_ALARM_STOP: 		xQueueSend(phoneBTReceiveQ, (void*)&charReceived, pdMS_TO_TICKS(0)); break;
				case SYSTEM_TIME_REQUEST: 	//PRINTF("\n\rTime: %s", getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											sendDataToPhone(getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											break;
				case SYSTEM_DATE_REQUEST: 	//PRINTF("\n\rDate: %s", getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											sendDataToPhone(getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct));
											break;
				case SYSTEM_TIME_CHANGE:	PRINTF("System time change\n\r"); break;
				case REQUEST_RECORDS:		PRINTF("\033[33mRequesting records...\033[0m\n\r");
											xSemaphoreGive(recordsRequestSemphr);
											if(xQueueReceive(recordsForThePhoneQ, &recordsForPhoneBuffer, pdMS_TO_TICKS(0)))
											{
												PRINTF("\033[32mData received!!!\033[0m\n\r");

												PRINTF("Printing data received!!!\n\r");
												sendDataToPhone("data data");
//												int i = 0;
//												while(recordsForPhoneBuffer[i] != NULL)
//												{
//													PRINTF("\n\r%s", recordsForPhoneBuffer[i]);
//													i++;
//												}

												int i = 0;
												PRINTF("\n\r\033[32mSending data to Phone!!!\033[0m\n\r");
												while(recordsForPhoneBuffer[i] != NULL)
												{
													sendDataToPhone(recordsForPhoneBuffer[i]);
													i++;
												}
												PRINTF("\n\r\033[32mData send!!!\033[0m\n\r");
											}
											break;
				default:					PRINTF("Invalid request\n\r"); charReceived = '\0'; break;
			}
			charReceived = '\0';
			UART_ClearStatusFlags(UART3,  kUART_RxDataRegFullFlag);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	} // for loop


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
/// @return void
void sendDataToPhone(char* data)
{
	//int i = 0;

	UART_WriteBlocking(UART3, &data, sizeof(data));
//	while(data[i] != '\0')
//	{
//		if (kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(UART3)) {
//			UART_WriteByte(UART3, data[i]);
//			i++;
//		}
//	}
}
