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
static uint16_t protocolIndex;
static uint8_t RCstatus = RC_SHUTDOWN;
static uint8_t lastRCstatus = RC_SHUTDOWN;
static uint8_t RFstatus = RF_DATA;
static uint8_t powerStatus = RC_POWER_OFF;
static uint8_t bindCount = 0;
void Status_Init()
{
    STMFLASH_Read(FLASH_ADDR,&protocolIndex,1);
    if(protocolIndex > 4)
    {
        protocolIndex = 0;
        STMFLASH_Write(FLASH_ADDR,&protocolIndex,1);
    }
    
    if(HAL_GPIO_ReadPin(KEY_BIND_GPIO_Port,KEY_BIND_Pin) == GPIO_PIN_RESET)
    {
        protocolIndex++;
        if(protocolIndex > 4)
        {
            protocolIndex = 0;
        }

        STMFLASH_Write(FLASH_ADDR,&protocolIndex,1);
    }
    if(HAL_GPIO_ReadPin(KEY_POWER_GPIO_Port,KEY_POWER_Pin) == GPIO_PIN_RESET)
    {
        RGB_TwinkleForInit((protocolIndex+1),400);
    }
    Version_Init(protocolIndex); 
}

void Status_Update()
{
    if(powerStatus == RC_POWER_ON)
    {
        RCstatus = RC_RADIOLINK;
        if(lastRCstatus == RC_SHUTDOWN)
        {
            vTaskResume(radiolinkTaskHandle);
          //  vTaskResume(mixesTaskHandle);
        }
        if(lastRCstatus == RC_CHRG_AND_JOYSTICK)
        {
            vTaskSuspend(joystickTaskHandle); 
            vTaskResume(radiolinkTaskHandle);
          //  vTaskResume(mixesTaskHandle);
        }
    }
    if(powerStatus == RC_POWER_OFF)
    {
        RCstatus = RC_CHRG_AND_JOYSTICK;
        if(lastRCstatus == RC_SHUTDOWN)
        {
            vTaskResume(joystickTaskHandle);
        }
        if(lastRCstatus == RC_RADIOLINK)
        {
            vTaskSuspend(radiolinkTaskHandle);
        //    vTaskSuspend(mixesTaskHandle);
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
        vTaskDelay(2);
        if(lastRCstatus == RC_INIT)
        {
            osDelay(3000);
            lastRCstatus = RC_SHUTDOWN;
        }
        else if(lastRCstatus == RC_SHUTDOWN)
        {
            Status_Update();
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
                if(HAL_GPIO_ReadPin(KEY_POWER_GPIO_Port,KEY_POWER_Pin) == GPIO_PIN_RESET)
                {

                }
                else
                {
                    xEventGroupSetBits( rgbEventHandle, CHRG_AND_JOYSTICK_RGB);                
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
