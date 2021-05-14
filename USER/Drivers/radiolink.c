#include "radiolink.h"
#include "gimbal.h"
#include "switches.h"
#include "frsky_d16.h"
#include "cc2500.h"
#include "led.h"
#include "mixes.h"
#include "key.h"

//uint16_t gimbal_val_buff[4];
//uint16_t switches_val_buff[4];
uint16_t control_data[8];

void radiolinkTask(void* param)
{
    initFRSKYD16();
    EventBits_t R_event = pdPASS;
    while(1)
    {
        osDelay(9);
        xQueueReceive(mixesdataVal_Queue,control_data,0);
        xEventGroupGetBits(KeyEventHandle);

        R_event= xEventGroupWaitBits( KeyEventHandle,
		                              BIND_SHORT_PRESS,
		                              pdTRUE,
	                                  pdTRUE,
		                              0);
		if((R_event & BIND_SHORT_PRESS) == BIND_SHORT_PRESS)
		{
            SetBind();
		}
        ReadFRSKYD16(control_data);
    }
}

