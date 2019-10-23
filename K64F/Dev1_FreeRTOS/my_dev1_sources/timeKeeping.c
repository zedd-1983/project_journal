/*
 * timeKeeping.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */

#include "fsl_debug_console.h" // to allow for PRINTF
#include "FreeRTOS.h"
#include "semphr.h"
#include "peripherals.h"

extern SemaphoreHandle_t secondsSemphr;
extern SemaphoreHandle_t setAlarmSemphr;
extern rtc_datetime_t RTC_1_dateTimeStruct;

void timeKeeping(void* pvParameters)
{
	//rtc_datetime_t RTC_1_alarmTimeStruct;
	uint32_t currSec = 0;
	//uint32_t alarmSec = 0;
	PRINTF("\r\nTime Keeping\r\n");
	for(;;)
	{
		RTC_GetDatetime(RTC, &RTC_1_dateTimeStruct);
		PRINTF("\rDate: %04hd-%02hd-%02hd Time: %02hd-%02hd-%02hd\r",
				RTC_1_dateTimeStruct.year,
				RTC_1_dateTimeStruct.month,
				RTC_1_dateTimeStruct.day,
				RTC_1_dateTimeStruct.hour,
				RTC_1_dateTimeStruct.minute,
				RTC_1_dateTimeStruct.second
				);

//		if(xSemaphoreTake(setAlarmSemphr, 0))
//		{
//			portENTER_CRITICAL();
//			// get current time in seconds and update alarm register
//			currSec = RTC->TSR;
//			currSec += 10;
//			RTC->TAR = currSec;
//			portEXIT_CRITICAL();
//
//			RTC_GetAlarm(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
//			PRINTF("Alarm will occur at: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
//					RTC_1_dateTimeStruct.year,
//					RTC_1_dateTimeStruct.month,
//					RTC_1_dateTimeStruct.day,
//				    RTC_1_dateTimeStruct.hour,
//					RTC_1_dateTimeStruct.minute,
//					RTC_1_dateTimeStruct.second
//					);
//		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
