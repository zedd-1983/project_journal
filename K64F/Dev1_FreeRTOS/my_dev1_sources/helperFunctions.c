/*
 * helperFunctions.c
 *
 *  Created on: 28 Oct 2019
 *      Author: zedd
 */
#include "fsl_debug_console.h"
#include "fsl_rtc.h"


void displayMenu()
{
	PRINTF("\n\rMenu\n\r");
	PRINTF("\n\rT\t- display time\n\r");
	PRINTF("A\t- display alarm time\n\r");
	PRINTF("C\t- configure user time\n\n\r");
}

void printCurrentTime(RTC_Type * base, rtc_datetime_t* datetime)
{
		// request time and print it
		RTC_GetDatetime(base, datetime );
		PRINTF("\rDate: %04hd-%02hd-%02hd Time: %02hd-%02hd-%02hd\r",
				datetime->year,
				datetime->month,
				datetime->day,
				datetime->hour,
				datetime->minute,
				datetime->second
				);
}

void configureAlarm(uint32_t secIncrement)
{
			// configure alarm
			uint32_t currSec = 0;
			currSec = RTC->TSR;
			currSec += secIncrement;
			RTC->TAR = currSec;
}

void displayAlarmTime(RTC_Type* base, rtc_datetime_t * datetime)
{
			RTC_GetAlarm(base, datetime);
			PRINTF("\n\rAlarm will occur at: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\n\r",
					datetime->year,
					datetime->month,
					datetime->day,
				    datetime->hour,
					datetime->minute,
					datetime->second
					);
}
