#include <stdbool.h>
#include "power_switch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "led.h"
#include "cmsis_os.h"
#include "buzzer.h"
static bool powerswitchStatus = false;

uint8_t key_status;
int i = 0;
void powerSwitchTask(void* param)
{
	EventBits_t R_event;
	while(1)
	{
		vTaskDelay(20);
		R_event= xEventGroupWaitBits( KeyEventHandle,
		                              POWERSWITCH_LONG_PRESS,
		                              pdTRUE,
	                                  pdTRUE,
		                              portMAX_DELAY);
		if((R_event & POWERSWITCH_LONG_PRESS) == POWERSWITCH_LONG_PRESS)
		{
			powerswitchStatus = !powerswitchStatus;
			if(powerswitchStatus == true)
			{
				Led_Blue(ON);
                xEventGroupSetBits( buzzerEventHandle, POWER_ON_RING);
                osDelay(1200);
				POWER_PIN_HOLD_UP();
			}
			else
			{
                xEventGroupSetBits( buzzerEventHandle, POWER_OFF_RING);
                osDelay(1200);
				POWER_PIN_HOLD_DOWN();
			}
		}
	}
}



