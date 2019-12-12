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
#include "keypad.h"

/// @brief Task reading values from keypad
/// @details This task scans through rows and columns and determines
/// what key was pressed and prints it out
/// 	 	|ROW  | Keypad Pin | K64F Pin |
///			|:---:|:----------:|:--------:|
/// 	 	|ROW1 | (K)        | PTB19    |
/// 		|ROW2 |(J)         |PTC1      |
///			|ROW3 |(H)         |PTC8      |
///			|ROW4 |(G)         |PTC9      |
/// 		|COL1 |(F)         |PTC5      |
///			|COL2 |(E)         |PTC7      |
///			|COL3 |(D)         |PTC0      |
void keypadTask(void* pvParameters)
{
	char pressedKey = '\0';

	for(;;)
	{
		pressedKey = scanKeypad();

		PRINTF("\rPressed key: %c", pressedKey);

		vTaskDelay(pdMS_TO_TICKS(150));
	}
}

/// @brief Function scans keypad for keys pressed
/// @details Rows of the keypad are connected to outputs of the
/// micro-controller while the columns to its inputs. This function
/// sets one of the outputs high and scans the inputs for HIGH value.
/// This value will be HIGH only if a key is pressed thus if the column
/// is HIGH and specific row is HIGH this function can determine what key
/// is pressed.
/// @return pressed key value (char)
char scanKeypad() {

	char pressedKey = '\0';
	bool keyWasPressed = false;

	if(keyWasPressed == false) {
		// scan the first row
		GPIO_PinWrite(BOARD_KEY_ROW1_GPIO, BOARD_KEY_ROW1_PIN, 1);
		if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '1';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '2';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '3';
		}
		GPIO_PinWrite(BOARD_KEY_ROW1_GPIO, BOARD_KEY_ROW1_PIN, 0);
	}

	if(keyWasPressed == false) {
		// scan the second row
		GPIO_PinWrite(BOARD_KEY_ROW2_GPIO, BOARD_KEY_ROW2_PIN, 1);
		if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '4';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '5';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '6';
	}
	GPIO_PinWrite(BOARD_KEY_ROW2_GPIO, BOARD_KEY_ROW2_PIN, 0);
	}

	if(keyWasPressed == false) {
		// scan the third row
		GPIO_PinWrite(BOARD_KEY_ROW3_GPIO, BOARD_KEY_ROW3_PIN, 1);
		if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '7';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '8';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '9';
		}
		GPIO_PinWrite(BOARD_KEY_ROW3_GPIO, BOARD_KEY_ROW3_PIN, 0);
	}

	if(keyWasPressed == false)
	{
		// scan the fourth row
		GPIO_PinWrite(BOARD_KEY_ROW4_GPIO, BOARD_KEY_ROW4_PIN, 1);
		if(GPIO_PinRead(BOARD_KEY_COL1_GPIO, BOARD_KEY_COL1_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '*';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL2_GPIO, BOARD_KEY_COL2_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '0';
		}
		else if(GPIO_PinRead(BOARD_KEY_COL3_GPIO, BOARD_KEY_COL3_PIN) == 1) {
			keyWasPressed = true;
			pressedKey = '#';
		}
		GPIO_PinWrite(BOARD_KEY_ROW4_GPIO, BOARD_KEY_ROW4_PIN, 0);
	}

	return pressedKey;
}
