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

/// @brief FreeRTOS bluetooth task
/// @param pvParameters a character is passed to this function that will print
/// it out before deleting itself (eventually on this device this task will send
/// notification to the other device to start an alarm)
void btTask(void* pvParameters) {

	uint8_t alarmFlag = 1;

	for(;;)
	{
		UART_WriteByte(UART4, alarmFlag);
		vTaskDelete(NULL);
	}
}
