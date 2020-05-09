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
/// @return void
void userTimeConfig(void* pvParameters)
{
		char stringDate[11] = "";
		char stringTime[6] = "";
		char stringTimeDate[17] = "";

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

/// @brief FreeRTOS task for user defined time configuration via phone
/// @details This task allows the user to change the system time remotely via
/// bluetooth module connected to UART3. Upon completion, it deletes itself.
/// @param pvParameters not used currently
/// @return void
void configureTimeViaPhone(void* pvParameters)
{

	uint8_t receivedDateTime[17];
	char* enterValDateTime = "Please enter valid dateTime in [YYYY-MM-DD:HH-MM] format";

	PRINTF("\n\r\033[33mIn configure time via phone\033[0m");

	for(;;) {
		UART_WriteBlocking(UART3, (uint8_t*)enterValDateTime, strlen(enterValDateTime) + 1 );

		switch(UART_ReadBlocking(UART3, receivedDateTime, 17)) {
			case kStatus_Success 				: 	PRINTF("\n\rReceived: %s", receivedDateTime);
													configureAndDisplayNewTime(receivedDateTime);
													break;
			case kStatus_UART_RxHardwareOverrun : 	PRINTF("\n\rHardware Overrun"); break;
			case kStatus_UART_NoiseError 		: 	PRINTF("\n\rNoise Error"); break;
			case kStatus_UART_FramingError 		: 	PRINTF("\n\rFraming Error"); break;
			case kStatus_UART_ParityError 		: 	PRINTF("\n\rParity Error"); break;
			default								: 	PRINTF("\n\rUnknown Error"); break;
		}
		vTaskDelete(NULL);
	}
}

void configureAndDisplayNewTime(char* receivedDateTime)
{
	uint16_t newYear = 1970;
	uint8_t newMonth = 1, newDay = 1, newHour = 1, newMinute =1;
	char* dateAndTimeSplit[2];

	dateAndTimeSplit[1] = strtok((char*)receivedDateTime, ":");
	PRINTF("\n\rDate: %s", dateAndTimeSplit[1]);
	struct userDate_t newDate = getDate(dateAndTimeSplit[1]);
	newYear = newDate.year;
	newMonth = newDate.month;
	newDay = newDate.day;

	if((newYear < 1970 || newYear > 2099) ||
			(newMonth < 1 || newMonth > 12) ||
			(newDay   < 1 || newDay > 31)) {
		UART_WriteBlocking(UART3, (uint8_t*)"Invalid date", strlen("Invalid date") + 1);
		return;
	}

	dateAndTimeSplit[2] = strtok(NULL, '\0');
	PRINTF("\n\rTime: %s", dateAndTimeSplit[2]);
	struct userTime_t newTime = getTime(dateAndTimeSplit[2]);
	newHour = newTime.hour;
	newMinute = newTime.minute;

	if((newHour < 0 || newHour > 23) || (newMinute < 0 || newMinute > 59))
	{
		UART_WriteBlocking(UART3, (uint8_t*)"Invalid time", strlen("Invalid time") + 1);
		return;
	}

	RTC_StopTimer(RTC_1_PERIPHERAL);
	RTC_1_dateTimeStruct.year = newYear;
	RTC_1_dateTimeStruct.month = newMonth;
	RTC_1_dateTimeStruct.day = newDay;
	RTC_1_dateTimeStruct.hour = newHour;
	RTC_1_dateTimeStruct.minute = newMinute;
	RTC_SetDatetime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
	RTC_StartTimer(RTC_1_PERIPHERAL);

	// display new date and time on user's phone
	char* systemDate = getSystemDate(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
	UART_WriteBlocking(UART3, (uint8_t*)"System date set to: ", strlen("System date set to: ") + 1);
	UART_WriteBlocking(UART3, (uint8_t*)systemDate, strlen(systemDate) + 1);

	char* systemTime = getSystemTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
	UART_WriteBlocking(UART3, (uint8_t*)"System time set to: ", strlen("System time set to: ") + 1);
	UART_WriteBlocking(UART3, (uint8_t*)systemTime, strlen(systemTime) + 1);


}
