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

extern bool busyWait;
extern SemaphoreHandle_t moistureDetectionSemphr;
extern rtc_datetime_t RTC_1_dateTimeStruct;

void moistureDetection(void* pvParameters)
{
	for(;;)
	{
		// request time and print it
		RTC_GetDatetime(RTC, &RTC_1_dateTimeStruct );
		PRINTF("\rDate: %04hd-%02hd-%02hd Time: %02hd-%02hd-%02hd\r",
				RTC_1_dateTimeStruct.year,
				RTC_1_dateTimeStruct.month,
				RTC_1_dateTimeStruct.day,
				RTC_1_dateTimeStruct.hour,
				RTC_1_dateTimeStruct.minute,
				RTC_1_dateTimeStruct.second
				);

		// receive semaphore, indicate success, moisture detected
		if(xSemaphoreTake(moistureDetectionSemphr, 0)) {
			PRINTF("\n\rMoisture detected in moistureDetection task, requesting current datetime\n\r");
			// flash an LED to indicate moisture detection
			for(int i = 0; i < 6; i++) {
				GPIO_PortToggle(BOARD_MD_LED_GPIO, 1 << BOARD_MD_LED_PIN);
				vTaskDelay(pdMS_TO_TICKS(150));
			}

			// configure alarm
			uint32_t currSec = 0;
			currSec = RTC->TSR;
			currSec += 10;
			RTC->TAR = currSec;

			// display alarm time
			RTC_GetAlarm(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
			PRINTF("\n\rAlarm will occur at: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\n\r",
					RTC_1_dateTimeStruct.year,
					RTC_1_dateTimeStruct.month,
					RTC_1_dateTimeStruct.day,
				    RTC_1_dateTimeStruct.hour,
					RTC_1_dateTimeStruct.minute,
					RTC_1_dateTimeStruct.second
					);
		}	 // moistureDetectionSemphr

		// trigger alarm, flag is set by an alarm interrupt in RTC_1_COMMON_IRQHANDLER()
		if(busyWait)
		{
			busyWait = false;
			PRINTF("\n\n\r\033[31;1m************ ALARM ************** \033[0m\n\n\r");
		}

		vTaskDelay(pdMS_TO_TICKS(150));

	}	// end of for(;;)

}
