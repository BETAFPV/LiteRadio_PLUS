#include "status.h"
#include "gimbal.h"
#include "key.h"
#include "rgb.h"
#include "power_switch.h"
#include "radiolink.h"
#include "stmflash.h"
#include "joystick.h"

uint16_t protocol_Index;

void status_init()
{
    STMFLASH_Read(FLASH_ADDR,&protocol_Index,1);
    if(protocol_Index >= 4)
    {
        protocol_Index = 0;
        STMFLASH_Write(FLASH_ADDR,&protocol_Index,1);
    }
    
   if(HAL_GPIO_ReadPin(KEY_BIND_GPIO_Port,KEY_BIND_Pin) == GPIO_PIN_RESET)
   {
       protocol_Index = protocol_Index + 1;
       if(protocol_Index >= 4)
		{
			protocol_Index = 0;
		}
        STMFLASH_Write(FLASH_ADDR,&protocol_Index,1);
   }
   Led_Twinkle_Init((protocol_Index+1));
   version_init(protocol_Index); 
}

void statusTask(void* param)
{
    uint8_t powerStatus = 0;
    EventBits_t R_event;
    uint8_t RCstatus = radio_datastatus;
    uint8_t lastRCstatus = initStatus;
    while(1)
    {
        vTaskDelay(2);
        if(RCstatus == radio_datastatus)
        {
            if(lastRCstatus == initStatus)
            {
                vTaskResume(radiolinkTaskHandle);
            }else if(lastRCstatus == joystickstatus)
            {
                vTaskResume(radiolinkTaskHandle);
            }
        }else if(RCstatus == joystickstatus)
        {
           if(lastRCstatus == initStatus)
           {
                vTaskResume(joystickTaskHandle);
           }
        }
        
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
