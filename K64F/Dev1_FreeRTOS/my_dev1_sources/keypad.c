/*
 * keypad.c
 *
 *  Created on: 12 Dec 2019
 *      Author: zedd
 */

#include "FreeRTOS.h"
#include "task.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"

char scanKeypad();

void keypadTask(void* pvParameters)
{
	char pressedKey = '\0';

	for(;;)
	{
		pressedKey = scanKeypad();

		PRINTF("\n\rPressed key: %c", pressedKey);

		vTaskDelay(pdMS_TO_TICKS(150));
	}
}

char scanKeypad() {

	char pressedKey = '\0';
	bool isKeyPressed = false;

	// scan the first row
	GPIO_PinWrite(BOARD_KEY_ROW1_GPIO, BOARD_KEY_ROW1_PIN, 1);
	if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '1';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '2';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '3';
	}
	GPIO_PinWrite(BOARD_KEY_ROW1_GPIO, BOARD_KEY_ROW1_PIN, 0);

	// scan the second row
	GPIO_PinWrite(BOARD_KEY_ROW2_GPIO, BOARD_KEY_ROW2_PIN, 1);
	if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '4';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '5';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '6';
	}
	GPIO_PinWrite(BOARD_KEY_ROW2_GPIO, BOARD_KEY_ROW2_PIN, 0);

	// scan the third row
	GPIO_PinWrite(BOARD_KEY_ROW3_GPIO, BOARD_KEY_ROW3_PIN, 1);
	if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '7';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '8';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '9';
	}
	GPIO_PinWrite(BOARD_KEY_ROW3_GPIO, BOARD_KEY_ROW3_PIN, 0);

	// scan the fourth row
	GPIO_PinWrite(BOARD_KEY_ROW4_GPIO, BOARD_KEY_ROW4_PIN, 1);
	if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '*';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '0';
	}
	else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
		isKeyPressed = true;
		pressedKey = '#';
	}
	GPIO_PinWrite(BOARD_KEY_ROW4_GPIO, BOARD_KEY_ROW4_PIN, 0);

	return pressedKey;
}
