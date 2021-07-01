#include "power_switch.h"
#include "key.h"
#include "rgb.h"
#include "buzzer.h"
#include "radiolink.h"

EventGroupHandle_t powerEventHandle = NULL;
TaskHandle_t powerTaskHandle;

void powerswitchTask(void* param)
{
	EventBits_t powerEvent;
	while(1)
	{
		vTaskDelay(100);
		powerEvent= xEventGroupWaitBits( powerEventHandle,
                                         POWER_ON|POWER_OFF,
		                                 pdTRUE,
	                                     pdFALSE,
		                                 0);
		if((powerEvent & POWER_ON) == POWER_ON)
		{

            RGB_Breath_Up(BLUE);
            taskENTER_CRITICAL();	/*进入临界*/
            
            vTaskSuspend(powerTaskHandle);

            taskEXIT_CRITICAL();   /*退出临界*/
		}
		if((powerEvent & POWER_OFF) == POWER_OFF)
		{
            RGB_Set(BLACK,BRIGHTNESS_MAX);
            xEventGroupSetBits( buzzerEventHandle, POWER_OFF_RING);
           // RGB_Breath_Down(RED);
            POWER_PIN_HOLD_DOWN();
		}        
	}
}



