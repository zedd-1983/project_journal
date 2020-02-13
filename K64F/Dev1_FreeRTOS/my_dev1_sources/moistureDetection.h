/*
 * moistureDetection.h
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */

#ifndef MOISTUREDETECTION_H_
#define MOISTUREDETECTION_H_

struct eventData_t {
	char* eventTime;
	char* eventDate;
	char* wasAcknowledged;
};

// functions
void mainTask(void*);
void printRecords(struct eventData_t*);
char* convertRecordToString(struct eventData_t);

#endif /* MOISTUREDETECTION_H_ */
