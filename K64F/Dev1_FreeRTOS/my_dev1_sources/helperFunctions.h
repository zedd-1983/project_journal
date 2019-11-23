/*
 * helperFunctions.h
 *
 *  Created on: 28 Oct 2019
 *      Author: zedd
 */

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

// structures
struct userDate_t {
	uint16_t year;
	uint8_t month;
	uint8_t day;
};

struct userTime_t {
	uint8_t hour;
	uint8_t minute;
};

// functions
uint8_t getMenuOption();
void displayMenu();
void printCurrentTime(RTC_Type *, rtc_datetime_t *);
void configureAlarm(uint32_t);
void displayAlarmTime(RTC_Type *, rtc_datetime_t *);
struct userDate_t getDate(char*);
struct userTime_t getTime(char*);

#endif /* HELPERFUNCTIONS_H_ */
