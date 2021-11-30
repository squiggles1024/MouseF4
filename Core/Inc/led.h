/*
 * led.h
 *
 *  Created on: Nov 1, 2021
 *      Author: evanl
 */

#ifndef INC_LED_H_
#define INC_LED_H_
#include <stdint.h>
#include "stm32f4xx.h"

typedef enum{
	LED_OK,
	LED_ERROR
}led_status_t;

typedef enum{
	SINGLE_COLOR,
	SINGLE_COLOR_PULSE,
	MULTI_COLOR_CYCLE,
	MULTI_COLOR_CYCLE_PULSE
}led_pulse_mode_t;


typedef struct{
	led_pulse_mode_t led_pulse_mode;
	uint8_t pulse_length; //255 = on to off length of 5 sec (10 sec period)
	uint8_t red_pwm;
	uint8_t green_pwm;
	uint8_t blue_pwm;
	TIM_HandleTypeDef *HTIMx;
}led_t;

led_status_t led_init(led_t* led);
led_status_t led_state_machine_update(led_t* led);



#endif /* INC_LED_H_ */
