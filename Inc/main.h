/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CRSF_EN_Pin GPIO_PIN_13
#define CRSF_EN_GPIO_Port GPIOC
#define KEY_POWER_Pin GPIO_PIN_14
#define KEY_POWER_GPIO_Port GPIOC
#define POWER_EN_Pin GPIO_PIN_15
#define POWER_EN_GPIO_Port GPIOC
#define PA_TXRX_EN_Pin GPIO_PIN_5
#define PA_TXRX_EN_GPIO_Port GPIOA
#define SX1280_RST_Pin GPIO_PIN_7
#define SX1280_RST_GPIO_Port GPIOA
#define CHRG_IN_Pin GPIO_PIN_0
#define CHRG_IN_GPIO_Port GPIOB
#define KEY_SETUP_Pin GPIO_PIN_1
#define KEY_SETUP_GPIO_Port GPIOB
#define SX1280_BUSY_Pin GPIO_PIN_2
#define SX1280_BUSY_GPIO_Port GPIOB
#define SX1280_DIO1_Pin GPIO_PIN_10
#define SX1280_DIO1_GPIO_Port GPIOB
#define SX1280_DIO1_EXTI_IRQn EXTI15_10_IRQn
#define RGB_OUT_Pin GPIO_PIN_11
#define RGB_OUT_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define KEY_BIND_Pin GPIO_PIN_8
#define KEY_BIND_GPIO_Port GPIOA
#define CRSF_TX_Pin GPIO_PIN_9
#define CRSF_TX_GPIO_Port GPIOA
#define CRSF_RX_Pin GPIO_PIN_10
#define CRSF_RX_GPIO_Port GPIOA
#define EXTERNAL_RF_EN_Pin GPIO_PIN_15
#define EXTERNAL_RF_EN_GPIO_Port GPIOA
#define INTERNAL_RF_EN_Pin GPIO_PIN_3
#define INTERNAL_RF_EN_GPIO_Port GPIOB
#define SWB_H_Pin GPIO_PIN_4
#define SWB_H_GPIO_Port GPIOB
#define SWB_L_Pin GPIO_PIN_5
#define SWB_L_GPIO_Port GPIOB
#define SWA_Pin GPIO_PIN_6
#define SWA_GPIO_Port GPIOB
#define SWC_H_Pin GPIO_PIN_7
#define SWC_H_GPIO_Port GPIOB
#define SWD_Pin GPIO_PIN_8
#define SWD_GPIO_Port GPIOB
#define SWC_L_Pin GPIO_PIN_9
#define SWC_L_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define THROTTLE_ENABLED				1					// 1 for enabled, 0 for disabled
#define RUDDER_ENABLED					1					// 1 for enabled, 0 for disabled
#define JOYSTICK_AXIS_NUM				3
#define SECONDARY_AXIS_NUM				3

#define BUTTONS_NUM					    0
#define LEDS_NUM						0
#define BATTERY_CHECK_NUM               1
#define JOYSTICK_AXIS_ENABLED  	        1
#define SECONDARY_AXIS_ENABLED  	    1
#define BUTTONS_ENABLED  	            0
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
