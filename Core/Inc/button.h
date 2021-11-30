/*
 * button.h
 *
 *  Created on: Oct 18, 2021
 *      Author: evanl
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_
#include "stm32f4xx.h"

typedef enum{
	button_unpressed = 0,
	button_pressed
}button_state_t;

typedef enum{
	button_ok = 0,
	button_error
}button_status_t;

typedef struct{
	button_state_t state;
	GPIO_TypeDef* GPIOx;
	uint16_t pin_number;
}button_t;

button_status_t button_read(button_t *button);

#endif /* INC_BUTTON_H_ */
