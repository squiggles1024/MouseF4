/*
 * mouse.c
 *
 *  Created on: Oct 18, 2021
 *      Author: evanl
 */
#include "mouse.h"
#include "mouse_parts.h"
#include "wheel.h"
#include "button.h"
#include "sensor.h"
#include "eeprom.h"
#include "led.h"

typedef enum{
	mouse_button = 0,
	keyboard_key,
	mod_key
}key_type_t;

static mouse_status_t mAction_update_led();
static mouse_status_t mAction_read_wheel();
static mouse_status_t mAction_read_buttons();
static mouse_status_t mAction_read_sensor();
static mouse_status_t mAction_read_settings();
static mouse_status_t mAction_write_settings(uint8_t *new_settings);

static mouse_change_t mEvent_change_detected();
static mouse_change_t mEvent_key_change_detected();
static mouse_change_t mEvent_new_settings_flag_status();

static mouse_change_t change_detect_flag = MOUSE_NO_CHANGE;
static mouse_change_t key_change_detect_flag = MOUSE_NO_CHANGE;
mouse_change_t new_settings_flag = MOUSE_NO_CHANGE;
static uint8_t pressed_buttons = 0;

static void update_change_flag(uint8_t i);
static key_type_t decode_key_type(uint8_t index);
static void update_mouse_button_report(uint8_t index);
static void update_keyboard_button_report(uint8_t index);
static void update_mod_key_report(uint8_t index);
static void clear_keys();
static settings_status_t validate_settings();

static mouse_report_t mouse_report = {
	    //uint8_t report_id;
	    .button_reg=0,
	    .x_movement = 0,
		.y_movement = 0,
		.wheel_movement = 0
};

static mouse_key_report_t key_report = {
		//uint8_t report_id;
	    .modifiers = 0,
	    .reserved = 0,
	    .keycodes = {0}
};

mouse_status_t mouse_init(){

	//init wheel
	if(wheel_init(&mouse_wheel) == wheel_error){
		return MOUSE_ERROR;
	}
	//init led
	led_init(&tled);
	//initialize sensor
	if(sensor_init(&sensor) == sensor_error){
		return MOUSE_ERROR;
	}
	return MOUSE_OK;
}


static mouse_status_t mAction_update_led(){
	led_state_machine_update(&tled);
	return MOUSE_ERROR;
}

static mouse_status_t mAction_read_wheel(){

	//read the wheel
	wheel_read(&mouse_wheel);

	//send data to mouse report struct
	mouse_report.wheel_movement = (uint8_t)(0xFF & mouse_wheel.scroll_counter);

	//raise the change flag if it reports any non-zero number
	if(mouse_wheel.scroll_counter != 0){
		change_detect_flag = MOUSE_CHANGE;
	}

	//reset wheel back to 0
	wheel_reset_count(&mouse_wheel);
	return MOUSE_OK;
}

static mouse_status_t mAction_read_buttons(){

	//clear previously pressed keys
	clear_keys();

	//DEBUG ONLY REMOVE LATER
	//static const button_t* debug_ptr = buttons;
	//static const mouse_report_t *m_ptr = &mouse_report;
	//static const mouse_key_report_t *k_ptr = &key_report;
	//DEBUG ONLY REMOVE LATER

	button_state_t previous_state = button_error;

	for(uint8_t i = 0; i < TOTAL_MOUSE_BUTTONS; i++){

		//get the previous state
		previous_state = buttons[i].state;

		//update the button state
		if(button_read(&buttons[i]) == button_error){
			return MOUSE_ERROR;
		}

		//check if it changed
		if(buttons[i].state != previous_state){
			//update mouse or key change flag
			update_change_flag(i);
		}

		//decode the type of key
		key_type_t button_type = decode_key_type(i);

		//update the appropriate report
		if(button_type == mouse_button){
			update_mouse_button_report(i);
		} else if (button_type == mod_key){
			update_mod_key_report(i);
		} else if(button_type == keyboard_key){
			update_keyboard_button_report(i);
		} else {
			return MOUSE_ERROR;
		}

	}
	//reset number of pressed keys
	pressed_buttons = 0;
	return MOUSE_OK;
}

static mouse_status_t mAction_read_sensor(){
	sensor_update_position(&sensor);
	mouse_report.x_movement = sensor.motion_x;
	mouse_report.y_movement = sensor.motion_y;
	if(mouse_report.x_movement != 0 || mouse_report.y_movement != 0){
		change_detect_flag = MOUSE_CHANGE;
	}
	return MOUSE_OK;
}

//receive new settings buffer and update mouse settings
static mouse_status_t mAction_write_settings(uint8_t *new_settings){
	//update key codes
	for(uint8_t i = 0; i < TOTAL_MOUSE_BUTTONS; i++){
		button_keycodes[i] = new_settings[i];
	}
	//set new DPI
	uint16_t new_dpi = 0;
	new_dpi = (new_settings[DPI_HI_BYTE_INDEX] << 8) | new_settings[DPI_LO_BYTE_INDEX];
	sensor_setdpi(&sensor, new_dpi);
	//update LED Settings
	tled.led_pulse_mode = new_settings[LED_MODE_INDEX];
	tled.pulse_length = new_settings[LED_PULSE_LENGTH_INDEX];
	tled.red_pwm = new_settings[LED_RED_INDEX];
	tled.green_pwm = new_settings[LED_GREEN_INDEX];
	tled.blue_pwm = new_settings[LED_BLUE_INDEX];
	if(eeprom_write_bytes(&eeprom, new_settings, TOTAL_SETTINGS) == EEPROM_ERROR){
		return MOUSE_ERROR;
	}
	new_settings_flag = MOUSE_NO_CHANGE;
	return MOUSE_OK;
}

//read eeprom settings and update mouse settings
static mouse_status_t mAction_read_settings(){

	//fetch settings
	if(eeprom_read_bytes(&eeprom, settings, TOTAL_SETTINGS) == EEPROM_ERROR){
		return MOUSE_ERROR;
	}


	if(validate_settings() == SETTINGS_ERROR){
		return MOUSE_ERROR;
	}


	//update key codes with saved settings
	for(uint8_t i = 0; i < TOTAL_MOUSE_BUTTONS; i++){
		button_keycodes[i] = settings[i];
	}


	//set new DPI
	uint16_t new_dpi = (settings[DPI_HI_BYTE_INDEX] << 8) | settings[DPI_LO_BYTE_INDEX];
	sensor_setdpi(&sensor, new_dpi);
	//update LED Settings
	tled.led_pulse_mode = settings[LED_MODE_INDEX];
	tled.pulse_length = settings[LED_PULSE_LENGTH_INDEX];
	tled.red_pwm = settings[LED_RED_INDEX];
	tled.green_pwm = settings[LED_GREEN_INDEX];
	tled.blue_pwm = settings[LED_BLUE_INDEX];
	return MOUSE_OK;
}

static uint8_t *mAction_get_mouse_report(){
	return (uint8_t*)(&mouse_report);
}

static uint8_t *mAction_get_key_report(){
	return (uint8_t*)(&key_report);
}

static mouse_change_t mEvent_change_detected(){
	if(change_detect_flag == MOUSE_CHANGE){
		change_detect_flag = MOUSE_NO_CHANGE;
		return MOUSE_CHANGE;
	} else {
		return MOUSE_NO_CHANGE;
	}
}

static mouse_change_t mEvent_key_change_detected(){
	if(key_change_detect_flag == MOUSE_CHANGE){
		key_change_detect_flag = MOUSE_NO_CHANGE;
		return MOUSE_CHANGE;
	} else {
		return MOUSE_NO_CHANGE;
	}
}

static mouse_change_t mEvent_new_settings_flag_status(){
	return new_settings_flag;
}

static void update_change_flag(uint8_t i){
	if(button_keycodes[i] >= KEY_A && button_keycodes[i] <= KEY_RIGHTMETA){
		key_change_detect_flag = MOUSE_CHANGE;
	} else {
		change_detect_flag = MOUSE_CHANGE;
	}
}

static key_type_t decode_key_type(uint8_t index){
	uint8_t code = button_keycodes[index];
	if(code >= KEY_A && code <= KEY_KPDOT){
		return keyboard_key;
	} else if(code >= KEY_LEFTCTRL && code <= KEY_RIGHTMETA){
		return mod_key;
	} else if(code >= MOUSE_LEFT && code <= MOUSE_5){
		return mouse_button;
	}
	return MOUSE_ERROR;
}

static void update_mouse_button_report(uint8_t index){
	uint8_t code = button_keycodes[index];
	button_state_t state = buttons[index].state;
	if(state == button_pressed){
		mouse_report.button_reg |= MOUSE_BTN_LUT[MOUSE_KEY_MSK & code];
	} else {
		mouse_report.button_reg &= ~MOUSE_BTN_LUT[MOUSE_KEY_MSK & code];
	}
}

static void update_keyboard_button_report(uint8_t index){
	uint8_t code = button_keycodes[index];
	button_state_t state = buttons[index].state;
	if(state == button_pressed && pressed_buttons < TOTAL_MOUSE_BUTTONS){
		key_report.keycodes[pressed_buttons] = code;
		pressed_buttons++;
	}
}

static void update_mod_key_report(uint8_t index){
	uint8_t code = button_keycodes[index];
	button_state_t state = buttons[index].state;
	if(state == button_pressed){
		key_report.modifiers |= MOD_KEY_LUT[MOD_KEY_MSK & code];
	} else {
		key_report.modifiers &= ~MOD_KEY_LUT[MOD_KEY_MSK & code];
	}
}

static void clear_keys(){
	for(uint8_t i = 0; i < TOTAL_MOUSE_BUTTONS; i++){
		key_report.keycodes[i] = 0;
	}
}

const mouse_t mouse = {
 .update_led = mAction_update_led,
 .read_sensor = mAction_read_sensor,
 .read_wheel = mAction_read_wheel,
 .read_buttons = mAction_read_buttons,
 .write_settings =mAction_write_settings,
 .read_settings = mAction_read_settings,
 .get_mouse_report = mAction_get_mouse_report,
 .get_key_report = mAction_get_key_report
};


const mouse_events_t mouse_events = {
		.change_detected = mEvent_change_detected,
		.key_change_detected = mEvent_key_change_detected,
		.new_settings_flag_status = mEvent_new_settings_flag_status
};

static settings_status_t validate_settings(){
	uint8_t code;
	for(uint8_t i = 0; i < TOTAL_MOUSE_BUTTONS; i++){
			code = settings[i];
			if( !(  (code >= KEY_A && code <= KEY_KPDOT)  ||  (code >= KEY_LEFTCTRL && code <= KEY_RIGHTMETA)  ||  (code >= MOUSE_LEFT && code <= MOUSE_5)  ) ){
				return SETTINGS_ERROR;
			}
	}
	uint16_t new_dpi = (settings[DPI_HI_BYTE_INDEX] << 8) | settings[DPI_LO_BYTE_INDEX];
	if(new_dpi < DPI_MIN || new_dpi > DPI_MAX){
		return SETTINGS_ERROR;
	}

	if(settings[LED_MODE_INDEX] > MULTI_COLOR_CYCLE_PULSE || settings[LED_MODE_INDEX] < SINGLE_COLOR){
		return SETTINGS_ERROR;
	}

	//do not need to check pulse mode and colors. they're all uint8

	return SETTINGS_OK;
}



