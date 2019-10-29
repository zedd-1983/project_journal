/*
 * helperFunctions.h
 *
 *  Created on: 28 Oct 2019
 *      Author: zedd
 */

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

void displayMenu();
void printCurrentTime(RTC_Type *, rtc_datetime_t *);
void configureAlarm(uint32_t);
void displayAlarmTime(RTC_Type *, rtc_datetime_t *);

#endif /* HELPERFUNCTIONS_H_ */
