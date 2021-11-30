/*
 * sensor.c
 *
 *  Created on: Oct 19, 2021
 *      Author: evanl
 */
#include <sensor.h>
#include <main.h>
#include <sensor_firmware.h>
#include "delay_timer.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_gpio.h"

extern TIM_HandleTypeDef US_DELAY_TIM;
static void set_csn_high(sensor_t* sensor);
static void set_csn_low(sensor_t* sensor);
static sensor_status_t sensor_write(sensor_t* sensor,uint8_t address, uint8_t data);
static uint8_t sensor_read(sensor_t* sensor,uint8_t address);

static void sensor_srom_download(sensor_t* sensor);

sensor_status_t sensor_init(sensor_t* sensor){
	HAL_TIM_Base_Start(&US_DELAY_TIM);
    /*
    load the firmware using these steps: set CS high, set it low, set it high again. write 0x5A to Power_Up_Reset reg, wait 50ms at least
    Read regs Motion, Delta_X_L, Delta_X_H, Delta_Y_L, Delta_Y_H, Download SROM, config other registers
    */
	set_csn_high(sensor);
	set_csn_low(sensor);
	set_csn_high(sensor);

	uint8_t data_dummy = 0x5A;
	sensor_write(sensor,Power_Up_Reset,data_dummy);
	HAL_Delay(50);

	if(sensor_read(sensor, Motion) == sensor_error){
		return sensor_error;
	}
	if(sensor_read(sensor, Delta_X_L) == sensor_error){
		return sensor_error;
	}
	if(sensor_read(sensor, Delta_X_H) == sensor_error){
		return sensor_error;
	}
	if(sensor_read(sensor, Delta_Y_L) == sensor_error){
		return sensor_error;
	}
	if(sensor_read(sensor, Delta_Y_H) == sensor_error){
		return sensor_error;
	}

    sensor_srom_download(sensor);

    HAL_Delay(10);
   sensor_setdpi(sensor, DEFAULT_DPI);



	return sensor_ok;
}

static void set_csn_high(sensor_t* sensor){
	HAL_GPIO_WritePin(sensor->CSN_GPIOx, sensor->csn_pin, GPIO_PIN_SET);
}

static void set_csn_low(sensor_t* sensor){
	HAL_GPIO_WritePin(sensor->CSN_GPIOx, sensor->csn_pin, GPIO_PIN_RESET);
}

static sensor_status_t sensor_write(sensor_t* sensor,uint8_t address, uint8_t cmd){
	uint8_t write_address = address | WRITE_MODE_MSK;
	set_csn_low(sensor);

	if(HAL_SPI_Transmit(sensor->HSPIx, &write_address, 1, TIMEOUT) == HAL_TIMEOUT){
		return sensor_error;
	}

	if(HAL_SPI_Transmit(sensor->HSPIx, &cmd, 1, TIMEOUT)== HAL_TIMEOUT){
		return sensor_error;
	}

    delay_us(20);
    set_csn_high(sensor);
    delay_us(50);
    return sensor_ok;
}

static sensor_status_t sensor_read(sensor_t* sensor,uint8_t address){
    uint8_t data = 0;
    uint8_t read_address = address & READ_MODE_MSK;
    set_csn_low(sensor);
    if(HAL_SPI_Transmit(sensor->HSPIx, &read_address, 1, TIMEOUT) == HAL_TIMEOUT){
    	return sensor_error;
    }
    delay_us(50);
    if(HAL_SPI_Receive(sensor->HSPIx, &data, 1, TIMEOUT) == HAL_TIMEOUT){
    	return sensor_error;
    }
    delay_us(1);
    set_csn_high(sensor);
    delay_us(19);
    sensor->fetched_data = data;
    return sensor_ok;
}

static void sensor_srom_download(sensor_t* sensor){
	uint8_t dummy = 0x20;
    sensor_write(sensor,Config2,dummy);

    dummy = 0x1d;
    sensor_write(sensor,SROM_Enable, dummy);

    HAL_Delay(10);

    dummy = 0x18;
    sensor_write(sensor,SROM_Enable, dummy);

    set_csn_low(sensor);

    dummy = SROM_Load_Burst | 0x80;
    HAL_SPI_Transmit(sensor->HSPIx,&dummy,1,TIMEOUT);

    delay_us(15);
    for(uint32_t i = 0; i < firmware_length; i++){
        dummy = firmware_data[i];
        HAL_SPI_Transmit(sensor->HSPIx, &dummy,1,TIMEOUT);
        delay_us(15);
    }
    set_csn_high(sensor);
}

sensor_status_t sensor_update_position(sensor_t* sensor){

	if(HAL_GPIO_ReadPin(sensor->MT_GPIOx, sensor->motion_track_pin) != MOTION_DETECTED){
		sensor->motion_x = 0;
		sensor->motion_y = 0;
		return sensor_ok;
	}

	if(sensor_write(sensor,Motion,0x1) == sensor_error){
		return sensor_error;
	}

	if(sensor_read(sensor,Motion) == sensor_error){
		return sensor_error;
	}

	if(sensor_read(sensor,Delta_X_L) == sensor_error){
		return sensor_error;
	} else {
		sensor->motion_x = sensor->fetched_data;
	}

	if(sensor_read(sensor,Delta_X_H) == sensor_error){
		return sensor_error;
	}

	if(sensor_read(sensor,Delta_Y_L) == sensor_error){
		return sensor_error;
	} else {
		sensor->motion_y = sensor->fetched_data;
	}

	if(sensor_read(sensor,Delta_Y_H) == sensor_error){
		return sensor_error;
	}

	return sensor_ok;
}

sensor_status_t sensor_setdpi(sensor_t* sensor, uint16_t DPI){
	if(DPI < DPI_MIN || DPI > DPI_MAX){
		return sensor_error;
	}
	if(DPI % 100 != 0){
		return sensor_error;
	}
	if(sensor_write(sensor, Config1, (DPI - 100) / 100) == sensor_error){
		return sensor_error;
	}
	return sensor_ok;
}

