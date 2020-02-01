/*
 * bluetooth2.h
 *
 *  Created on: 20 Jan 2020
 *      Author: zedd
 */

#ifndef BLUETOOTH2_H_
#define BLUETOOTH2_H_

#define DEV2_ALARM_STOP 'a'
#define SYSTEM_TIME_REQUEST 'b'
#define SYSTEM_TIME_CHANGE 'c'
#define REQUEST_RECORDS 'd'

void phoneBTTask(void*);

#endif /* BLUETOOTH2_H_ */
