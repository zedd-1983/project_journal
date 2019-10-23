/*
 * moistureDetection.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */
#include "board.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "pin_mux.h"

extern SemaphoreHandle_t moistureDetectionSemphr;

void moistureDetection(void* pvParameters)
{
	for(;;)
	{
		// receive semaphore, indicate success
		if(xSemaphoreTake(moistureDetectionSemphr, 0)) {
			PRINTF("\r\nMoisture detected in moistureDetection task\r\n");
			for(int i = 0; i < 6; i++) {
				GPIO_PortToggle(BOARD_MD_LED_GPIO, 1 << BOARD_MD_LED_PIN);
				vTaskDelay(pdMS_TO_TICKS(150));
			}
		}

		// request time

		// assemble message for transmission
	}
}

