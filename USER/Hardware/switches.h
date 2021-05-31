#ifndef _SWITCHES_H_
#define _SWITCHES_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern QueueHandle_t switchesValQueue;
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

#define SWA_GPIO_Port 		GPIOB

#define SWB_H_GPIO_Port 	GPIOB

#define SWB_L_GPIO_Port 	GPIOB

#define SWD_GPIO_Port 		GPIOB

#define SWC_H_GPIO_Port 	GPIOB

#define SWC_L_GPIO_Port 	GPIOB

void switchesTask(void* param);
uint16_t Get_SwitchValue(SwitchesChannelTypeDef switchIndex);
uint16_t GetSwitchJoyStickValue(SwitchesChannelTypeDef switchIndex);
uint16_t JoyStickValMapToChannelVal(uint16_t switch_val);
#endif


