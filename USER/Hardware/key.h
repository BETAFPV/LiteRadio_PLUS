#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "gpio.h"

extern EventGroupHandle_t KeyEventHandle;

//按键状态
#define BIND_SHORT_PRESS	    (0x01 << 0)	
#define BIND_LONG_PRESS	        (0x01 << 1)	
#define SETUP_SHORT_PRESS	    (0x01 << 2)	
#define SETUP_LONG_PRESS	    (0x01 << 3)	
#define POWERSWITCH_SHORT_PRESS	(0x01 << 4)	
#define POWERSWITCH_LONG_PRESS	(0x01 << 5)	


#define BIND_KEY_PORT           GPIOA
#define BIND_KEY_PIN            GPIO_PIN_8


#define POWER_KEY_PORT          GPIOC
#define POWER_KEY_PIN           GPIO_PIN_14

#define BIND_KEY_STATUS()       HAL_GPIO_ReadPin(BIND_KEY_PORT,BIND_KEY_PIN)
#define SETUP_KEY_STATUS()      HAL_GPIO_ReadPin(KEY_SETUP_GPIO_Port,KEY_SETUP_Pin)
#define POWER_KEY_STATUS()		HAL_GPIO_ReadPin(POWER_KEY_PORT,POWER_KEY_PIN)

void keyTask(void* param);
uint8_t getKeyState(void);
#endif
 
