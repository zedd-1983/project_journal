/*
 * helperFunctions.h
 *
 *  Created on: 28 Oct 2019
 *      Author: zedd
 */

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

/// @brief structure containing variables for storing date
struct userDate_t {
	uint16_t year;
	uint8_t month;
	uint8_t day;
};

/// @brief structure containing variables for storing time
struct userTime_t {
	uint8_t hour;
	uint8_t minute;
};

uint8_t getMenuOption();
void displayMenu();
void printCurrentTime(RTC_Type *, rtc_datetime_t *);
void configureAlarm(uint32_t);
void displayAlarmTime(RTC_Type *, rtc_datetime_t *);
struct userDate_t getDate(char*);
struct userTime_t getTime(char*);
char* getSystemTime(RTC_Type* base, rtc_datetime_t* datetime);
char* getSystemDate(RTC_Type* base, rtc_datetime_t* datetime);

#endif /* HELPERFUNCTIONS_H_ */
