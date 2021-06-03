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
static uint8_t RCstatus = RC_INIT;
static uint8_t lastRCstatus = RC_INIT;
static uint8_t powerStatus = RC_POWER_OFF;
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

void Status_Update()
{
    if(powerStatus == RC_POWER_ON)
    {
        RCstatus = RC_DATA;
        if(lastRCstatus == RC_INIT)
        {
            vTaskResume(radiolinkTaskHandle);
            vTaskResume(mixesTaskHandle);
        }
        if(lastRCstatus == RC_CHRG_AND_JOYSTICK)
        {
            vTaskSuspend(joystickTaskHandle); 
            vTaskResume(radiolinkTaskHandle);
            vTaskResume(mixesTaskHandle);
        }
    }
    if(powerStatus == RC_POWER_OFF)
    {
        RCstatus = RC_CHRG_AND_JOYSTICK;
        if(lastRCstatus == RC_INIT)
        {
            vTaskResume(joystickTaskHandle);
        }
        if(lastRCstatus == RC_DATA)
        {
            vTaskSuspend(radiolinkTaskHandle);
            vTaskSuspend(mixesTaskHandle);
            vTaskResume(joystickTaskHandle); 
        }
    }
    lastRCstatus = RCstatus;
}

void statusTask(void* param)
{

    EventBits_t keyEvent;
    EventBits_t gimbalEvent;
    Status_Update();
    while(1)
    {
        vTaskDelay(2);
        
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
                xEventGroupSetBits( powerEventHandle, POWER_OFF);
                vTaskResume(powerTaskHandle);
            }
            else
            {
                powerStatus = RC_POWER_ON;
                xEventGroupSetBits( powerEventHandle, POWER_ON);
            }
            Status_Update();
        }
        
		if((keyEvent & BIND_SHORT_PRESS) == BIND_SHORT_PRESS)
		{    
            xEventGroupSetBits( radioEventHandle, RADIOLINK_BIND);
        }    
		if((keyEvent & SETUP_SHORT_PRESS) == SETUP_SHORT_PRESS)
		{    
            xEventGroupSetBits( gimbalEventHandle, GIMBAL_CALIBRATE_IN);
            if(RCstatus == RC_DATA)
            {
                RCstatus = RC_CALIBARATION;
            }
        }       
        
		gimbalEvent = xEventGroupWaitBits( gimbalEventHandle,
		                                   GIMBAL_CALIBRATE_END,
		                                   pdTRUE,
	                                       pdTRUE,
		                                   0);
		if((gimbalEvent & GIMBAL_CALIBRATE_END) == GIMBAL_CALIBRATE_END)
		{
            RGB_Set(BLUE,BRIGHTNESS_MAX);
			RCstatus = RC_DATA;
		}

        if(RCstatus == RC_CALIBARATION)
        {
            osDelay(200);
            RGB_Twinkle(2,200);
        }
        else if(RCstatus == RC_CHRG_AND_JOYSTICK)
        {
            RGB_Breath();
        }
    }    
}
