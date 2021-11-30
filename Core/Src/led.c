/*
 * led.c
 *
 *  Created on: Nov 1, 2021
 *      Author: evanl
 */
#include "led.h"
#include "main.h"
#include "stm32f4xx_hal_tim.h"



typedef enum{
	RAMP_UP=0,
	RAMP_DOWN
}ramp_dir;

typedef enum{
	init=0,
	red,
	redgreen,
	green,
	greenblue,
	blue,
	bluered
}led_state_var;

static int16_t ticks = 0;
static ramp_dir direction = RAMP_UP;
static led_state_var state = init;
#define TICKS_MAX 5000

static void set_pwm_channel_dutycycle(TIM_HandleTypeDef* htim,uint8_t channel, uint8_t duty_cycle);
static void increment_clock(uint8_t ramp_time);

led_status_t led_init(led_t* led){
	if(HAL_TIM_PWM_Start(led->HTIMx, LED_RED_CH) == HAL_ERROR){
		return LED_ERROR;
	}

	if(HAL_TIM_PWM_Start(led->HTIMx, LED_GREEN_CH) == HAL_ERROR){
		return LED_ERROR;
	}

	if(HAL_TIM_PWM_Start(led->HTIMx, LED_BLUE_CH) == HAL_ERROR){
		return LED_ERROR;
	}
	return LED_OK;
}





led_status_t led_state_machine_update(led_t* led){
	uint8_t color_to_duty = 0;
	uint16_t max_ticks = (led->pulse_length * TICKS_MAX) / 255; //number that the clock counts to
	switch(led->led_pulse_mode){
		case SINGLE_COLOR:
				//set duty cycle of each channel to led channel parameters
			set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, led->red_pwm);
			set_pwm_channel_dutycycle(led->HTIMx, LED_GREEN_CH, led->green_pwm);
			set_pwm_channel_dutycycle(led->HTIMx, LED_BLUE_CH, led->blue_pwm);
			break;
		case SINGLE_COLOR_PULSE:
			//ramp up to channel parameters, then ramp down
			color_to_duty = (led->red_pwm * ticks) / max_ticks;
			set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, color_to_duty);

			color_to_duty = (led->green_pwm * ticks) / max_ticks;
			set_pwm_channel_dutycycle(led->HTIMx, LED_GREEN_CH, color_to_duty);

			color_to_duty = (led->blue_pwm * ticks) / max_ticks;
			set_pwm_channel_dutycycle(led->HTIMx, LED_BLUE_CH, color_to_duty);

			increment_clock(led->pulse_length);
			break;
		case MULTI_COLOR_CYCLE:
				//init
				switch(state){
					case init:
						ticks = 1;
						direction = RAMP_UP;
						led->red_pwm = 255;
						led->green_pwm = 255;
						led->blue_pwm = 255;
						state = redgreen;
						set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, 255);
						break;
					case red:
						//turn off blue
						color_to_duty = (led->blue_pwm * ticks) / max_ticks;
						set_pwm_channel_dutycycle(led->HTIMx, LED_BLUE_CH, color_to_duty);
						increment_clock(led->pulse_length);
	                    if(direction == RAMP_UP){
	                        state = redgreen;
	                    }
						break;
					case redgreen:
						color_to_duty = (led->green_pwm * ticks) / max_ticks;
						set_pwm_channel_dutycycle(led->HTIMx, LED_GREEN_CH, color_to_duty);
						increment_clock(led->pulse_length);
	                    if(direction == RAMP_DOWN){
	                        state = green;
	                    }
						break;
					case green:
						//turn off red
						color_to_duty = (led->red_pwm * ticks) / max_ticks;
						set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, color_to_duty);
						increment_clock(led->pulse_length);
	                    if(direction == RAMP_UP){
	                        state = greenblue;
	                    }
						break;
					case greenblue:
						//turn on blue
						color_to_duty = (led->blue_pwm * ticks) / max_ticks;
						set_pwm_channel_dutycycle(led->HTIMx, LED_BLUE_CH, color_to_duty);
						increment_clock(led->pulse_length);
	                    if(direction == RAMP_DOWN){
	                        state = blue;
	                    }
						break;
					case blue:
						//turn off green
						color_to_duty = (led->green_pwm * ticks) / max_ticks;
						set_pwm_channel_dutycycle(led->HTIMx, LED_GREEN_CH, color_to_duty);
						increment_clock(led->pulse_length);
	                    if(direction == RAMP_UP){
	                        state = bluered;
	                    }
						break;
					case bluered:
						//turn on red
						color_to_duty = (led->red_pwm * ticks) / max_ticks;
						set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, color_to_duty);
						increment_clock(led->pulse_length);
	                    if(direction == RAMP_DOWN){
	                        state = red;
	                    }
						break;

				}
			break;
		case MULTI_COLOR_CYCLE_PULSE:
			//init
			switch(state){
				case init:
					ticks = 1;
					direction = RAMP_UP;
					led->red_pwm = 255;
					led->green_pwm = 255;
					led->blue_pwm = 255;
					state = red;
					break;
				case red:
					//turn on red
					color_to_duty = (led->red_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, color_to_duty);
					increment_clock(led->pulse_length);
                    if(direction == RAMP_UP && ticks == 1){
                        state = redgreen;
                    }
					break;
				case redgreen:
					//turn on red and green
					color_to_duty = (led->red_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, color_to_duty);
					color_to_duty = (led->green_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_GREEN_CH, color_to_duty);
					increment_clock(led->pulse_length);
                    if(direction == RAMP_UP && ticks == 1){
                        state = green;
                    }
					break;
				case green:
					//turn off red
					color_to_duty = (led->green_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_GREEN_CH, color_to_duty);
					increment_clock(led->pulse_length);
                    if(direction == RAMP_UP && ticks == 1){
                        state = greenblue;
                    }
					break;
				case greenblue:
					//turn on blue and blue
					color_to_duty = (led->green_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_GREEN_CH, color_to_duty);
					color_to_duty = (led->blue_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_BLUE_CH, color_to_duty);
					increment_clock(led->pulse_length);
                    if(direction == RAMP_UP && ticks == 1){
                        state = blue;
                    }
					break;
				case blue:
					//turn on blue
					color_to_duty = (led->blue_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_BLUE_CH, color_to_duty);
					increment_clock(led->pulse_length);
                    if(direction == RAMP_UP && ticks == 1){
                        state = bluered;
                    }
					break;
				case bluered:
					//turn on red and blue
					color_to_duty = (led->red_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_RED_CH, color_to_duty);
					color_to_duty = (led->blue_pwm * ticks) / max_ticks;
					set_pwm_channel_dutycycle(led->HTIMx, LED_BLUE_CH, color_to_duty);
					increment_clock(led->pulse_length);
                    if(direction == RAMP_UP && ticks == 1){
                        state = red;
                    }
					break;

			}
			break;
	}
	return LED_OK;
}

static void set_pwm_channel_dutycycle(TIM_HandleTypeDef* htim, uint8_t channel, uint8_t duty_cycle){
	if(channel == TIM_CHANNEL_1){
		htim->Instance->CCR1 = duty_cycle;
	} else if (channel == TIM_CHANNEL_2){
		htim->Instance->CCR2 = duty_cycle;
	} else if (channel == TIM_CHANNEL_3){
		htim->Instance->CCR3 = duty_cycle;
	} else {
		htim->Instance->CCR4 = duty_cycle;
	}
}


static void increment_clock(uint8_t ramp_time){

	if (direction == RAMP_UP){
		ticks++; //increment clock
		if (ticks > (ramp_time * TICKS_MAX) / 255){
			direction = RAMP_DOWN;
			ticks = ticks - 2;
		}
	} else {
		ticks--;
		if(ticks < 0){
			direction = RAMP_UP;
			ticks = 1;
		}
	}
}
