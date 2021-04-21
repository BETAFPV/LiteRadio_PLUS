#ifndef _SWITCHES_H_
#define _SWITCHES_H_
#include "stm32f1xx_hal.h"

typedef enum
{
  	SWA 	= 0 ,       //2POS
	SWB 	= 1 ,       //3POS
	SWC  	= 2 ,       //3POS
	SWD   	= 3 ,       //2POS
}SwitchesChannelTypeDef;


#define SWITCH_MAX_VALUE 2000
#define SWITCH_MID_VALUE 1500
#define SWITCH_MIN_VALUE 988

#define SWITCH_JOYSTICK_MAX_VALUE 4094
#define SWITCH_JOYSTICK_MID_VALUE 2047
#define SWITCH_JOYSTICK_MIN_VALUE 10

#define SWA_Pin 			GPIO_PIN_6
#define SWA_GPIO_Port 		GPIOB

#define SWB_H_Pin 			GPIO_PIN_4
#define SWB_H_GPIO_Port 	GPIOB

#define SWB_L_Pin 			GPIO_PIN_5
#define SWB_L_GPIO_Port 	GPIOB

#define SWD_Pin 			GPIO_PIN_8
#define SWD_GPIO_Port 		GPIOB

#define SWC_H_Pin 			GPIO_PIN_7
#define SWC_H_GPIO_Port 	GPIOB

#define SWC_L_Pin 			GPIO_PIN_9
#define SWC_L_GPIO_Port 	GPIOB

void switchesTask(void* param);
uint16_t GetSwitchValue(SwitchesChannelTypeDef switchnum);
uint16_t GetSwitchJoyStickValue(SwitchesChannelTypeDef switchnum);
uint16_t JoyStickValMapToChannelVal(uint16_t switch_val);
#endif


