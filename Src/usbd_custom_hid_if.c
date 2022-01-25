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
#include "radiolink.h"
#include "status.h"
#include "crsf.h"
#include "joystick.h"
#include "stdbool.h"
#include "common.h"
#if defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)
#include "common.h"
#endif    
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t USB_Recive_Buffer[64]; 
extern uint8_t MasterUID[6];
extern uint16_t BuzzerSwitch;
extern bool MasterUidUseChipIDFlag;
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
    0x26, 0x00,0x08,   /*LOGICAL_MAXIMUM (2048)*/
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
    0x26,0x00,0x08,     /*Logical Maximum(2048)*/
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
	0x26,0x00,0x08,	    /*LOGICAL_MAXIMUM (2048)*/
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
	0x26,0x00,0x08,	    /*LOGICAL_MAXIMUM (2048)*/
	0x75,0x10,          /*REPORT_SIZE (16)*/
	0x95,0x01,          /*REPORT_COUNT (3)*/
	0x81,0x02,          /*INPUT (Data,Var,Abs)*/
	0xc0,               /*End Collection*/
    
    0x09, 0x01,                    //   USAGE (Vendor Usage 1) 每个功能的一个卷标志
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)    表示每个传输数据限定�??0
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)    表示每个传输数据的最大�?�限定为255
    0x95, 0x40,                    //   REPORT_COUNT (64) 每次接收的数据长度，这里�??64�??
    0x75, 0x08,                    //   REPORT_SIZE (8)        传输字段的宽度为8bit，表示每个传输的数据范围�??0~ffff ffff
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
    uint16_t writeWord[10];
    for(int i=0;i<8;i++)
    {
        writeWord[i] = (uint16_t)USB_Recive_Buffer[i];
    }
    switch (USB_Recive_Buffer[0])
    {
        case CHANNEILS_INPUT_ID:
        {    
            STMFLASH_Write(CACHE_MIX_CHANNEL_INFO_ADDR+USB_Recive_Buffer[1]*8,&writeWord[2],4);
            mixUpdateFlag = 0x01;
            break;
        }
        case LITE_CONFIGER_INFO_ID:
        {
            uint16_t channelBuff[32];
            STMFLASH_Read(CACHE_MIX_CHANNEL_INFO_ADDR,channelBuff,32);
            STMFLASH_Write(MIX_CHANNEL_INFO_ADDR,channelBuff,32);  
            
            STMFLASH_Write(CONFIGER_INFO_ADDR,&writeWord[1],3); 
            HAL_NVIC_SystemReset();
            break;
        }
        
        case EXTRA_CUSTOM_CONFIG_ID:
        {
            if(USB_Recive_Buffer[1] == 1)
            {
                STMFLASH_Write(BuzzerSwitch_ADDR,&writeWord[2],1); 
                BuzzerSwitch = writeWord[2];
            }
            else if(USB_Recive_Buffer[1] == 2)
            {
                STMFLASH_Write(JoystickDeadZonePercent_ADDR,&writeWord[2],1); 
            }
        }
        
#if defined(LiteRadio_Plus_SX1280)    
        case INTERNAL_CONFIGER_INFO_ID:
        {
            if(writeWord[1] == 0x02)
            {
                internalCRSFdata.crsfParameter.power = writeWord[2];
                internalCRSFdata.crsfParameter.rate = writeWord[3];
                internalCRSFdata.crsfParameter.TLM = writeWord[4];
                tx_config.power = internalCRSFdata.crsfParameter.power;
                tx_config.rate = internalCRSFdata.crsfParameter.rate;
                tx_config.tlm = internalCRSFdata.crsfParameter.TLM;
            }
            break;  
        }
        case UID_BYTES_ID:
        {
            if(USB_Recive_Buffer[1]==0&&USB_Recive_Buffer[2]==0&&USB_Recive_Buffer[3]==0&&USB_Recive_Buffer[4]==0&&USB_Recive_Buffer[5]==0&&USB_Recive_Buffer[6]==0)
            {
                MasterUidUseChipIDFlag = true;
                uint16_t Writetemp[1]; 
                Writetemp[0] = MasterUidUseChipIDFlag;
                STMFLASH_Write(MasterUidUseChipIDFlag_ADDR,Writetemp,1);
            }
            else
            {
                MasterUID[0] = USB_Recive_Buffer[1];
                MasterUID[1] = USB_Recive_Buffer[2];
                MasterUID[2] = USB_Recive_Buffer[3];
                MasterUID[3] = USB_Recive_Buffer[4];
                MasterUID[4] = USB_Recive_Buffer[5];
                MasterUID[5] = USB_Recive_Buffer[6];
                
                MasterUidUseChipIDFlag = false;
                uint16_t Writetemp[1]; 
                Writetemp[0] = MasterUidUseChipIDFlag;
                STMFLASH_Write(MasterUidUseChipIDFlag_ADDR,Writetemp,1);
                STMFLASH_Write(MasterID1FromBindPhrase_ADDR,&writeWord[1],1);
                STMFLASH_Write(MasterID2FromBindPhrase_ADDR,&writeWord[2],1);
                STMFLASH_Write(MasterID3FromBindPhrase_ADDR,&writeWord[3],1);
                STMFLASH_Write(MasterID4FromBindPhrase_ADDR,&writeWord[4],1);
                STMFLASH_Write(MasterID5FromBindPhrase_ADDR,&writeWord[5],1);
                STMFLASH_Write(MasterID6FromBindPhrase_ADDR,&writeWord[6],1);
            }
        }
#endif   
        case EXTERNAL_CONFIGER_INFO_ID:
        {
            if(writeWord[1] == 0x01)
            {
                externalCRSFdata.configStatus = CONFIG_CRSF_ON;
                externalCRSFdata.configUpdateFlag = 0x01;
            }
            else if(writeWord[1] == 0x00)
            {
                externalCRSFdata.configStatus = CONFIG_CRSF_OFF;
            }
            else if(writeWord[1] == 0x02)
            {               
                externalCRSFdata.configSetFlag = 0x01;
                if(writeWord[6] == 0x01)
                {
                    externalCRSFdata.inBindingMode = 0x01;
                }
                else if(writeWord[7] == 0x01)
                {
                    externalCRSFdata.webUpdateMode = 0x01;
                }
                else
                {
                    uint8_t rate = 0,power = 0;
                    power = writeWord[2];
                    rate = writeWord[3];
                    externalCRSFdata.crsfParameter.TLM = writeWord[4];
                    switch (externalCRSFdata.regulatoryDomainIndex)
                    {
                        case NANO_TX_915Mhz:
                            switch(rate)
                            {
                                case 0:
                                    externalCRSFdata.crsfParameter.rate = FREQ_900_RATE_25HZ;
                                    break;
                                case 1:
                                    externalCRSFdata.crsfParameter.rate = FREQ_900_RATE_50HZ;
                                    break;
                                case 2:
                                    externalCRSFdata.crsfParameter.rate = FREQ_900_RATE_100HZ;
                                    break;
                                case 3:
                                    externalCRSFdata.crsfParameter.rate = FREQ_900_RATE_200HZ;
                                    break;
                                default:
                                    break;
                            }
                            
                            break;
                        case NANO_TX_2400Mhz:
                            switch(rate)
                            {
                                case 0:
                                    externalCRSFdata.crsfParameter.rate = FREQ_2400_RATE_50HZ;
                                    break;
                                case 1:
                                    externalCRSFdata.crsfParameter.rate = FREQ_2400_RATE_150HZ;
                                    break;
                                case 2:
                                    externalCRSFdata.crsfParameter.rate = FREQ_2400_RATE_250HZ;
                                    break;
                                case 3:
                                    externalCRSFdata.crsfParameter.rate = FREQ_2400_RATE_500HZ;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    switch(externalCRSFdata.regulatoryDomainIndex)
                    {
                        case NANO_TX_915Mhz:
                            switch(power)
                            {
                                case power100mw:
                                    externalCRSFdata.crsfParameter.power = power915Mhz100mw;
                                    break;
                                case power250mw:
                                    externalCRSFdata.crsfParameter.power = power915Mhz250mw;
                                    break;
                                case power500mw:
                                    externalCRSFdata.crsfParameter.power = power915Mhz500mw;
                                    break;
                                default:
                                    break;
                            }
                            break;
                            case NANO_TX_2400Mhz:
                                externalCRSFdata.crsfParameter.power = power;
                                break;
                            default:
                                break;
                    }
                    
                }
            }
            break;
        }
        case REQUEST_INFO_ID:
        {
            requestType1 = USB_Recive_Buffer[1];
            requestType2 = USB_Recive_Buffer[2];
            /*当请求停止时*/
            if(requestType1 == 0x00 && requestType2 == 0x01)
            {
                sendSpam = 0;
            }
            /*上位机连接正�?*/
            if(requestType1 == 0x00 && requestType2 == 0x03)
            {
                configFlag = 1;
            }
            break;
        }
        default:
            break;
    }

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

