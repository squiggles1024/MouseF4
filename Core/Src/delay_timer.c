/*
 * delay_timer.c
 *
 *  Created on: Oct 19, 2021
 *      Author: evanl
 */
#include <delay_timer.h>
#include <main.h>

extern TIM_HandleTypeDef US_DELAY_TIM;

void delay_us(uint16_t us){
	__HAL_TIM_SetCounter(&US_DELAY_TIM, 0);
	while(__HAL_TIM_GetCounter(&US_DELAY_TIM) < us);
}

