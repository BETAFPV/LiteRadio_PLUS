#ifndef _SWITCHES_H_
#define _SWITCHES_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern QueueHandle_t switchesValQueue;
typedef enum
{
  	SWA 	= 0 ,       /*2POS*/
	SWB 	= 1 ,       /*3POS*/
	SWC  	= 2 ,       /*3POS*/
	SWD   	= 3 ,       /*2POS*/
}SwitchesChannelTypeDef;


#define SWITCH_MAX_VALUE 2012
#define SWITCH_MID_VALUE 1500
#define SWITCH_MIN_VALUE 988

#define SWA_GPIO_Port 		GPIOB
#define SWB_H_GPIO_Port 	GPIOB
#define SWB_L_GPIO_Port 	GPIOB
#define SWC_H_GPIO_Port 	GPIOB
#define SWC_L_GPIO_Port 	GPIOB
#define SWD_GPIO_Port 		GPIOB

void switchesTask(void* param);
uint16_t Get_SwitchValue(SwitchesChannelTypeDef switchIndex);
#endif


