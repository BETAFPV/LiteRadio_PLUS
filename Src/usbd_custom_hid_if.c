/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
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
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "mixes.h"
#include "stmflash.h"
#include "cmsis_os.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static uint8_t USB_Recive_Buffer[64]; 
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
    0x05,0x01,          /*Usage Page(Generic Desktop)*/ 
    0x09,0x04,          /*Usage(joystick)*/
    0xA1,0x01,          /*Collection(Application)*/
	//24
	0x05, 0x01,         /*USAGE_PAGE (Generic Desktop)*/
    0x09, 0x01,         /*USAGE (Pointer)*/
    0xa1, 0x00,         /*COLLECTION (Physical)*/
    0x09, 0x33,         /*USAGE (Rx)*/
    0x09, 0x34,         /*USAGE (Ry)*/
    0x09, 0x35,         /*USAGE (Rz)*/
    0x15, 0x00,         /*LOGICAL_MINIMUM (0)*/
    0x26, 0x00, 0x10,   /*LOGICAL_MAXIMUM (4096)*/
    0x75, 0x10,         /*REPORT_SIZE (16)*/
    0x95, 0x03,         /*REPORT_COUNT (3)*/
    0x81, 0x02,         /*INPUT (Data,Var,Abs)*/
	0xc0,               /*End Collection*/
	//24
    0x05,0x01,          /*USAGE_PAGE (Generic Desktop)*/
    0x09,0x01,          /*USAGE (Pointer)*/
    0xa1,0x00,          /*COLLECTION (Physical)*/
    0x09,0x30,          /*Usage(X axis)*/
    0x09,0x31,          /*Usage(Y axis)*/ 
    0x09,0x32,          /*Usage(Z axis)*/ 
    0x15,0x00,          /*Logical Minimum(0)*/
    0x26,0x00,0x10,     /*Logical Maximum(4096)*/
	0x95,0x03,          /*REPORT_COUNT(3)*/
    0x75,0x10,          /*REPORT_SIZE (16)*/
    0x81,0x02,          /*INPUT (Data,Var,Abs)*/
    0xc0,               /*End Collection*/
	//20
    0x05,0x01,          /*USAGE_PAGE (Generic Desktop*/
	0x09,0x36,			/*USAGE (Pointer)*/
	0xA1,0x00,			/*COLLECTION (Physical)*/
	0x09,0x36,          /*USAGE (Slider)*/
	0x15,0x00,			/*LOGICAL_MINIMUM (0)*/
	0x26,0x00,0x10,	    /*LOGICAL_MAXIMUM (4096)*/
	0x75,0x10,          /*REPORT_SIZE (16)*/
	0x95,0x01,          /*REPORT_COUNT (3)*/
	0x81,0x02,          /*INPUT (Data,Var,Abs)*/
	0xc0,               /*End Collection*/
    //20
    0x05,0x01,          /*USAGE_PAGE (Generic Desktop*/
	0x09,0x36,			/*USAGE (Pointer)*/
	0xA1,0x00,			/*COLLECTION (Physical)*/
	0x09,0x36,          /*USAGE (Slider)*/
	0x15,0x00,			/*LOGICAL_MINIMUM (0)*/
	0x26,0x00,0x10,	    /*LOGICAL_MAXIMUM (4096)*/
	0x75,0x10,          /*REPORT_SIZE (16)*/
	0x95,0x01,          /*REPORT_COUNT (3)*/
	0x81,0x02,          /*INPUT (Data,Var,Abs)*/
	0xc0,               /*End Collection*/
    
    0x09, 0x01,                    //   USAGE (Vendor Usage 1) 每个功能的一个卷标志
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)    表示每个传输数据限定�?0
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)    表示每个传输数据的最大�?�限定为255
    0x95, 0x40,                    //   REPORT_COUNT (64) 每次接收的数据长度，这里�?64�?
    0x75, 0x08,                    //   REPORT_SIZE (8)        传输字段的宽度为8bit，表示每个传输的数据范围�?0~ffff ffff
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs) 表示USB设备要接收PC的数据的功能
    
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
    char i;

    USBD_CUSTOM_HID_HandleTypeDef   *hhid;
    unsigned char USB_Received_Count = 0;
    USB_Received_Count = USBD_GetRxCount( &hUsbDeviceFS,CUSTOM_HID_EPOUT_ADDR ); 
    hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;
    
    for(i=0;i<USB_Received_Count;i++) 
    {
        USB_Recive_Buffer[i]=hhid->Report_buf[i]; 
    } 
    SaveMixValueToFlash();
    return (USBD_OK);
    
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
void SaveMixValueToFlash(void)
{
    uint16_t writeWord[8];
    for(int i=0;i<8;i++)
    {
        writeWord[i] = (uint16_t)USB_Recive_Buffer[i+1];
    }
    switch (USB_Recive_Buffer[0])
    {
        case CHANNEILS_INPUT_ID:
        {    
            STMFLASH_Write(MIX_CHANNEL_INPUT_ADDR,writeWord,8);
            break;
        }
        case CHANNEILS_REVERSE_ID:
        {
            STMFLASH_Write(MIX_CHANNEL_INVERSE_ADDR,writeWord,8);
            break;
        }
        case CHANNEILS_WEIGHT_ID:
        {
            STMFLASH_Write(MIX_CHANNEL_WEIGHT_ADDR,writeWord,8);
            break;
        }
        case CHANNEILS_OFFSET_ID:
        {
            STMFLASH_Write(MIX_CHANNEL_OFFSET_ADDR,writeWord,8);
            break;
        }
        case CONFIGER_INFO_ID:
        {
            STMFLASH_Write(CONFIGER_INFO_ADDR,writeWord,8);
            break;
        }
        default:
            break;
    }
    writeWord[0] = 0x01;
    STMFLASH_Write(CONFIGER_INFO_FLAG,writeWord,1);
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

