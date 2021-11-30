/*
 * mouse_parts.h
 *
 *  Created on: Oct 18, 2021
 *      Author: evanl
 */

#ifndef INC_MOUSE_PARTS_H_
#define INC_MOUSE_PARTS_H_
#include "main.h"
#include "button.h"
#include "wheel.h"
#include "sensor.h"
#include "eeprom.h"
#include "led.h"
#include "mouse_keydefs.h"

extern TIM_HandleTypeDef MOUSEWHEEL_TIM;
extern SPI_HandleTypeDef MOUSESENSOR_SPI;
extern I2C_HandleTypeDef SETTINGS_EEPROM_I2C;
extern TIM_HandleTypeDef LED_TIM;

static button_t buttons[TOTAL_MOUSE_BUTTONS] = {
		{
				.GPIOx = MOUSE00_GPIO_Port,
				.pin_number = MOUSE00_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE01_GPIO_Port,
				.pin_number = MOUSE01_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE02_GPIO_Port,
				.pin_number = MOUSE02_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE03_GPIO_Port,
				.pin_number = MOUSE03_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE04_GPIO_Port,
				.pin_number = MOUSE04_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE05_GPIO_Port,
				.pin_number = MOUSE05_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE06_GPIO_Port,
				.pin_number = MOUSE06_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE07_GPIO_Port,
				.pin_number = MOUSE07_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE08_GPIO_Port,
				.pin_number = MOUSE08_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE09_GPIO_Port,
				.pin_number = MOUSE09_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE10_GPIO_Port,
				.pin_number = MOUSE10_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE11_GPIO_Port,
				.pin_number = MOUSE11_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE12_GPIO_Port,
				.pin_number = MOUSE12_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE13_GPIO_Port,
				.pin_number = MOUSE13_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE14_GPIO_Port,
				.pin_number = MOUSE14_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE15_GPIO_Port,
				.pin_number = MOUSE15_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE16_GPIO_Port,
				.pin_number = MOUSE16_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSE17_GPIO_Port,
				.pin_number = MOUSE17_Pin,
				.state = button_unpressed
		},
		{

				.GPIOx = MOUSE18_GPIO_Port,
				.pin_number = MOUSE18_Pin,
				.state = button_unpressed
		},
		{

				.GPIOx = MOUSE19_GPIO_Port,
				.pin_number = MOUSE19_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSELEFT_GPIO_Port,
				.pin_number = MOUSELEFT_Pin,
				.state = button_unpressed
		},
		{
				.GPIOx = MOUSERIGHT_GPIO_Port,
				.pin_number = MOUSERIGHT_Pin,
				.state = button_unpressed
		}
};

static uint8_t button_keycodes[TOTAL_MOUSE_BUTTONS] = {
		KEY_F1,
		KEY_F2,
		KEY_F3,
		MOUSE_3,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_LEFTSHIFT,
		KEY_LEFTCTRL,
		KEY_LEFTALT,
		MOUSE_4,
		MOUSE_5,
		KEY_F4,
		KEY_PAGEUP,
		KEY_PAGEDOWN,
		MOUSE_LEFT,
		MOUSE_RIGHT
};

typedef enum{
	SETTINGS_ERROR = 1,
	SETTINGS_OK = 0
}settings_status_t;

//format: first 22 button settings, then 2 bytes of DPI, LED mode, LED Colors
static uint8_t settings[TOTAL_SETTINGS] = {};

//const tri_led_t led;
static wheel_t mouse_wheel = {
		.scroll_counter = 0,
		.HTIMx = &MOUSEWHEEL_TIM
};

static sensor_t sensor = {
		.motion_x = 0,
		.motion_y = 0,
		.fetched_data = 0,
		.MT_GPIOx = MOTION_TRACK_GPIO_Port,
		.motion_track_pin = MOTION_TRACK_Pin,
		.CSN_GPIOx = SENSOR_CSN_GPIO_Port,
		.csn_pin = SENSOR_CSN_Pin,
		.HSPIx = &MOUSESENSOR_SPI
};

static led_t tled = {
		.HTIMx = &LED_TIM,
		.led_pulse_mode = SINGLE_COLOR_PULSE,
		.pulse_length = 100,
		.red_pwm = 0,
		.green_pwm = 255,
		.blue_pwm = 0
};

static eeprom_t eeprom = {
		.HI2Cx = &SETTINGS_EEPROM_I2C,
		.device_id = 0,
};


#endif /* INC_MOUSE_PARTS_H_ */
