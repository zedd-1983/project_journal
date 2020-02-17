/*
 * bluetooth2.h
 *
 *  Created on: 20 Jan 2020
 *      Author: zedd
 */

#ifndef BLUETOOTH2_H_
#define BLUETOOTH2_H_

#define DEV2_ALARM_STOP 		0x61	// 'a'
#define SYSTEM_TIME_REQUEST 	0x62	// 'b'
#define SYSTEM_DATE_REQUEST		0x63	// 'c'
#define SYSTEM_TIME_CHANGE 		0x64	// 'd'
#define REQUEST_RECORDS 		0x65	// 'e'

void phoneBTTask(void*);
void sendDataToPhone(char*);
void busyDelay(int);
char* getDataFromPhone();

#endif /* BLUETOOTH2_H_ */
