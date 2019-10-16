/**
 * @file    Dev1_FreeRTOS.c
 * @brief   This application will use FreeRTOS and various tasks
 * 			to detect moisture, run real-time clock, use flash memory to
 * 			store data and use UART to communicate with BT module and
 * 			thus with second device (the bracelet)
 */
#include "FreeRTOS.h"
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "moistureDetection.h"
#include "timeKeeping.h"
#include "task.h"
#include "semphr.h"

/* TODO: insert other definitions and declarations here. */
TaskHandle_t detectMoistureHandle = NULL;
TaskHandle_t timeKeepingHandle = NULL;
SemaphoreHandle_t moistureDetectionSemphr = NULL;
/* TODO: interrupts */

// this interrupt will simulate interrupt received from moisture detection
// device. It should pass a semaphore to moisture detection task that will
// request current time from timekeeping task and assemble a message that will
// be transmitted via BT
void PORTC_IRQHandler()
{
	static BaseType_t xHigherPriorityTaskWoken;

	// clear pending bits
	GPIO_PortClearInterruptFlags(BOARD_SW2_GPIO, 1 << BOARD_SW2_GPIO_PIN);
	PRINTF("\r\nMoisture detected (Button 2 Pressed)\r\n");
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(moistureDetectionSemphr, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    // enable interrupts in NVIC
    NVIC_SetPriority(BOARD_SW2_IRQ, 10);
    NVIC_ClearPendingIRQ(BOARD_SW2_IRQ);
    NVIC_EnableIRQ(BOARD_SW2_IRQ);

    PRINTF("Easysleep - moisture detection\r\n");

    if(xTaskCreate(moistureDetection, "Moisture Detection Task", configMINIMAL_STACK_SIZE + 10, NULL, 2, &detectMoistureHandle) == pdFALSE)
    {
    	PRINTF("\r\nFailed to start \"Moisture Detection Task\"\r\n");
    }

//    if(xTaskCreate(timeKeeping, "Time Keeping Task", configMINIMAL_STACK_SIZE + 10, NULL, 2, &timeKeepingHandle) == pdFALSE)
//    {
//    	PRINTF("\r\nFailed to start \"Time Keeping Task\"\r\n");
//    }

    moistureDetectionSemphr = xSemaphoreCreateBinary();

    vTaskStartScheduler();

    // should never reach here

    return 0 ;
}
