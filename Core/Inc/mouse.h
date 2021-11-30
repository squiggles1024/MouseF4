/*
 * mouse.h
 *
 *  Created on: Oct 18, 2021
 *      Author: evanl
 */

#ifndef INC_MOUSE_H_
#define INC_MOUSE_H_
#include <stdint.h>
#include "main.h"

typedef enum{
	MOUSE_NO_CHANGE = 0,
	MOUSE_CHANGE
}mouse_change_t;

typedef enum{
	MOUSE_OK = 0,
	MOUSE_ERROR
}mouse_status_t;

typedef struct{
    //uint8_t report_id;
    uint8_t button_reg;
    int8_t x_movement;
    int8_t y_movement;
    int8_t wheel_movement;
}__attribute__((packed))mouse_report_t;

typedef struct{
	//uint8_t report_id;
    uint8_t modifiers;
    const uint8_t reserved;
    uint8_t keycodes[TOTAL_MOUSE_BUTTONS];
}__attribute__((packed))mouse_key_report_t;

typedef struct{
	mouse_status_t (*update_led)();
	mouse_status_t (*read_wheel)();
	mouse_status_t (*read_buttons)();
	mouse_status_t (*read_sensor)();
	mouse_status_t (*write_settings)(uint8_t *);
	mouse_status_t (*read_settings)();
	uint8_t* (*get_mouse_report)();
	uint8_t* (*get_key_report)();
}mouse_t;

typedef struct{
	mouse_change_t (*change_detected)();
	mouse_change_t (*key_change_detected)();
	mouse_change_t (*new_settings_flag_status)();
}mouse_events_t;

mouse_status_t mouse_init();


#endif /* INC_MOUSE_H_ */
