#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"


void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void Error_Handler(void);

#define MOUSE00_Pin GPIO_PIN_13
#define MOUSE00_GPIO_Port GPIOC
#define MOUSE01_Pin GPIO_PIN_14
#define MOUSE01_GPIO_Port GPIOC
#define MOUSE02_Pin GPIO_PIN_15
#define MOUSE02_GPIO_Port GPIOC
#define MOUSE03_Pin GPIO_PIN_0
#define MOUSE03_GPIO_Port GPIOC
#define MOUSE04_Pin GPIO_PIN_1
#define MOUSE04_GPIO_Port GPIOC
#define MOUSE05_Pin GPIO_PIN_2
#define MOUSE05_GPIO_Port GPIOC
#define MOUSE06_Pin GPIO_PIN_3
#define MOUSE06_GPIO_Port GPIOC
#define MOUSE07_Pin GPIO_PIN_0
#define MOUSE07_GPIO_Port GPIOA
#define MOUSE08_Pin GPIO_PIN_1
#define MOUSE08_GPIO_Port GPIOA
#define MOUSE09_Pin GPIO_PIN_2
#define MOUSE09_GPIO_Port GPIOA
#define MOUSE10_Pin GPIO_PIN_3
#define MOUSE10_GPIO_Port GPIOA
#define SENSOR_CSN_GPIO_Port GPIOA
#define SENSOR_CSN_Pin GPIO_PIN_4
#define MOTION_TRACK_Pin GPIO_PIN_4
#define MOTION_TRACK_GPIO_Port GPIOC
#define MOUSE11_Pin GPIO_PIN_5
#define MOUSE11_GPIO_Port GPIOC
#define MOUSE12_Pin GPIO_PIN_0
#define MOUSE12_GPIO_Port GPIOB
#define MOUSE13_Pin GPIO_PIN_1
#define MOUSE13_GPIO_Port GPIOB
#define MOUSE14_Pin GPIO_PIN_2
#define MOUSE14_GPIO_Port GPIOB
#define MOUSE15_Pin GPIO_PIN_10
#define MOUSE15_GPIO_Port GPIOB
#define MOUSE16_Pin GPIO_PIN_12
#define MOUSE16_GPIO_Port GPIOB
#define MOUSE17_Pin GPIO_PIN_13
#define MOUSE17_GPIO_Port GPIOB
#define MOUSE18_Pin GPIO_PIN_14
#define MOUSE18_GPIO_Port GPIOB
#define MOUSE19_Pin GPIO_PIN_15
#define MOUSE19_GPIO_Port GPIOB
#define TIM3_CH1_LEDR_Pin GPIO_PIN_6
#define TIM3_CH1_LEDR_GPIO_Port GPIOC
#define TIM3_CH2_LEDG_Pin GPIO_PIN_7
#define TIM3_CH2_LEDG_GPIO_Port GPIOC
#define TIM3_CH3_LEDB_Pin GPIO_PIN_8
#define TIM3_CH3_LEDB_GPIO_Port GPIOC
#define MOUSERIGHT_Pin GPIO_PIN_11
#define MOUSERIGHT_GPIO_Port GPIOC
#define MOUSELEFT_Pin GPIO_PIN_12
#define MOUSELEFT_GPIO_Port GPIOC
#define TIM4_CH1_CLK_Pin GPIO_PIN_7
#define TIM4_CH1_CLK_GPIO_Port GPIOB
#define TIM4_CH2_DT_Pin GPIO_PIN_6
#define TIM4_CH2_DT_GPIO_Port GPIOB



#define LED_TIM htim3
#define MOUSEWHEEL_TIM htim4
#define MOUSESENSOR_SPI hspi1
#define US_DELAY_TIM htim6
#define SETTINGS_EEPROM_I2C hi2c1
//2 bytes for DPI, 1 byte for LED Cycle Mode, 1 byte for pulse length, 3 bytes for LED Colors,
#define TOTAL_MOUSE_BUTTONS (22U)
#define TOTAL_SETTINGS (TOTAL_MOUSE_BUTTONS + 2 + 1 + 1 + 3)
#define DPI_LO_BYTE_INDEX (TOTAL_MOUSE_BUTTONS)
#define DPI_HI_BYTE_INDEX (TOTAL_MOUSE_BUTTONS + 1)
#define LED_MODE_INDEX (TOTAL_MOUSE_BUTTONS + 2)
#define LED_PULSE_LENGTH_INDEX (TOTAL_MOUSE_BUTTONS + 3)
#define LED_RED_INDEX (TOTAL_MOUSE_BUTTONS + 4)
#define LED_GREEN_INDEX (TOTAL_MOUSE_BUTTONS + 5)
#define LED_BLUE_INDEX (TOTAL_MOUSE_BUTTONS + 6)

#define LED_RED_CH TIM_CHANNEL_1
#define LED_GREEN_CH TIM_CHANNEL_2
#define LED_BLUE_CH TIM_CHANNEL_3

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
