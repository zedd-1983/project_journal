/*
 * liquid_crystal.c
 *
 *  Created on: 13 Dec 2019
 *      Author: zedd
 */

#include "FreeRTOS.h"
#include "task.h"
#include "liquid_crystal.h"
#include "board.h"

/// @brief Task that looks after displaying text on an LCD display
/// @param pvParameters (void*)
void lcdTask(void* pvParameters)
{
	unsigned char test[4] = "zed";
	int l = 0;

	lcd_init();

	for(;;)
	{
		while(test[l] != '\0')
		{
			lcd_data(test[l]);
			l++;
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
		l = 0;
	}
}

void lcd_command(unsigned char command)
{
	GPIO_PortSet(BOARD_LCD_D7_GPIO, command << D7);
	GPIO_PinWrite(BOARD_LCD_RS_GPIO, RS, 0);
	GPIO_PinWrite(BOARD_LCD_RW_GPIO, RW, 0);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, EN, 1);
	vTaskDelay(pdMS_TO_TICKS(10));
//	for(int i = 0; i < 1000000; i++);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, EN, 0);
}

void lcd_data(unsigned char data)
{
	GPIO_PortSet(GPIOD, data << D7);
	GPIO_PinWrite(BOARD_LCD_RS_GPIO, RS, 1);
	GPIO_PinWrite(BOARD_LCD_RW_GPIO, RW, 0);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, EN, 1);
	vTaskDelay(pdMS_TO_TICKS(10));
	//for(int i = 0; i < 1000000; i++);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, EN, 0);
}

void lcd_init()
{
	lcd_command(0x28);
	vTaskDelay(pdMS_TO_TICKS(10));
	//for(int i = 0; i < 1000000; i++);
	lcd_command(0x0f);
	vTaskDelay(pdMS_TO_TICKS(10));
	//for(int i = 0; i < 1000000; i++);
	lcd_command(0x01);
	vTaskDelay(pdMS_TO_TICKS(10));
	//for(int i = 0; i < 1000000; i++);
	lcd_command(0x81);
	vTaskDelay(pdMS_TO_TICKS(10));
	//for(int i = 0; i < 1000000; i++);
}
