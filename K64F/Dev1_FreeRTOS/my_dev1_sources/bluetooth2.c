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
extern QueueHandle_t singleRecordQueue;
extern TaskHandle_t mainTaskHandle;

extern SemaphoreHandle_t recordsRequestSemphr;
extern SemaphoreHandle_t timeChangeRequestSemphr;
extern SemaphoreHandle_t configureTimeViaPhoneSemphr;

void phoneBTTask(void *pvParameters)
{
	uint8_t charReceived = '\0';
	//char** recordsForPhoneBuffer = (char**) malloc(sizeof(char) * 300);
	char* recordsForPhoneBuffer[10] = {""};
	char* oneRecord = "";
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
				case SYSTEM_TIME_CHANGE:	PRINTF("System time change\n\r");
											charReceived = '\0';
											xSemaphoreGive(configureTimeViaPhoneSemphr);
//											sendDataToPhone("Insert new date: ");
//											char* newData = getDataFromPhone();
//											PRINTF("\n\r%s", newData);
											//xSemaphoreGive(timeChangeRequestSemphr);
											break;
				case REQUEST_RECORDS:		PRINTF("\n\r\033[33mRequesting records...\033[0m\n\r");
											xSemaphoreGive(recordsRequestSemphr);

//											if(xQueueReceive(singleRecordQueue, &oneRecord, pdMS_TO_TICKS(200)))
//											{
//												PRINTF("\n\rReceived: %s", oneRecord);
//												//PRINTF("\n\rSending data to phone: ");
//												sendDataToPhone(oneRecord);
//											}


											if(xQueueReceive(recordsForThePhoneQ, &recordsForPhoneBuffer, pdMS_TO_TICKS(200)))
											{
												PRINTF("\033[32mData received!!!\033[0m\n\r");

												//PRINTF("Printing data received!!!\n\r");
												int i = 0;
//												while(recordsForPhoneBuffer[i] != NULL)
//												{
//													PRINTF("\n\rData in the buffer");
//													PRINTF("\n\r%s", recordsForPhoneBuffer[i]);
//													i++;
//												}
//
//												i = 0;
												PRINTF("\n\r\033[32mSending data to Phone!!!\033[0m\n\r");
												while(recordsForPhoneBuffer[i] != 0x0)
												{
													sendDataToPhone(recordsForPhoneBuffer[i]);
													busyDelay(20000);
													i++;
												}
												PRINTF("\n\r\033[32mData send!!!\033[0m\n\r");
											}

											break;
				default:					PRINTF("Invalid request\n\r"); charReceived = '\0'; break;
			} // switch
			charReceived = '\0';
			UART_ClearStatusFlags(UART3,  kUART_RxDataRegFullFlag);
		} // handling requests from phone
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
} // end of phoneBTTask

/// @brief This function sends data to phone via UART3 and Bluetooth module
/// @param data string representation of data
/// @return void
void sendDataToPhone(char* data)
{
	int i = 0;
	int count = 0;

	UART_WriteBlocking(UART3, (void*)data, strlen(data) + sizeof(char));
	PRINTF("\n\rSent: %s", data);

	/*
	while(data[i] != 0x0)
	{
		//PRINTF("%c", data[i]);
		count++;

		UART_WriteByte(UART3, data[i]);
		// this seems to work, looks like I was sending data too fast
		// will need to be replaced by a proper interrupt using PIT
		busyDelay(20000);
		i++;
	}
	*/

	//PRINTF("\n\rCount: %d", count);
	//UART_WriteBlocking(UART3, (void*)data, sizeof(data[21]));
//	while(data[i] != '\0')
//	{
//		if (kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(UART3)) {
//			UART_WriteByte(UART3, data[i]);
//		}
//		i++;
//	}null
}

void busyDelay(int noOfLoops)
{
	for(int i = 0; i < noOfLoops; i++);
}

char* getDataFromPhone() {
	//int i = 0;
	char* receivedData = "";

	UART_ReadBlocking(UART3, (uint8_t*)&receivedData, sizeof(receivedData));

	PRINTF("\n\r%s", receivedData);
	// if there are data in the receive register
//	if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(UART3))
//	{
//		while(receivedData[i] != '\0') {
//			receivedData[i] = UART_ReadByte(UART3);
//			i++;
//		}
//	}
	return receivedData;
}
