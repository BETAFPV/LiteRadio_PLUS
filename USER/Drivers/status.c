#include "status.h"
#include "gimbal.h"
#include "key.h"
#include "rgb.h"
#include "power_switch.h"
#include "radiolink.h"
#include "stmflash.h"
#include "joystick.h"
#include "mixes.h"
#include "buzzer.h"
#include "crsf.h"
#include "tim.h"
#include "adc.h"
#include "gimbal.h"
#include "function.h"
#include "common.h" 

uint8_t requestType1;
uint8_t requestType2;
uint8_t configFlag;

static uint64_t statusNowTick;
static uint64_t statusLastTick;
static uint16_t protocolIndex;
static uint32_t protocolDelayTime;
static uint8_t RCstatus = RC_SHUTDOWN;
static uint8_t lastRCstatus = RC_SHUTDOWN;
static uint8_t RFstatus = RF_DATA;
static uint8_t powerStatus = RC_POWER_OFF;
static uint64_t lowElectricityNowTick;
static uint64_t lowElectricityLastTick;

static uint16_t electricityADCvalue;
static uint16_t calADCvalue;
uint16_t upElectricityLimit = 3300;
uint16_t downElectricityLimit = 3300;
uint8_t batteryWarningStatus = 1;

void Status_Init()
{
    STMFLASH_Read(CONFIGER_INFO_ADDR,&protocolIndex,1);
    if(protocolIndex > PROTOCOL_INDEX_LIMIT)
    {
#if defined(LiteRadio_Plus_CC2500) 
        protocolIndex = 2;
#elif defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)
		protocolIndex = 0;
#endif
        
        STMFLASH_Write(CONFIGER_INFO_ADDR,&protocolIndex,1);
    }
    
    if(HAL_GPIO_ReadPin(KEY_BIND_GPIO_Port,KEY_BIND_Pin) == GPIO_PIN_RESET)
    {
        protocolIndex++;
        if(protocolIndex > PROTOCOL_INDEX_LIMIT)
        {
            protocolIndex = 0;
        }

        STMFLASH_Write(CONFIGER_INFO_ADDR,&protocolIndex,1);
    }
    if(HAL_GPIO_ReadPin(KEY_POWER_GPIO_Port,KEY_POWER_Pin) == GPIO_PIN_RESET)
    {
#if defined(LiteRadio_Plus_CC2500) 
        RGB_TwinkleForInit((protocolIndex+1),180);
#elif defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)
		RGB_TwinkleForInit((protocolIndex+1),200);
#endif
    }
 
    Version_Init(protocolIndex);
    switch(protocolIndex)
    {
#if defined(LiteRadio_Plus_CC2500)         
        case 0:
        {
            protocolDelayTime = D16_INTERVAL;
            HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);        
            break;
        }
        case 1: 
        {
            protocolDelayTime = D16_INTERVAL;
            HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);
            break;
        }
        case 2: 
        {
            protocolDelayTime = D8_INTERVAL;
            HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);
            break;
        }
        case 3: 
        {
            protocolDelayTime = SFHSS_INTERVAL;
            HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);
            break;
        }
        case 4: 
        {
            protocolDelayTime = CRSF_INTERVAL;
            HAL_GPIO_WritePin(EXTERNAL_RF_EN_GPIO_Port, EXTERNAL_RF_EN_Pin, GPIO_PIN_SET);                  
            break;
        } 
#elif defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)   
        case 0: 
        {
            protocolDelayTime = CRSF_INTERVAL;
            HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);     
            setup();     
            break;
        } 
        case 1:
        {
            protocolDelayTime = CRSF_INTERVAL;
            HAL_GPIO_WritePin(EXTERNAL_RF_EN_GPIO_Port, EXTERNAL_RF_EN_Pin, GPIO_PIN_SET);             
            break;
        } 
#endif        
        default:
        {
            break;
        }
    }

}

uint32_t Get_ProtocolDelayTime()
{
    return protocolDelayTime;
}

uint8_t Get_ProtocolIndex()
{
    uint8_t currProtocolIndex = (uint8_t)protocolIndex;
    return currProtocolIndex;
}

void Status_Update()
{
    if(powerStatus == RC_POWER_ON)
    {
       if(protocolIndex == CRSF_PROTOCOL_INDEX)
       {
           HAL_GPIO_WritePin(EXTERNAL_RF_EN_GPIO_Port, EXTERNAL_RF_EN_Pin, GPIO_PIN_SET);
       }
        RCstatus = RC_RADIOLINK;
        if(lastRCstatus == RC_SHUTDOWN)
        {
            vTaskResume(radiolinkTaskHandle);
#if defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)              
            if(protocolIndex != CRSF_PROTOCOL_INDEX)
            {
                HAL_TIM_Base_Start_IT(&htim1);
            }
#endif
        }
        if(lastRCstatus == RC_CHRG_AND_JOYSTICK)
        {
            vTaskSuspend(joystickTaskHandle); 
            vTaskResume(radiolinkTaskHandle);
#if defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)              
            if(protocolIndex != CRSF_PROTOCOL_INDEX)
            {
                //HAL_TIM_Base_Start_IT(&htim1);
            }
#endif
        }
    }
    if(powerStatus == RC_POWER_OFF)
    {		
        if(protocolIndex == CRSF_PROTOCOL_INDEX)
        {
            HAL_GPIO_WritePin(EXTERNAL_RF_EN_GPIO_Port, EXTERNAL_RF_EN_Pin, GPIO_PIN_RESET);   
        }
        RCstatus = RC_CHRG_AND_JOYSTICK;
        if(lastRCstatus == RC_SHUTDOWN)
        {
            vTaskResume(joystickTaskHandle);
        }
        if(lastRCstatus == RC_RADIOLINK)
        {
#if defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)              
            if(protocolIndex != CRSF_PROTOCOL_INDEX)
            {
                HAL_TIM_Base_Stop_IT(&htim1);
            }
#endif            
            vTaskSuspend(radiolinkTaskHandle);
            vTaskResume(joystickTaskHandle); 
        }
    }
    lastRCstatus = RCstatus;
}



void statusTask(void* param)
{

    EventBits_t keyEvent;
    EventBits_t gimbalEvent;

    while(1)
    {
        vTaskDelay(1);
        
        /*低电量警告*/
        /*使用影响的通道值做补偿*/
        calADCvalue = Get_AdcValue(3);
        if(  calADCvalue > 1500)
        {
            electricityADCvalue = Get_AdcValue(4);
            electricityADCvalue -= (calADCvalue - 1500)*110/1500;
        }
        else
        {
            electricityADCvalue = Get_AdcValue(4);
            electricityADCvalue += (1500 - calADCvalue)*110/1500;
        }
        batteryWarningStatus = Cal_ElectricRelay(electricityADCvalue,batteryWarningStatus,downElectricityLimit,upElectricityLimit);
        
		if(powerStatus == RC_POWER_ON)
		{
			if(batteryWarningStatus == DOWN_VALUE_STATUS)
			{
				lowElectricityNowTick = HAL_GetTick();
				if((lowElectricityNowTick - lowElectricityLastTick) > 20000)
				{
					xEventGroupSetBits(buzzerEventHandle,LOW_ELECTRICITY_RING);
					xEventGroupSetBits( rgbEventHandle, LOW_ELECTRICITY_RGB);
					lowElectricityLastTick = lowElectricityNowTick;
				}
			}
		}
        
        
        if(lastRCstatus == RC_INIT)
        {
            osDelay(3000);
            lastRCstatus = RC_SHUTDOWN;
        }
        else if(lastRCstatus == RC_SHUTDOWN)
        {
            Status_Update();
        }
        if(configFlag)
        {
            statusNowTick = HAL_GetTick();
            if((statusNowTick - statusLastTick) > 1000)
            {
               configFlag = 0; 
            }
            statusLastTick = statusNowTick;    
        }
        keyEvent= xEventGroupWaitBits( KeyEventHandle,
		                               POWERSWITCH_LONG_PRESS|BIND_SHORT_PRESS|SETUP_SHORT_PRESS,
		                               pdTRUE,
	                                   pdFALSE,
		                               0);  
        
        if((keyEvent & POWERSWITCH_LONG_PRESS) == POWERSWITCH_LONG_PRESS)
        {    
            if(powerStatus)
            {
                powerStatus = RC_POWER_OFF;
                xEventGroupSetBits( buzzerEventHandle, POWER_OFF_RING);
                xEventGroupSetBits( rgbEventHandle, POWER_OFF_RGB);
                osDelay(1200);
                POWER_PIN_HOLD_DOWN();
            }
            else
            {
                powerStatus = RC_POWER_ON;
                xEventGroupSetBits( buzzerEventHandle, POWER_ON_RING);
                xEventGroupSetBits( rgbEventHandle, POWER_ON_RGB);
                POWER_PIN_HOLD_UP();
            }
            Status_Update();
        }
        
        if((keyEvent & BIND_SHORT_PRESS) == BIND_SHORT_PRESS)
        {    
            if(RCstatus != RC_CHRG_AND_JOYSTICK)
            {
                xEventGroupSetBits( radioEventHandle, RADIOLINK_BIND);
                RFstatus = RF_BIND;
            }
        }    
        if((keyEvent & SETUP_SHORT_PRESS) == SETUP_SHORT_PRESS)
        {    
            if(RCstatus != RC_CHRG_AND_JOYSTICK)
            {
                xEventGroupSetBits( gimbalEventHandle, GIMBAL_CALIBRATE_IN);
                if(RCstatus == RC_RADIOLINK)
                {
                    RFstatus = RF_CALIBARATION;
                }
            }   
        }       
        
        gimbalEvent = xEventGroupWaitBits( gimbalEventHandle,
                                           GIMBAL_CALIBRATE_END,
                                           pdTRUE,
                                           pdTRUE,
                                           0);
        if((gimbalEvent & GIMBAL_CALIBRATE_END) == GIMBAL_CALIBRATE_END)
        {
            RFstatus = RF_DATA;
        }

        switch (RCstatus)
        {
            case RC_SHUTDOWN:
            {
                break;
            }
            case RC_RADIOLINK:
            {
                switch(RFstatus)
                {
                    case RF_DATA:
                    {
                        xEventGroupSetBits( rgbEventHandle, DATA_RGB);
                        break;
                    }
                    case RF_CALIBARATION:
                    {
                        xEventGroupSetBits( rgbEventHandle, SETUP_RGB);
                        break;
                    }
                    case RF_BIND:
                    {
                        xEventGroupSetBits( rgbEventHandle, BIND_RGB);
                        RFstatus = RF_DATA;
                        break;                            
                    }
                    default:
                    {
                        break;
                    }
                }
                break;
            }
            case RC_CHRG_AND_JOYSTICK:
            {    
                if(HAL_GPIO_ReadPin(KEY_POWER_GPIO_Port,KEY_POWER_Pin) != GPIO_PIN_RESET)
                {
                    xEventGroupSetBits( rgbEventHandle, CHRG_AND_JOYSTICK_RGB);                
                }

                /*内部高频头*/
                if(internalCRSFdata.lastConfigStatus == CONFIG_CRSF_OFF && internalCRSFdata.configStatus == CONFIG_CRSF_ON)
                {
                    vTaskResume(radiolinkTaskHandle);
                    HAL_TIM_Base_Start_IT(&htim1);
                    internalCRSFdata.lastConfigStatus = CONFIG_CRSF_ON;  
                }
                else if(internalCRSFdata.lastConfigStatus == CONFIG_CRSF_ON && internalCRSFdata.configStatus == CONFIG_CRSF_OFF)
                {
                    vTaskSuspend(radiolinkTaskHandle);
                    HAL_TIM_Base_Stop_IT(&htim1);
                    internalCRSFdata.lastConfigStatus = CONFIG_CRSF_OFF;
                }
                
                /*外部高频头*/
                if(externalCRSFdata.lastConfigStatus == CONFIG_CRSF_OFF && externalCRSFdata.configStatus == CONFIG_CRSF_ON)
                {
                    vTaskResume(radiolinkTaskHandle);
                    externalCRSFdata.lastConfigStatus = CONFIG_CRSF_ON;
                    HAL_GPIO_WritePin(EXTERNAL_RF_EN_GPIO_Port, EXTERNAL_RF_EN_Pin, GPIO_PIN_SET);    
                }
                else if(externalCRSFdata.lastConfigStatus == CONFIG_CRSF_ON && externalCRSFdata.configStatus == CONFIG_CRSF_OFF)
                {
                    vTaskSuspend(radiolinkTaskHandle);
                    externalCRSFdata.lastConfigStatus = CONFIG_CRSF_OFF;
                    HAL_GPIO_WritePin(EXTERNAL_RF_EN_GPIO_Port, EXTERNAL_RF_EN_Pin, GPIO_PIN_RESET); 
                }
                break;
            }
            default:
            {
                break;
            }
        
        }
    }    
}
