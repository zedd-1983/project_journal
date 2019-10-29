/*
 * timeKeeping.c
 *
 *  Created on: 5 Oct 2019
 *      Author: zedd
 */

#include "fsl_debug_console.h" // to allow for PRINTF
#include "FreeRTOS.h"
#include "semphr.h"
#include "peripherals.h"

extern SemaphoreHandle_t userTimeConfig;
extern SemaphoreHandle_t setAlarmSemphr;
extern rtc_datetime_t RTC_1_dateTimeStruct;

//void userTimeConfig(void* pvParameters)
//{
//	uint32_t currSec = 0;
//	uint8_t buffer[12] = "";
//	uint8_t count = 0;
//
//	PRINTF("\n\rUser time configuration \n\r");
//	for(;;)
//	{
//		if(UART_GetStatusFlags(UART0) & kUART_RxDataRegFullFlag)
//		{
//			buffer[count] = UART_ReadByte(UART0);
//			PRINTF("%c", buffer[count]);
//			if(buffer[count] == '\r')
//			{
//				// parse the date
//				uint16_t year = ((buffer[0]-48) * 1000) + ((buffer[1]-48) * 100) +
//								((buffer[2]-48) * 10) + (buffer[3]-48);
//				uint8_t month = ((buffer[5]-48) * 10) + (buffer[6]-48);
//				uint8_t day = 	((buffer[8]-48) * 10) + (buffer[9]-48);
//
//				RTC_1_dateTimeStruct.year = year;
//				RTC_1_dateTimeStruct.month = month;
//				RTC_1_dateTimeStruct.day = day;
//
//				RTC_StopTimer(RTC_1_PERIPHERAL);
//				RTC_SetDateTime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
//				RTC_StartTimer(RTC_1_PERIPHERAL);
//
//				PRINTF("\n\rTime changed to: %04hd-%02hd-%02hd Time: %02hd-%02hd-%02hd\r",
//					RTC_1_dateTimeStruct.year,
//					RTC_1_dateTimeStruct.month,
//					RTC_1_dateTimeStruct.day,
//					RTC_1_dateTimeStruct.hour,
//					RTC_1_dateTimeStruct.minute,
//					RTC_1_dateTimeStruct.second
//					);
//			}
//			else
//			{
//				count++;
//			}
//			vTaskDelay(pdMS_TO_TICKS(100));
//		}
//	}
//}
