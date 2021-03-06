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
#include "SEGGER_RTT.h"
#include "helperFunctions.h"

extern SemaphoreHandle_t setAlarmSemphr;
extern rtc_datetime_t RTC_1_dateTimeStruct;

#define SHOW_MESSAGES 1

/// @brief FreeRTOS task for user defined time configuration
/// @details this task stops disables the interrupt on UART0 first
/// followed by stopping the RTC timer. User input is then obtained
/// and verified and upon success loaded into rtc_datetime_t structure
/// that holds values of current time and date thus changing it. Timer
/// and UART0 interrupts are then enabled again. The task then deletes
/// itself.
/// @param pvParameters not used currently
/// @return none
void userTimeConfig(void* pvParameters)
{
		char stringDate[11] = "";
		char stringTime[6] = "";
		char stringTimeDate[17] = "";

//		char year[5] = "";
//		char month[3] = "";
//		char day[3] = "";
//		char hour[3] = "";
//		char minute[3] = "";

		uint16_t newYear = 1970;
		uint8_t newMonth = 1, newDay = 1, newHour = 1, newMinute =1;

		for(;;)
		{
			RTC_StopTimer(RTC);

			PRINTF("\n\rEnter new date [min val 1970-01-01] in format YYYY-MM-DD: ");
			SCANF("%s", stringDate);

			// get date from the user input
			struct userDate_t newDate = getDate(stringDate);

			PRINTF("\n\rNew date: %s", stringDate);
			newYear = newDate.year;
			newMonth = newDate.month;
			newDay = newDate.day;

			while((newYear < 1970 || newYear > 2099) ||
					(newMonth < 1 || newMonth > 12) ||
					(newDay   < 1 || newDay > 31))
			{
				PRINTF("\n\r*****  Invalid date [min val 1970-01-01] *****\n\r"
						"Enter new date in format YYYY-MM-DD: ");
				SCANF("%s", stringDate);

				newDate = getDate(stringDate);

				newYear = newDate.year;
				newMonth = newDate.month;
				newDay = newDate.day;
			}

			PRINTF("\n\rEnter new time in format HH-MM: ");
			SCANF("%s", stringTime);

			struct userTime_t newTime = getTime(stringTime);

			newHour = newTime.hour;
			newMinute = newTime.minute;

			while((newHour < 0 || newHour > 23) || (newMinute < 0 || newMinute > 59))
			{
				PRINTF("\n\rInvalid time value, try again: ");
				SCANF("%s", stringTime);

				newTime = getTime(stringTime);

				newHour = newTime.hour;
				newMinute = newTime.minute;
			}

			RTC_1_dateTimeStruct.year = newYear;
			RTC_1_dateTimeStruct.month = newMonth;
			RTC_1_dateTimeStruct.day = newDay;
			RTC_1_dateTimeStruct.hour = newHour;
			RTC_1_dateTimeStruct.minute = newMinute;

			RTC_SetDatetime(RTC, &RTC_1_dateTimeStruct);
			memset(stringTimeDate, 0, sizeof(stringTimeDate));
	#ifdef SHOW_MESSAGES
			PRINTF("\n\rYear: %d", RTC_1_dateTimeStruct.year);
			PRINTF("\n\rMonth: %d", RTC_1_dateTimeStruct.month);
			PRINTF("\n\rDay: %d", RTC_1_dateTimeStruct.day);
			PRINTF("\n\rHour: %d", RTC_1_dateTimeStruct.hour);
			PRINTF("\n\rMinute: %d\n\n\r", RTC_1_dateTimeStruct.minute);
	#endif
			RTC_StartTimer(RTC);
			vTaskDelete(NULL);
		}
}
