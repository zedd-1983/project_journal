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

#include "moistureDetection.h"
#include "helperFunctions.h"
#include "timeKeeping.h"
#include "task.h"
#include "semphr.h"

TaskHandle_t mainTaskHandle = NULL;
TaskHandle_t terminalTaskHandle = NULL;
TaskHandle_t userTimeConfigHandle = NULL;
SemaphoreHandle_t moistureDetectionSemphr = NULL;
SemaphoreHandle_t alarmSemphr = NULL;
SemaphoreHandle_t setAlarmSemphr = NULL;
SemaphoreHandle_t userTimeConfigSemphr = NULL;

//static void terminalTask(void*);
//void configureTime(void*);

// variables
bool busyWait = false;

#define INTERRUPT_MESSAGES 1

/// @brief moisture detection external interrupt (SW2)
/// @details this interrupt simulates interrupt received from moisture detection
/// device. Eventually the switch will be replaced by this device. It should
/// pass a semaphore to moisture detection task that will request current
/// time from timekeeping task and assemble a message that will be transmitted
/// via BT to the other K64F
void PORTC_IRQHandler()
{
#ifdef INTERRUPT_MESSAGES
	PRINTF("\r\nMoisture detected (Button 2 Pressed)\r\n");
#endif

	static BaseType_t xHigherPriorityTaskWoken;

	GPIO_PortClearInterruptFlags(BOARD_MD_GPIO, 1 << BOARD_MD_PIN);
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(moistureDetectionSemphr, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/// @brief time configuration interrupt (SW3)
/// @details external interrupt (SW3) that gives semaphore to main task to create
/// userTimeConfig task
void PORTA_IRQHandler()
{
#ifdef INTERRUPT_MESSAGES
	PRINTF("\n\rTime config interrupt\n\r");
#endif

	static BaseType_t xHigherPriorityTaskWoken;

	GPIO_PortClearInterruptFlags(BOARD_SET_AL_GPIO, 1 << BOARD_SET_AL_PIN);
	PRINTF("\r\nTime configuration (in ISR)\r\n");
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(userTimeConfigSemphr, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/// @brief RTC interrupt that sets flag when an alarm interrupt occurs.
/// @details Currently gets set to 10s after moisture detection but in actual run
/// would be set to occur approx 23 hours 45 mins later to try to make the
/// child/person more aware/awake
void RTC_1_COMMON_IRQHANDLER()
{
#ifdef INTERRUPT_MESSAGES
	PRINTF("\n\rRTC interrupt\n\r");
#endif

	uint32_t status = RTC_GetStatusFlags(RTC_1_PERIPHERAL);

	if(status & kRTC_AlarmFlag)
	{
		busyWait = true;
		RTC_ClearStatusFlags(RTC_1_PERIPHERAL, kRTC_AlarmInterruptEnable);
	}
}


/// @brief BlueTooth IRQ Handler (UART4) for managing interrupts from Bluetooth module
/// @note rx - PTC14, tx - PTC15
/// @note 38400,8,N,1 , priority 8
void BLUETOOTH_IRQHandler() {
#ifdef INTERRUPT_MESSAGES
	PRINTF("\n\rBluetooth Interrupt\n\r");
#endif

	static char charReceived = 0;

	if(UART_GetStatusFlags(BLUETOOTH_PERIPHERAL) & kUART_RxDataRegFullFlag) {
		charReceived = UART_ReadByte(BLUETOOTH_PERIPHERAL);
		printf("\n\r%c\n\r", charReceived);
		printf(&charReceived);
		puts(&charReceived);
	}
}

/// @brief UART0 IRQ Handler
/// @details interrupt handler for UART0 used to get user input from
/// the menu displayed by the main task
/// @return void
void UART0_RX_TX_IRQHandler() {

	uint8_t charReceived;

	if(UART_GetStatusFlags(UART0) & kUART_RxDataRegFullFlag)
	{
		charReceived = UART_ReadByte(UART0);
		PRINTF("\n\r%c\n\r");

		switch(charReceived)
		{
			case 'T':
			case 't': printCurrentTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct); break;
			case 'A':
			case 'a': displayAlarmTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct); break;
			case 'C':
			case 'c': PRINTF("\n\rConfigure user time\n\r"); break;
			default:
				PRINTF("\n\rInvalid option\n\r");
		}
	}
}

/// @brief Main function
/// @details K64F hardware is initialized through this function, IRQs are
/// enabled in NVIC and FreeRTOS mainTask is started.
/// A number of semaphores are created as well
/// @param void
/// @return int
int main(void) {

	// enable cycle counter for SYSTEMVIEW
	DWT->CTRL |= (1 << 0);

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("Application Start\n\r");

    NVIC_SetPriority(BOARD_SW2_IRQ, 10);
    NVIC_ClearPendingIRQ(BOARD_SW2_IRQ);
    NVIC_EnableIRQ(BOARD_SW2_IRQ);

    UART_EnableInterrupts(UART0, kUART_RxDataRegFullInterruptEnable);
    NVIC_SetPriority(UART0_RX_TX_IRQn, 11);
    NVIC_ClearPendingIRQ(UART0_RX_TX_IRQn);
    NVIC_EnableIRQ(UART0_RX_TX_IRQn);

    UART_EnableInterrupts(BLUETOOTH_PERIPHERAL, kUART_RxDataRegFullInterruptEnable);
    NVIC_SetPriority(UART4_RX_TX_IRQn, 12); // was 8
    NVIC_ClearPendingIRQ(UART4_RX_TX_IRQn);
    NVIC_EnableIRQ(UART4_RX_TX_IRQn);

    NVIC_SetPriority(BOARD_SW3_IRQ, 10);
    NVIC_ClearPendingIRQ(BOARD_SW3_IRQ);
    NVIC_EnableIRQ(BOARD_SW3_IRQ);

    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();

    if(xTaskCreate(mainTask, "Main Task", configMINIMAL_STACK_SIZE + 50, NULL, 2, &mainTaskHandle) == pdFALSE)
    {
    	PRINTF("\r\nFailed to start \"Main Task\"\r\n");
    }

    moistureDetectionSemphr = xSemaphoreCreateBinary();
    userTimeConfigSemphr = xSemaphoreCreateBinary();

    vTaskStartScheduler();

    return 0;
}
