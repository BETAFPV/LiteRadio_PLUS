#include "status.h"
#include "gimbal.h"
#include "key.h"
#include "rgb.h"
#include "power_switch.h"
#include "radiolink.h"
#include "stmflash.h"
#include "joystick.h"
#include "mixes.h"
static uint16_t protocolIndex;
static uint8_t RCstatus = RADIO_DATA;
static uint8_t lastRCstatus = initStatus;
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
   RGB_TwinkleForInit((protocolIndex+1),400);
   Version_Init(protocolIndex); 
}

void statusTask(void* param)
{
    uint8_t powerStatus = 0;
    EventBits_t keyEvent;
    EventBits_t gimbalEvent;
    while(1)
    {
        vTaskDelay(2);
        
		gimbalEvent = xEventGroupWaitBits( gimbalEventHandle,
		                                   GIMBAL_CALIBRATE_END,
		                                   pdTRUE,
	                                       pdTRUE,
		                                   0);
		if((gimbalEvent & GIMBAL_CALIBRATE_END) == GIMBAL_CALIBRATE_END)
		{
            RGB_Set(BLUE,255);
			RCstatus = lastRCstatus;
		}
        if(RCstatus == RADIO_DATA)
        {
            if(lastRCstatus == initStatus)
            {
                vTaskResume(radiolinkTaskHandle);
                vTaskResume(mixesTaskHandle);
            }
            else if(lastRCstatus == joystickstatus)
            {
                vTaskSuspend(joystickTaskHandle);              
                vTaskResume(radiolinkTaskHandle);
                vTaskResume(mixesTaskHandle);
            }
            lastRCstatus = RCstatus;
        } 
        if(RCstatus == joystickstatus)
        {
            if(lastRCstatus == initStatus)
            {
                vTaskResume(joystickTaskHandle);
            }
            else if(lastRCstatus == joystickstatus)
            {
                vTaskSuspend(radiolinkTaskHandle);
                vTaskSuspend(mixesTaskHandle);
                vTaskResume(joystickTaskHandle);              
            }
            lastRCstatus = RCstatus;
        }
        
        if(RCstatus == RADIO_CALIBARATION)
        {
            osDelay(200);
            RGB_Twinkle(2,200);
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
                powerStatus = 0;
                xEventGroupSetBits( powerEventHandle, POWER_OFF);
                vTaskResume(powerTaskHandle);
            }
            else
            {
                powerStatus = 1;
                xEventGroupSetBits( powerEventHandle, POWER_ON);
                
            }
        }
		if((keyEvent & BIND_SHORT_PRESS) == BIND_SHORT_PRESS)
		{    
            xEventGroupSetBits( radioEventHandle, RADIOLINK_BIND);
        }    
		if((keyEvent & SETUP_SHORT_PRESS) == SETUP_SHORT_PRESS)
		{    
            xEventGroupSetBits( gimbalEventHandle, GIMBAL_CALIBRATE_IN);
            RCstatus = RADIO_CALIBARATION;
        }         
    }
        
}
