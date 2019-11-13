/*
 * moistureDetection.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */
#include "board.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "pin_mux.h"
#include "peripherals.h"
#include "fsl_rtc.h"
#include "helperFunctions.h"
#include "SEGGER_RTT.h"

extern bool busyWait;
extern SemaphoreHandle_t moistureDetectionSemphr;
extern rtc_datetime_t RTC_1_dateTimeStruct;

void mainTask(void* pvParameters)
{
	displayMenu();

	for(;;)
	{
		printCurrentTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);

		if(xSemaphoreTake(moistureDetectionSemphr, 0))
		{
			PRINTF("\n\r\033[34mMoisture detected\033[0m\n\r");
			// flash an LED to indicate moisture detection
			for(int i = 0; i < 6; i++) {
				GPIO_PortToggle(BOARD_MD_LED_GPIO, 1 << BOARD_MD_LED_PIN);
				vTaskDelay(pdMS_TO_TICKS(150));
			}

			configureAlarm(10);
			displayAlarmTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);

		}	 // moistureDetectionSemphr

		// trigger alarm, flag is set by an alarm interrupt in RTC_1_COMMON_IRQHANDLER()
		if(busyWait)
		{
			busyWait = false;
			PRINTF(RTT_CTRL_TEXT_BRIGHT_RED"************ ALARM **************"RTT_CTRL_RESET);
		}

		vTaskDelay(pdMS_TO_TICKS(150));

	}	// end of for(;;)
}
