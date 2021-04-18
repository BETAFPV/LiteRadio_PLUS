#ifndef _POWER_SWITCH_H_
#define _POWER_SWITCH_H_
#include "stm32f1xx_hal.h"

#define POWER_ON_PORT           GPIOC
#define POWER_SWITCH_PORT       GPIOC

#define POWER_ON_PIN            GPIO_PIN_15
#define POWER_SWITCH_PIN        GPIO_PIN_14

#define POWER_ON  HAL_GPIO_WritePin(POWER_SWITCH_PORT,POWER_SWITCH_PIN,GPIO_PIN_SET)       //遥控开机
#define POWER_OFF HAL_GPIO_WritePin(POWER_SWITCH_PORT,POWER_SWITCH_PIN,GPIO_PIN_RESET)     //遥控关机

#define POWER_SWITCH_STATUS     HAL_GPIO_ReadPin(POWER_SWITCH_PORT,POWER_SWITCH_PIN)       //读取开关引脚状态

enum{
    SWITCH_PRESS,
    SWITCH_UP
};

#endif

