/*
 * bluetooth.c
 *
 *  Created on: 3 Dec 2019
 *      Author: zedd
 */
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"
#include "fsl_uart.h"

extern uint32_t alarmType;

/// @brief FreeRTOS bluetooth task
/// @param pvParameters a character is passed to this function that will print
/// it out before deleting itself (eventually on this device this task will send
/// notification to the other device to start an alarm)
void btTask(void* pvParameters) {

	//uint8_t alarmFlag = 1;
	uint32_t receivedAlarmType;
	uint8_t transmitValue;

	for(;;)
	{
		if(xTaskNotifyWait(0,0,&alarmType, 0) == pdTRUE)
			receivedAlarmType = alarmType;

		if(receivedAlarmType == 1) {
			receivedAlarmType = 0;
			transmitValue = 1;
			UART_WriteByte(UART4, transmitValue);
		}
		else if(receivedAlarmType == 2) {
			receivedAlarmType = 0;
			transmitValue = 2;
			UART_WriteByte(UART4, transmitValue);
		}

		vTaskDelay(pdMS_TO_TICKS(150));
		//vTaskDelete(NULL);
	}
}
