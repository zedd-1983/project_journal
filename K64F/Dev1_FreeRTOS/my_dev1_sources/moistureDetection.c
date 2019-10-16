/*
 * moistureDetection.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern SemaphoreHandle_t moistureDetectionSemphr;

void moistureDetection(void* pvParameters)
{
	for(;;)
	{
		// receive semaphore
		if(xSemaphoreTake(moistureDetectionSemphr, portMAX_DELAY));
			PRINTF("\r\nMoisture detected in moistureDetection task\r\n");

		// request time

		// assemble message for transmission
	}
}

