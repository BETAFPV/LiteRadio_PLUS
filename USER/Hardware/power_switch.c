#include <stdbool.h>
#include "power_switch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "led.h"

static bool powerswitchStatus = false;
static uint8_t PowerSwitchKeybackup = 0;
static uint8_t PowerSwitchUpKeyState = 0;
void powerSwitchTask(void* param)
{
	while(1)
	{
		vTaskDelay(50);
		uint8_t key_status = getKeyState();
		if(key_status == POWERSWITCH_LONG_PRESS)
		{
			PowerSwitchUpKeyState = POWERSWITCH_LONG_PRESS;
		}
		else
		{
			PowerSwitchUpKeyState = 0x00;
		}
		if(PowerSwitchUpKeyState != PowerSwitchKeybackup)
		{
			key_status = getKeyState();			
			if(PowerSwitchUpKeyState != key_status)
			{
				powerswitchStatus = !powerswitchStatus;
			}
			PowerSwitchKeybackup = PowerSwitchUpKeyState;
		}
		if(powerswitchStatus == true)
		{
			POWER_ON();
			Led_Blue(ON);
		}
		else
		{
			POWER_OFF();
			Led_Blue(OFF);
		}
		
	}
}


