/*
 * eeprom.c
 *
 *  Created on: Oct 25, 2021
 *      Author: evanl
 */

#include "eeprom.h"
#include "main.h"

eeprom_status_t eeprom_write_bytes(eeprom_t* eeprom, uint8_t *buffer,uint8_t number_of_bytes){
	if(HAL_I2C_Mem_Write(eeprom->HI2Cx, eeprom_default_address | (eeprom->device_id), 0, I2C_MEMADD_SIZE_16BIT, buffer, number_of_bytes, EEPROM_TIMEOUT) == HAL_ERROR){
		return EEPROM_ERROR;
	}
	return EEPROM_OK;
}

eeprom_status_t eeprom_read_bytes(eeprom_t* eeprom,uint8_t *buffer,uint8_t number_of_bytes){
	if(HAL_I2C_Mem_Read(eeprom->HI2Cx, eeprom_default_address | (eeprom->device_id), 0, I2C_MEMADD_SIZE_16BIT, buffer, number_of_bytes, EEPROM_TIMEOUT) == HAL_ERROR){
		return EEPROM_ERROR;
	}
	return EEPROM_OK;
}
