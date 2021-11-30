/*
 * eeprom.h
 *
 *  Created on: Oct 25, 2021
 *      Author: evanl
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_
#include <stdint.h>
#include "stm32f4xx.h"

#define eeprom_default_address (0xA0U)
#define eeprom_read_mask 0xFE
#define eeprom_write_mask 0x01
#define EEPROM_TIMEOUT 100U

typedef enum{
	EEPROM_OK = 0,
	EEPROM_ERROR = 1
}eeprom_status_t;

typedef struct{
	I2C_HandleTypeDef *HI2Cx;
	const uint8_t device_id; //device address bits
}eeprom_t;

eeprom_status_t eeprom_write_bytes(eeprom_t* eeprom, uint8_t *buffer,uint8_t number_of_bytes);
eeprom_status_t eeprom_read_bytes(eeprom_t* eeprom, uint8_t *buffer,uint8_t number_of_bytes);


#endif /* INC_EEPROM_H_ */
