/*
 * bluetooth2.c
 *
 *  Created on: 20 Jan 2020
 *      Author: zedd
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_uart.h"
#include "fsl_debug_console.h"
#include "queue.h"

extern QueueHandle_t phoneBTReceiveQ;

void phoneBTTask(void *pvParameters)
{
	// task for handling communication between the master device and the phone

///	TODO:	check if connected and receiving characters
/// TODO:	send data via queue to bluetooth task
	for(;;)
	{
		if(GPIO_PinRead(BOARD_BT2_STATUS_GPIO, BOARD_BT2_STATUS_PIN) == 1)
			GPIO_PinWrite(BOARD_GREEN_LED_GPIO, BOARD_GREEN_LED_PIN, 0);
		else
			GPIO_PortToggle(BOARD_GREEN_LED_GPIO, 1 << BOARD_GREEN_LED_PIN);

		if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(UART3)) {
			uint8_t charReceived = UART_ReadByte(UART3);

			if(charReceived != '\0') {
				PRINTF("\n\rReceived: %c", charReceived);

				xQueueSend(phoneBTReceiveQ, &charReceived, pdMS_TO_TICKS(0));
			}


			//charReceived = '\0';
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}


/// TODO: 	if event happens, this task needs to be notified (an LED goes ON)
/// TODO: 	need to be able to request time and date of previous events from mald ster device
///			(these will probably be recorded in an array of strings at first)
///			(should include time and date, ACK status, maybe length and type of wake-up method used)
/// TODO: 	need to be able to stop pending alarm if event occurs, event still needs to be
///			recorded
/// TODO: 	allow for setting time and date through the phone application rather than through LCD and
/// 		keypad (maybe)
}
