#include "switches.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"
#include "key.h"
QueueHandle_t switchesValQueue = NULL;
uint16_t Get_SwitchValue(SwitchesChannelTypeDef switchIndex)
{
	uint16_t switchValue;
	uint8_t temp;
	temp = switchIndex;
	switch(temp)
	{
		case SWA :
			if(switchIndex == SWA)       //2POS
			{
				if( HAL_GPIO_ReadPin(SWA_GPIO_Port, SWA_Pin) != RELEASED)
				{
					switchValue = SWITCH_MIN_VALUE;
				}
				else
				{
					switchValue = SWITCH_MAX_VALUE;
				}
			}
			break;
		case SWB:
			if(switchIndex == SWB)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_RESET))
				{
					switchValue = SWITCH_MIN_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_MAX_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_MID_VALUE;
				}
			}
			break;
		case SWC:
			if(switchIndex == SWC)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_RESET))
				{
					switchValue = SWITCH_MIN_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_MAX_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_MID_VALUE;
				}
			}
			break;
		case SWD:
			if(switchIndex == SWD)       //2POS
			{
				if(HAL_GPIO_ReadPin(SWD_GPIO_Port, SWD_Pin) != GPIO_PIN_SET)
				{
					switchValue = SWITCH_MIN_VALUE;
				}
				else
				{
					switchValue = SWITCH_MAX_VALUE;
				}
			}
			break;
		default :
			break;
	}
	return switchValue;
}

uint16_t GetSwitchJoyStickValue(SwitchesChannelTypeDef switchIndex)
{
	static uint16_t switchValue;
	uint8_t temp;
	temp = switchIndex;
	switch(temp)
	{
		case SWA :
			if(switchIndex == SWA)       //2POS
			{
				if(HAL_GPIO_ReadPin(SWA_GPIO_Port, SWA_Pin) != GPIO_PIN_SET)
				{
					switchValue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				else
				{
					switchValue = SWITCH_JOYSTICK_MAX_VALUE;
				}
			}
			break;
		case SWB:
			if(switchIndex == SWB)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_RESET))
				{
					switchValue = SWITCH_JOYSTICK_MAX_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_JOYSTICK_MID_VALUE;
				}
			}
			break;
		case SWC:
			if(switchIndex == SWC)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_RESET))
				{
					switchValue = SWITCH_JOYSTICK_MAX_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchValue = SWITCH_JOYSTICK_MID_VALUE;
				}
			}
			break;
		case SWD:
			if(switchIndex == SWD)       //2POS
			{
				if(HAL_GPIO_ReadPin(SWD_GPIO_Port, SWD_Pin) != GPIO_PIN_SET)
				{
					switchValue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				else
				{
					switchValue = SWITCH_JOYSTICK_MAX_VALUE;
				}
			}
			break;
		default :
			break;
	}
	return switchValue;
}

uint16_t JoyStickValMapToChannelVal(uint16_t switch_val)
{
	uint16_t tempVal;
	if(switch_val == SWITCH_JOYSTICK_MAX_VALUE)
	{
		tempVal = SWITCH_MAX_VALUE;
	}
	
	if(switch_val == SWITCH_JOYSTICK_MID_VALUE)
	{
		tempVal = SWITCH_MID_VALUE;
	}
	
	if(switch_val == SWITCH_JOYSTICK_MIN_VALUE)
	{
		tempVal = SWITCH_MIN_VALUE;
	}
	return tempVal;
}

void switchesTask(void* param)
{
    static uint16_t switchesBuff[4] = {0};
	switchesValQueue = xQueueCreate(20,sizeof(switchesBuff));
	while(1)
	{
		vTaskDelay(15);
        
		switchesBuff[SWA] =  Get_SwitchValue(SWA);
		switchesBuff[SWB] =  Get_SwitchValue(SWB);
		switchesBuff[SWC] =  Get_SwitchValue(SWC);
		switchesBuff[SWD] =  Get_SwitchValue(SWD);
        
		xQueueSend(switchesValQueue,switchesBuff,0);
	}
}
