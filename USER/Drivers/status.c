#include "status.h"
#include "key.h"
#include "power_switch.h"
#include "radiolink.h"

void statusTask(void* param)
{
    uint8_t powerStatus = 0;
    EventBits_t R_event;
    uint8_t RCstatus = radio_datastatus;
    uint8_t lastRCstatus = 3;
    while(1)
    {
        vTaskDelay(2);
        if(RCstatus == radio_datastatus)
        {
           
        }else if(RCstatus == joystickstatus)
        {
        
        }
		R_event= xEventGroupWaitBits( KeyEventHandle,
		                              POWERSWITCH_LONG_PRESS|BIND_SHORT_PRESS,
		                              pdTRUE,
	                                  pdFALSE,
		                              portMAX_DELAY);  
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
    }
        
}