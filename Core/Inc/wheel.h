/*
 * wheel.h
 *
 *  Created on: Oct 19, 2021
 *      Author: evanl
 */

#ifndef INC_WHEEL_H_
#define INC_WHEEL_H_
#include "stm32f4xx.h"

typedef enum{
	wheel_ok = 0,
	wheel_error
}wheel_status_t;


typedef struct{
	TIM_HandleTypeDef *HTIMx;
	int32_t scroll_counter;
}wheel_t;

wheel_status_t wheel_init(wheel_t *wheel);

void wheel_read(wheel_t *wheel);

void wheel_reset_count(wheel_t *wheel);

#endif /* INC_WHEEL_H_ */
