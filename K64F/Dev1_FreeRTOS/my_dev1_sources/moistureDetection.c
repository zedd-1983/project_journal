/*
 * moistureDetection.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */

// FreeRTOS
#include "FreeRTOS.h"
#include "semphr.h"

// K64F
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "peripherals.h"
#include "fsl_rtc.h"

// My headers
#include "helperFunctions.h"
#include "timeKeeping.h"
#include "bluetooth.h"
#include "SEGGER_RTT.h"
#include "moistureDetection.h"

extern bool busyWait;

extern SemaphoreHandle_t moistureDetectionSemphr;
extern SemaphoreHandle_t userTimeConfigSemphr;
extern SemaphoreHandle_t btSemphr;
extern SemaphoreHandle_t recordsRequestSemphr;
extern SemaphoreHandle_t timeChangeRequestSemphr;

extern TaskHandle_t userTimeConfigHandle;
//extern TaskHandle_t phoneTimeConfigHandle;

extern QueueHandle_t recordsForThePhoneQ;
extern QueueHandle_t singleRecordQueue;

extern rtc_datetime_t RTC_1_dateTimeStruct;
static uint8_t eventCount = 0;


#define SPACER "   "

/// @brief FreeRTOS main task
/// @details this task displays menu and if it receives semaphore from SW3 creates
/// another task for configuration of date and time. If moisture is detected (SW2)
/// semaphore is given from an interrupt to the main task to configure the alarm
/// (currently set to 10s)
void mainTask(void* pvParameters)
{
	//displayMenu();
	struct eventData_t events[10] = {};
	char* recordsAsStrings[10] = {""};
	char* singleString = "";

	for(;;)
	{
		//printCurrentTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);

		// check BT status
		if(GPIO_PinRead(BOARD_BT1_STATUS_GPIO, BOARD_BT1_STATUS_PIN) == 1)
			GPIO_PinWrite(BOARD_BLUE_LED_GPIO, BOARD_BLUE_LED_PIN, 0);
		else
			GPIO_PortToggle(BOARD_BLUE_LED_GPIO, 1 << BOARD_BLUE_LED_PIN);
			//GPIO_PinWrite(BOARD_BLUE_LED_GPIO, BOARD_BLUE_LED_PIN, 1);

		// check BT for incoming character
//		if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(BLUETOOTH_PERIPHERAL)) {
//			uint8_t btChar = UART_ReadByte(BLUETOOTH_PERIPHERAL);
//			xTaskCreate(btTask, "BT task", configMINIMAL_STACK_SIZE + 50, &btChar, 5, NULL);
//		}

		if(xSemaphoreTake(moistureDetectionSemphr, 0))
		{
			PRINTF("\n\r\033[34mMoisture detected\033[0m\n\r");
			// flash an LED to indicate moisture detection
			for(int i = 0; i < 6; i++) {
				GPIO_PortToggle(BOARD_MD_LED_GPIO, 1 << BOARD_MD_LED_PIN);
				vTaskDelay(pdMS_TO_TICKS(150));
			}

			// store the event time in a struct array
			events[eventCount].eventTime = getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
			events[eventCount].eventDate = getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
			events[eventCount].wasAcknowledged = "no";

			// convert given structure to a single string and add it to a string array
			recordsAsStrings[eventCount] = convertRecordToString(events[eventCount]);

			// testing - just getting one string to be passed to the queue and
			// sent to the phone
			singleString = recordsAsStrings[0];

			eventCount++;

			// struct array set to 10 events only, this is a precaution
			if(eventCount > 9)
				eventCount = 9;

			configureAlarm(20);
			displayAlarmTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
		}	 // moistureDetectionSemphr

		// time change via terminal
		if(xSemaphoreTake(userTimeConfigSemphr, 0) == pdTRUE) {
		    if(xTaskCreate(userTimeConfig, "UserTimeConfig Task", configMINIMAL_STACK_SIZE + 150, NULL, 3, &userTimeConfigHandle) == pdFALSE)
		    {
		    	PRINTF("\r\nFailed to start \"UserTimeConfig Task\"\r\n");
		    }
		}

		// time change via phone
//		if(xSemaphoreTake(timeChangeRequestSemphr, 0) == pdTRUE) {
//		    if(xTaskCreate(timeConfigFromPhone, "Phone time config", configMINIMAL_STACK_SIZE + 150, NULL, 3, &phoneTimeConfigHandle) == pdFALSE)
//		    {
//		    	PRINTF("\r\nFailed to start \"Phone Time Config Task\"\r\n");
//		    }
//		}

		// print available records when a request is received from the phone
		// will send data over a Queue to BT2 task to be transmitted back to the phone
		if(xSemaphoreTake(recordsRequestSemphr, 0) == pdTRUE)
		{
			// send only one string
			PRINTF("\n\rThe ONE string: %s", singleString);

			// put the One string into a queue and transfer to bt2 task
			xQueueSend(singleRecordQueue, &singleString, 0);

/*
			PRINTF("\n\rPrinting strings");
			for(int i = 0; i <= eventCount; i++) {
				PRINTF("\n\r%s", recordsAsStrings[i]);
			}
			if(recordsForThePhoneQ != 0) {
			// send records to the BT2 task if there are items in the queue
				if(xQueueSend(recordsForThePhoneQ, &singleString, 0) != pdPASS)
					PRINTF("\n\rFailed to send the queue");
			}

*/

		}

		// trigger alarm, flag is set by an alarm interrupt in RTC_1_COMMON_IRQHANDLER()
//		if(xSemaphoreTake(btSemphr, 0) == pdTRUE) {
//			xTaskCreate(btTask, "BT task", configMINIMAL_STACK_SIZE + 50, NULL, 5, NULL);
//		}



//		if(busyWait)
//		{
//			busyWait = false;
//
//			PRINTF(RTT_CTRL_TEXT_BRIGHT_RED"\n\r************ ALARM **************\n\r"RTT_CTRL_RESET);
//		}

		vTaskDelay(pdMS_TO_TICKS(150));
		//	while(data[i] != '\0')
		//	{
		//		if (kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(UART3)) {
		//			UART_WriteByte(UART3, data[i]);
		//		}
		//		i++;
		//	}

	}	// end of for(;;)
}

/// @brief This function prints recorded event data
/// @param *p_event pointer to an array of structures holding the event data
/// @return void
void printRecords(struct eventData_t *p_event)
{
	for(int i = 0; i < eventCount; i++) {
		PRINTF("\n\rEvent no %d: %s \t%s \t%s",
				i+1,
				p_event->eventDate,
				p_event->eventTime,
				p_event->wasAcknowledged
				);
				p_event++;
	}
}

/// @brief Function converts given structure to a String and returns
/// @param event structure to convert
/// @return char* recordAsString
char* convertRecordToString(struct eventData_t event)
{
	// alocate memory for a string
	char* recordAsStrings = (char*) malloc(sizeof(char) * 21);
	//char* recordAsStrings = "";

	strcpy(recordAsStrings, event.eventDate);
	strcat(recordAsStrings, SPACER);
	strcat(recordAsStrings, event.eventTime);
	strcat(recordAsStrings, SPACER);
	strcat(recordAsStrings, event.wasAcknowledged);

//	PRINTF("\n\r%s", recordAsStrings);

	return recordAsStrings;
}
