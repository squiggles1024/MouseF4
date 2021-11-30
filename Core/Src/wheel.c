/*
 * wheel.c
 *
 *  Created on: Oct 19, 2021
 *      Author: evanl
 */
#include "wheel.h"
#include "stm32f4xx_hal_tim.h"

wheel_status_t wheel_init(wheel_t *wheel){
	if(HAL_TIM_Encoder_Start(wheel->HTIMx, TIM_CHANNEL_ALL) == HAL_OK){
		return wheel_ok;
	}
	return wheel_error;
}

void wheel_read(wheel_t *wheel){
	wheel->scroll_counter = wheel->HTIMx->Instance->CNT;
}

void wheel_reset_count(wheel_t *wheel){
	wheel->scroll_counter = 0;
	wheel->HTIMx->Instance->CNT = 0;
}
