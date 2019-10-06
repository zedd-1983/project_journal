/*
 * timeKeeping.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */

#include "fsl_debug_console.h" // to allow for PRINTF

void timeKeeping(void* pvParameters)
{
	for(;;)
		PRINTF("\r\nTime Keeping\r\n");
}
