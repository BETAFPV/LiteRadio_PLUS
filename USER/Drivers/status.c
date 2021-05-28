#include "status.h"
#include "gimbal.h"
#include "key.h"
#include "rgb.h"
#include "power_switch.h"
#include "radiolink.h"
#include "stmflash.h"
#include "joystick.h"
#include "mixes.h"
uint16_t proIndex;
uint8_t RCstatus = radio_datastatus;
uint8_t lastRCstatus = initStatus;
void Status_Init()
{
    STMFLASH_Read(FLASH_ADDR,&proIndex,1);
    if(proIndex > 4)
    {
        proIndex = 0;
        STMFLASH_Write(FLASH_ADDR,&proIndex,1);
    }
    
   if(HAL_GPIO_ReadPin(KEY_BIND_GPIO_Port,KEY_BIND_Pin) == GPIO_PIN_RESET)
   {
       proIndex++;
       if(proIndex > 4)
		{
			proIndex = 0;
		}

        STMFLASH_Write(FLASH_ADDR,&proIndex,1);
   }
   Led_Twinkle_Init((proIndex+1));
   Version_Init(proIndex); 
}

void statusTask(void* param)
{
    uint8_t powerStatus = 0;
    EventBits_t R_event;

    while(1)
    {
        vTaskDelay(2);
        
        if(RCstatus == radio_datastatus)
        {
            if(lastRCstatus == initStatus)
            {
                vTaskResume(radiolinkTaskHandle);
                vTaskResume(mixesTaskHandle);
            }else if(lastRCstatus == joystickstatus)
            {
                vTaskSuspend(joystickTaskHandle);              
                vTaskResume(radiolinkTaskHandle);
                vTaskResume(mixesTaskHandle);
            }
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
        }
        lastRCstatus = RCstatus;
        
		R_event= xEventGroupWaitBits( KeyEventHandle,
		                              POWERSWITCH_LONG_PRESS|BIND_SHORT_PRESS|SETUP_SHORT_PRESS,
		                              pdTRUE,
	                                  pdFALSE,
		                              0);  
		if((R_event & POWERSWITCH_LONG_PRESS) == POWERSWITCH_LONG_PRESS)
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
		if((R_event & BIND_SHORT_PRESS) == BIND_SHORT_PRESS)
		{    
            xEventGroupSetBits( radioEventHandle, RADIOLINK_BIND);
        }    
		if((R_event & SETUP_SHORT_PRESS) == SETUP_SHORT_PRESS)
		{    
            xEventGroupSetBits( gimbalEventHandle, GIMBAL_CALIBRATE);
        }         
    }
        
}
