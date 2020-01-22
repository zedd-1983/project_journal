/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_rtc.h"
#include "fsl_uart.h"
#include "fsl_clock.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* Definition of peripheral ID */
#define RTC_1_PERIPHERAL RTC
/* RTC_1 interrupt vector ID (number). */
#define RTC_1_COMMON_IRQN RTC_IRQn
/* RTC_1 interrupt vector priority. */
#define RTC_1_COMMON_IRQ_PRIORITY 11
/* RTC_1 interrupt handler identifier. */
#define RTC_1_COMMON_IRQHANDLER RTC_IRQHandler
/* Definition of peripheral ID */
#define BLUETOOTH_PERIPHERAL UART4
/* Definition of the clock source frequency */
#define BLUETOOTH_CLOCK_SOURCE CLOCK_GetFreq(UART4_CLK_SRC)
/* Definition of peripheral ID */
#define UART_1_PERIPHERAL UART0
/* Definition of the clock source frequency */
#define UART_1_CLOCK_SOURCE CLOCK_GetFreq(UART0_CLK_SRC)
/* Definition of peripheral ID */
#define PHONE_BT_PERIPHERAL UART3
/* Definition of the clock source frequency */
#define PHONE_BT_CLOCK_SOURCE CLOCK_GetFreq(UART3_CLK_SRC)

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const rtc_config_t RTC_1_config;
/* Date and time structure */
extern rtc_datetime_t RTC_1_dateTimeStruct;
extern const uart_config_t BLUETOOTH_config;
extern const uart_config_t UART_1_config;
extern const uart_config_t PHONE_BT_config;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */
