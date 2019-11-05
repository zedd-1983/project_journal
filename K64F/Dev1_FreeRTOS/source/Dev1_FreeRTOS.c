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
#include "fsl_uart.h"
#include "SEGGER_RTT.h"
/* TODO: insert other include files here. */
#include "moistureDetection.h"
#include "helperFunctions.h"
#include "timeKeeping.h"
#include "task.h"
#include "semphr.h"

/* TODO: insert other definitions and declarations here. */
TaskHandle_t mainTaskHandle = NULL;
TaskHandle_t terminalTaskHandle = NULL;
TaskHandle_t userTimeConfig = NULL;
SemaphoreHandle_t moistureDetectionSemphr = NULL;
SemaphoreHandle_t alarmSemphr = NULL;
SemaphoreHandle_t setAlarmSemphr = NULL;
//SemaphoreHandle_t userTimeConfig = NULL;

static void terminalTask(void*);
void configureTime(void*);

bool busyWait = false;
/* TODO: interrupts */

// this interrupt will simulate interrupt received from moisture detection
// device. Eventually the switch will be replaced by this device. It should
// pass a semaphore to moisture detection task that will request current
// time from timekeeping task and assemble a message that will be transmitted
// via BT to the other K64F
void PORTC_IRQHandler()
{
	static BaseType_t xHigherPriorityTaskWoken;

	// clear pending bits
	GPIO_PortClearInterruptFlags(BOARD_MD_GPIO, 1 << BOARD_MD_PIN);
	//PRINTF("\r\nMoisture detected (Button 2 Pressed)\r\n");
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(moistureDetectionSemphr, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

//void PORTA_IRQHandler()
//{
//	static BaseType_t xHigherPriorityTaskWoken;
//
//	// clear pending bits
//	GPIO_PortClearInterruptFlags(BOARD_SET_AL_GPIO, 1 << BOARD_SET_AL_PIN);
//	PRINTF("\r\nTime configuration (in ISR)\r\n");
//	xHigherPriorityTaskWoken = pdFALSE;
//	xSemaphoreGiveFromISR(userTimeConfig, &xHigherPriorityTaskWoken);
//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//}

// RTC interrupt that sets flag when an alarm interrupt occurs. Currently gets
// set to 10s after moisture detection but in actual run would be set to occur
// approx 23 hours 45 mins later to try to make the child/person more aware/awake
void RTC_1_COMMON_IRQHANDLER()
{
	uint32_t status = RTC_GetStatusFlags(RTC_1_PERIPHERAL);

	if(status & kRTC_AlarmFlag)
	{
		busyWait = true;
		RTC_ClearStatusFlags(RTC_1_PERIPHERAL, kRTC_AlarmInterruptEnable);
	}
}

//void UART0_RX_TX_IRQHandler() {
//
//	static uint8_t charReceived;
//
//	if(UART_GetStatusFlags(UART0) & kUART_RxDataRegFullFlag)
//	{
//		charReceived = UART_ReadByte(UART0);
//		PRINTF("\n\r%c\n\r");
//
//		switch(charReceived)
//		{
//			case 'T':
//			case 't': printCurrentTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct); break;
//			case 'A':
//			case 'a': displayAlarmTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct); break;
//			case 'C':
//			case 'c': PRINTF("\n\rConfigure user time\n\r"); break;
//			default:
//				PRINTF("\n\rInvalid option\n\r");
//		}
//	}
//}


int main(void) {

	// enable cycle counter for SYSTEMVIEW
	DWT->CTRL |= (1 << 0);

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

    UART_EnableInterrupts(UART0, kUART_RxDataRegFullInterruptEnable);
    NVIC_SetPriority(UART0_RX_TX_IRQn, 11);
    NVIC_ClearPendingIRQ(UART0_RX_TX_IRQn);
    NVIC_EnableIRQ(UART0_RX_TX_IRQn);
//    NVIC_SetPriority(BOARD_SW3_IRQ, 10);
//    NVIC_ClearPendingIRQ(BOARD_SW3_IRQ);
//    NVIC_EnableIRQ(BOARD_SW3_IRQ);

    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN"Easysleep - moisture detection");
    SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_BLUE"Terminal 0");

    //SEGGER_RTT_SetTerminal(1);

    SEGGER_RTT_TerminalOut(1, RTT_CTRL_TEXT_BRIGHT_RED"Terminal 1");
    //SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED"Terminal 1");

    // start recording
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();

    if(xTaskCreate(mainTask, "Main Task", configMINIMAL_STACK_SIZE + 50, NULL, 2, &mainTaskHandle) == pdFALSE)
    {
    	PRINTF("\r\nFailed to start \"Main Task\"\r\n");
    }

    if(xTaskCreate(terminalTask, "Terminal Task", configMINIMAL_STACK_SIZE + 50, NULL, 2, &terminalTaskHandle) == pdFALSE)
    {
    	PRINTF("\r\nFailed to start \"Terminal Task\"\r\n");
    }

    moistureDetectionSemphr = xSemaphoreCreateBinary();
    //userTimeConfig = xSemaphoreCreateBinary();

    vTaskStartScheduler();

    return 0 ;
}

void terminalTask(void* pvParameters)
{
	for(;;)
	{
		uint8_t character = '\0';
		if(SEGGER_RTT_HasKey())
		{
			character = SEGGER_RTT_GetKey();
			switch(character)
			{
				case 't':
				case 'T': printCurrentTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct); break;
				case 'a':
				case 'A': displayAlarmTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct, 0); break;
				case 'c':
				case 'C': xTaskCreate(configureTime, "Time configuration task", configMINIMAL_STACK_SIZE + 50, NULL, 3, NULL);
			}
		}
	vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void configureTime(void* pvParameters)
{
	uint8_t buffer[17];
	SEGGER_RTT_TerminalOut(0, RTT_CTRL_TEXT_BRIGHT_CYAN"Time config"RTT_CTRL_RESET);
	SEGGER_RTT_TerminalOut(0, "Enter datetime in YYYY-MM-DD-HH-MM format: ");

	if(SEGGER_RTT_HasData(0)) {
		uint16_t year = ((buffer[0] - 48) * 1000) + ((buffer[1] - 48) * 100) + ((buffer[2] - 48) * 10) + ((buffer[3] - 48));
		uint8_t month = ((buffer[5] - 48) * 10) + ((buffer[6] - 48));
		uint8_t day = ((buffer[8] - 48) * 10) + ((buffer[9] - 48));
		uint8_t hour = ((buffer[11] - 48) * 10) + ((buffer[12] - 48));
		uint8_t minute = ((buffer[14] - 48) * 10) + ((buffer[15] - 48));

		// stop rtc timer
		RTC_StopTimer(RTC_1_PERIPHERAL);

		RTC_1_dateTimeStruct.year = year;
		RTC_1_dateTimeStruct.month = month;
		RTC_1_dateTimeStruct.day = day;
		RTC_1_dateTimeStruct.hour = hour;
		RTC_1_dateTimeStruct.minute = minute;
		RTC_1_dateTimeStruct.second = 0;

		// reconfigure rtc timer and start again
		RTC_SetDatetime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
		RTC_StartTimer(RTC_1_PERIPHERAL);
	}

	vTaskDelete(NULL);
}
