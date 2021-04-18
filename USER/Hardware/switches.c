#include "switches.h"

uint16_t GetSwitchValue(SwitchesChannelTypeDef switchnum)
{
	uint16_t switchvalue;
	uint8_t temp;
	temp = switchnum;
	switch(temp)
	{
		case SWA :
			if(switchnum == SWA)       //2POS
			{
				if(HAL_GPIO_ReadPin(SWA_GPIO_Port, SWA_Pin) != GPIO_PIN_SET)
				{
					switchvalue = SWITCH_MIN_VALUE;
				}
				else
				{
					switchvalue = SWITCH_MAX_VALUE;
				}
			}
			break;
		case SWB:
			if(switchnum == SWB)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_RESET))
				{
					switchvalue = SWITCH_MIN_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_MAX_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_MID_VALUE;
				}
			}
			break;
		case SWC:
			if(switchnum == SWC)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_RESET))
				{
					switchvalue = SWITCH_MIN_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_MAX_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_MID_VALUE;
				}
			}
			break;
		case SWD:
			if(switchnum == SWD)       //2POS
			{
				if(HAL_GPIO_ReadPin(SWD_GPIO_Port, SWD_Pin) != GPIO_PIN_SET)
				{
					switchvalue = SWITCH_MIN_VALUE;
				}
				else
				{
					switchvalue = SWITCH_MAX_VALUE;
				}
			}
			break;
		default :
			break;
	}
	return switchvalue;
}

uint16_t GetSwitchJoyStickValue(SwitchesChannelTypeDef switchnum)
{
	static uint16_t switchvalue;
	uint8_t temp;
	temp = switchnum;
	switch(temp)
	{
		case SWA :
			if(switchnum == SWA)       //2POS
			{
				if(HAL_GPIO_ReadPin(SWA_GPIO_Port, SWA_Pin) != GPIO_PIN_SET)
				{
					switchvalue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				else
				{
					switchvalue = SWITCH_JOYSTICK_MAX_VALUE;
				}
			}
			break;
		case SWB:
			if(switchnum == SWB)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_RESET))
				{
					switchvalue = SWITCH_JOYSTICK_MAX_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				
				if((HAL_GPIO_ReadPin(SWB_H_GPIO_Port, SWB_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWB_L_GPIO_Port, SWB_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_JOYSTICK_MID_VALUE;
				}
			}
			break;
		case SWC:
			if(switchnum == SWC)       //3POS
			{
				if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_RESET))
				{
					switchvalue = SWITCH_JOYSTICK_MAX_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				
				else if((HAL_GPIO_ReadPin(SWC_H_GPIO_Port, SWC_H_Pin) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(SWC_L_GPIO_Port, SWC_L_Pin) == GPIO_PIN_SET))
				{
					switchvalue = SWITCH_JOYSTICK_MID_VALUE;
				}
			}
			break;
		case SWD:
			if(switchnum == SWD)       //2POS
			{
				if(HAL_GPIO_ReadPin(SWD_GPIO_Port, SWD_Pin) != GPIO_PIN_SET)
				{
					switchvalue = SWITCH_JOYSTICK_MIN_VALUE;
				}
				else
				{
					switchvalue = SWITCH_JOYSTICK_MAX_VALUE;
				}
			}
			break;
		default :
			break;
	}
	return switchvalue;
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

