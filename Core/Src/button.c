/*
 * button.c
 *
 *  Created on: Oct 18, 2021
 *      Author: evanl
 */

#include "button.h"
#include "stm32f4xx_hal_gpio.h"

button_status_t button_read(button_t *button){

	if(HAL_GPIO_ReadPin(button->GPIOx, button->pin_number) == GPIO_PIN_SET){
		button->state = button_pressed;
		return button_ok;
	} else if (HAL_GPIO_ReadPin(button->GPIOx, button->pin_number) == GPIO_PIN_RESET){
		button->state = button_unpressed;
		return button_ok;
	}
	return button_error;
}

