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
extern TaskHandle_t userTimeConfigHandle;
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
	const char* recordsAsStrings[10] = {};

	//struct eventData_t *p_event[] = &events;
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
			recordsAsStrings[eventCount] = convertRecordToString(events[eventCount]);

			for(int i = 0; i < eventCount; i++) {
				PRINTF("\n\rPrinting strings");
				PRINTF("\n\r%s", recordsAsStrings[i]);
			}

			eventCount++;

			// struct array set to 10 events only, this is a precaution
			if(eventCount > 9)
				eventCount = 0;

			// print available records
//			printRecords(events);


//			convertRecordToString(events);

			configureAlarm(20);
			displayAlarmTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
		}	 // moistureDetectionSemphr

		if(xSemaphoreTake(userTimeConfigSemphr, 0) == pdTRUE) {
		    if(xTaskCreate(userTimeConfig, "UserTimeConfig Task", configMINIMAL_STACK_SIZE + 150, NULL, 3, &userTimeConfigHandle) == pdFALSE)
		    {
		    	PRINTF("\r\nFailed to start \"UserTimeConfig Task\"\r\n");
		    }
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

	}	// end of for(;;)
}

/// @brief This function prints recorded event data
/// @param *p_event pointer to an array of structures holding the event data
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
const char* convertRecordToString(struct eventData_t event)
{
	char recordAsStrings[30];

	strcpy(recordAsStrings, event.eventDate);
	strcat(recordAsStrings, SPACER);
	strcat(recordAsStrings, event.eventTime);
	strcat(recordAsStrings, SPACER);
	strcat(recordAsStrings, event.wasAcknowledged);

	PRINTF("\n\r%s", recordAsStrings);

	return recordAsStrings;
}
