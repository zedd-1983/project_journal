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


/// current website followed to get the lcd working https://www.8051projects.net/lcd-interfacing/lcd-4-bit-programming.php

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
		}
		l = 0;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void lcd_command(unsigned char command)
{
	GPIO_PinWrite(BOARD_LCD_RS_GPIO, BOARD_LCD_RS_PIN, 0);
//	GPIO_PortSet(BOARD_LCD_D4_GPIO, ((command >> 4) & 0x0f) | 0x80);
	GPIO_PortSet(BOARD_LCD_D4_GPIO, ((command >> 4) & 0xff));
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

//	GPIO_PortSet(BOARD_LCD_D4_GPIO, ((command >> 4) & 0x0f));
	GPIO_PortSet(BOARD_LCD_D4_GPIO, ((command >> 4) & 0xff));
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

//	GPIO_PortSet(BOARD_LCD_D4_GPIO, (command & 0x0f) | 0x80);
	GPIO_PortSet(BOARD_LCD_D4_GPIO, (command & 0xff));
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

//	GPIO_PortSet(BOARD_LCD_D4_GPIO, (command & 0x0f));
	GPIO_PortSet(BOARD_LCD_D4_GPIO, (command & 0xff));
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	vTaskDelay(pdMS_TO_TICKS(20));
}

void lcd_data(unsigned char data)
{
	GPIO_PinWrite(BOARD_LCD_RS_GPIO, BOARD_LCD_RS_PIN, 1);	// select data register
//	GPIO_PortSet(GPIOD, (((data >> D4) & 0x0f) | 0x80 | 0x20));	// set port to data value and EN pin high and RS pin high

	GPIO_PortSet(GPIOD, (data >> 4) & 0xff);	// set port to data value and EN pin high and RS pin high
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

//	GPIO_PortSet(GPIOD, (((data >> D4) & 0x0f) | 0x20));		// set port to data value and EN pin low (assuming this is the high/low transition needed)
	GPIO_PortSet(GPIOD, (data >> 4) & 0xff);		// set port to data value and EN pin low (assuming this is the high/low transition needed
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

//	GPIO_PortSet(GPIOD, ((data & 0x0f) | 0x80 | 0x20));			// set port to data value and EN pin high and RS pin high
	GPIO_PortSet(GPIOD, data & 0xff);			// set port to data value and EN pin high and RS pin high
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

//	GPIO_PortSet(GPIOD, ((data & 0x0f) | 0x20));				// set port to data value and EN pin low (transition again)
	GPIO_PortSet(GPIOD, data & 0xff);				// set port to data value and EN pin low (transition again)
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 0);

	GPIO_PinWrite(BOARD_LCD_RS_GPIO, BOARD_LCD_RS_PIN, 0);
	GPIO_PinWrite(BOARD_LCD_EN_GPIO, BOARD_LCD_EN_PIN, 1);
	vTaskDelay(pdMS_TO_TICKS(20));
}

void lcd_init()
{
	lcd_reset();
	lcd_command(MODULE_TYPE);
	lcd_command(DISP_ON_CURS_BLNK);
	lcd_command(0x06);
	lcd_command(0x80);
}

void lcd_reset()
{
	GPIO_PortSet(GPIOD, 0xff);
	vTaskDelay(pdMS_TO_TICKS(20));
	GPIO_PortSet(GPIOD, 0x03 + 0x80);
	GPIO_PortSet(GPIOD, 0x03);
	vTaskDelay(pdMS_TO_TICKS(10));
	GPIO_PortSet(GPIOD, 0x03 + 0x80);
	GPIO_PortSet(GPIOD, 0x03);
	vTaskDelay(pdMS_TO_TICKS(1));
	GPIO_PortSet(GPIOD, 0x03 + 0x80);
	GPIO_PortSet(GPIOD, 0x03);
	vTaskDelay(pdMS_TO_TICKS(1));
	GPIO_PortSet(GPIOD, 0x02 + 0x80);
	GPIO_PortSet(GPIOD, 0x02);
	vTaskDelay(pdMS_TO_TICKS(1));
}
