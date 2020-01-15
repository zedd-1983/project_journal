/*
 * liquid_crystal.h
 *
 *  Created on: 13 Dec 2019
 *      Author: zedd
 */

#ifndef LIQUID_CRYSTAL_H_
#define LIQUID_CRYSTAL_H_

#include "pin_mux.h"

#define RS BOARD_LCD_RS_PIN			///< PTB9, Instruction Mode = 0, Data Mode = 1
#define RW BOARD_LCD_RW_PIN 		///< PTA1, Read = 1, Write = 0
#define EN BOARD_LCD_EN_PIN 		///< PTB23, High-to-Low transition executes

#define D4 BOARD_LCD_D4_PIN 		///< PTD0
#define D5 BOARD_LCD_D5_PIN 		///< PTD1
#define D6 BOARD_LCD_D6_PIN 		///< PTD2
#define D7 BOARD_LCD_D7_PIN 		///< PTD3

#define MODULE_TYPE 		0x28	///< 16x2, 7x5 matrix display, 4-bit mode
#define DISP_ON_CURS_OFF 	0x0C	///< Display ON, Cursor OFF
#define DISP_ON_CURS_BLNK	0x0F	///< Display ON, Cursor Blinking
#define DISPLAY_CLEAN 		0x01	///< wipe display
#define	TOP_LEFT 			0x80	///< cursor in first row, first position
#define BOTTOM_LEFT 		0xC0	///< cursor in second row, first position

void lcd_data(unsigned char);
void lcd_command(unsigned char);
void lcd_init();
void lcd_reset();
void lcdTask(void*);

#endif /* LIQUID_CRYSTAL_H_ */
