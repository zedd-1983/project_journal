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
/// @details bluetooth task responsible for transmission of data to
/// Dev2. Based on the alarm type received from moisture detection interrupt
/// and rtc interrupt through task notifications sends appropriate flag to Dev2.
/// If 1 - notifies Dev2 mainTask to create a motorTask and if 2 - to create a
/// buzzerTask
void btTask(void* pvParameters) {

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
	}
}
