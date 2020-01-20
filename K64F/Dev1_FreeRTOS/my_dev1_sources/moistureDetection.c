/*
 * moistureDetection.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */

// FreeRTOS
#include "FreeRTOS.h"
#include "semphr.h"

// K64F
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "peripherals.h"
#include "fsl_rtc.h"

// My headers
#include "helperFunctions.h"
#include "timeKeeping.h"
#include "bluetooth.h"
#include "SEGGER_RTT.h"

extern bool busyWait;
extern SemaphoreHandle_t moistureDetectionSemphr;
extern SemaphoreHandle_t userTimeConfigSemphr;
extern SemaphoreHandle_t btSemphr;
extern TaskHandle_t userTimeConfigHandle;
extern rtc_datetime_t RTC_1_dateTimeStruct;

/// @brief FreeRTOS main task
/// @details this task displays menu and if it receives semaphore from SW3 creates
/// another task for configuration of date and time. If moisture is detected (SW2)
/// semaphore is given from an interrupt to the main task to configure the alarm
/// (currently set to 10s)
void mainTask(void* pvParameters)
{
	//displayMenu();

	for(;;)
	{
		//printCurrentTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);

		// check BT status
		if(GPIO_PinRead(BOARD_BT1_STATUS_GPIO, BOARD_BT1_STATUS_PIN) == 1)
			GPIO_PinWrite(BOARD_BLUE_LED_GPIO, BOARD_BLUE_LED_PIN, 0);
		else
			GPIO_PortToggle(BOARD_BLUE_LED_GPIO, 1 << BOARD_BLUE_LED_PIN);
			//GPIO_PinWrite(BOARD_BLUE_LED_GPIO, BOARD_BLUE_LED_PIN, 1);

		// check BT for incoming character
//		if(kUART_RxDataRegFullFlag & UART_GetStatusFlags(BLUETOOTH_PERIPHERAL)) {
//			uint8_t btChar = UART_ReadByte(BLUETOOTH_PERIPHERAL);
//			xTaskCreate(btTask, "BT task", configMINIMAL_STACK_SIZE + 50, &btChar, 5, NULL);
//		}

		if(xSemaphoreTake(moistureDetectionSemphr, 0))
		{
			PRINTF("\n\r\033[34mMoisture detected\033[0m\n\r");
			// flash an LED to indicate moisture detection
			for(int i = 0; i < 6; i++) {
				GPIO_PortToggle(BOARD_MD_LED_GPIO, 1 << BOARD_MD_LED_PIN);
				vTaskDelay(pdMS_TO_TICKS(150));
			}

			configureAlarm(20);
			displayAlarmTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);

		}	 // moistureDetectionSemphr

		if(xSemaphoreTake(userTimeConfigSemphr, 0) == pdTRUE) {
		    if(xTaskCreate(userTimeConfig, "UserTimeConfig Task", configMINIMAL_STACK_SIZE + 150, NULL, 3, &userTimeConfigHandle) == pdFALSE)
		    {
		    	PRINTF("\r\nFailed to start \"UserTimeConfig Task\"\r\n");
		    }
		}
		// trigger alarm, flag is set by an alarm interrupt in RTC_1_COMMON_IRQHANDLER()
//		if(xSemaphoreTake(btSemphr, 0) == pdTRUE) {
//			xTaskCreate(btTask, "BT task", configMINIMAL_STACK_SIZE + 50, NULL, 5, NULL);
//		}



//		if(busyWait)
//		{
//			busyWait = false;
//
//			PRINTF(RTT_CTRL_TEXT_BRIGHT_RED"\n\r************ ALARM **************\n\r"RTT_CTRL_RESET);
//		}

		vTaskDelay(pdMS_TO_TICKS(150));

	}	// end of for(;;)
}
