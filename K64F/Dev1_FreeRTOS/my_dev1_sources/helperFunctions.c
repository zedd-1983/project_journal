/*
 * helperFunctions.c
 *
 *  Created on: 28 Oct 2019
 *      Author: zedd
 */
#include "fsl_debug_console.h"
#include "fsl_rtc.h"
#include "SEGGER_RTT.h"
#include "helperFunctions.h"

//#define NRF_LOG_USES_RTT  1

/// @brief function to display user menu
/// @param none
/// @return none
void displayMenu()
{
	PRINTF("\n\rMenu [DISABLED]\n\r");
	PRINTF("\n\rT\t- display time\n\r");
	PRINTF("A\t- display alarm time\n\r");
	PRINTF("C\t- configure user time\n\n\r");
}

/// @brief function to get current time from the RTC structure
/// @param base pointer to RTC base address
/// @param datetime pointer to rtc_datetime_t structure
/// @return none
void printCurrentTime(RTC_Type * base, rtc_datetime_t* datetime)
{
	RTC_GetDatetime(base, datetime );
	PRINTF("\rDate: %04d-%02d-%02d Time: %02d:%02d:%02d",
			datetime->year,
			datetime->month,
			datetime->day,
			datetime->hour,
			datetime->minute,
			datetime->second
			);
}

/// @brief function to configure RTC alarm
/// @param secIncrement sets alarm to number of seconds from function call
/// @return none
void configureAlarm(uint32_t secIncrement)
{
			// configure alarm
			uint32_t currSec = 0;
			currSec = RTC->TSR;
			currSec += secIncrement;
			RTC->TAR = currSec;
}

/// @brief function to get current alarm time from the RTC structure
/// @param base pointer to RTC base address
/// @param datetime pointer to rtc_datetime_t structure
/// @return none
void displayAlarmTime(RTC_Type* base, rtc_datetime_t* datetime)
{
		RTC_GetAlarm(base, datetime);
		PRINTF("\n\rAlarm will occur at: %04d-%02d-%02d %02d:%02d:%02d\n\r",
				datetime->year,
				datetime->month,
				datetime->day,
				datetime->hour,
				datetime->minute,
				datetime->second
				);
}

/// @brief polling method to get user selected menu option
/// @param none
/// @return uint8_t
uint8_t getMenuOption()
{
	char ch;

	if(SEGGER_RTT_HasKey())
		ch = SEGGER_RTT_GetKey();

	return ch;
}

/// @brief function that converts date from user input to userDate_t structure
/// @param stringDate user input
/// @return struct userDate_t
struct userDate_t getDate(char* stringDate)
{
	struct userDate_t date = {};

	date.year = ((stringDate[0] - 48) * 1000) +
				((stringDate[1] - 48) * 100) +
				((stringDate[2] - 48) * 10) +
				((stringDate[3] - 48));
	date.month = ((stringDate[5] - 48) * 10) + ((stringDate[6] - 48));
	date.day = ((stringDate[8] - 48) * 10) + ((stringDate[9] - 48));

	return date;
}

/// @brief function that converts time from user input to userTime_t structure
/// @param stringTime user input
/// @return struct userTime_t
struct userTime_t getTime(char* stringTime)
{
	struct userTime_t time;

	time.hour = ((stringTime[0] - 48) * 10) + ((stringTime[1] - 48));
	time.minute = ((stringTime[3] - 48) * 10) + ((stringTime[4] - 48));

	return time;
}







