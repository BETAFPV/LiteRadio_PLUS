/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v2.0_Cube
  * @brief          : This file implements the USB Device
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

/* Includes ------------------------------------------------------------------*/

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN Includes */
#include "adc.h"
#include "xinput.h"
#include "phoenix.h"
#include "stmflash.h"
#include "common.h"
#include "joystick.h"
#include "gimbal.h"

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */
    STMFLASH_Read(LITE_RADIO_HARDWARE_TYPE_ADDR, &liteRadioIndex, 1);
    if(HAL_GPIO_ReadPin(KEY_POWER_GPIO_Port, KEY_POWER_Pin) == GPIO_PIN_SET){
        if(liteRadioIndex == LITE_RADIO_1_CC2500){
            if(adc_value[0] < Sampling_MaxMinData[0][ADC_INPUT_ID_MIN]+200
            && adc_value[1] < Sampling_MaxMinData[1][ADC_INPUT_ID_MIN]+200
            && adc_value[2] < Sampling_MaxMinData[2][ADC_INPUT_ID_MIN]+200
            && adc_value[3] > Sampling_MaxMinData[3][ADC_INPUT_ID_MAX]-200){
                isXboxMode = 1;
            }
        }else{
            if(adc_value[0] > Sampling_MaxMinData[0][ADC_INPUT_ID_MAX]-200
            && adc_value[1] < Sampling_MaxMinData[1][ADC_INPUT_ID_MIN]+200
            && adc_value[2] < Sampling_MaxMinData[2][ADC_INPUT_ID_MIN]+200
            && adc_value[3] < Sampling_MaxMinData[3][ADC_INPUT_ID_MIN]+200){
                /* 向上的内八打杆进入凤凰模拟器 */
                isPhoenixMode = 1;
                phoenixSetDescLen();    // 修改原本的hid描述符长度
            }else if(adc_value[0] < Sampling_MaxMinData[0][ADC_INPUT_ID_MIN]+200
            && adc_value[1] < Sampling_MaxMinData[1][ADC_INPUT_ID_MIN]+200
            && adc_value[2] > Sampling_MaxMinData[2][ADC_INPUT_ID_MAX]-200
            && adc_value[3] < Sampling_MaxMinData[3][ADC_INPUT_ID_MIN]+200){
                /* 向下的内八打杆进入xbox */
                isXboxMode = 1;
            }
        }
    }
  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CUSTOM_HID) != USBD_OK)
  {
    Error_Handler();
  }
  if(isPhoenixMode){
     if (USBD_CUSTOM_HID_RegisterInterface(&hUsbDeviceFS, &USBD_Pnx_fops_FS) != USBD_OK)
     {
        Error_Handler();
     }
  }else{
     if (USBD_CUSTOM_HID_RegisterInterface(&hUsbDeviceFS, &USBD_CustomHID_fops_FS) != USBD_OK)
     {
        Error_Handler();
     }
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  
  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
