#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "gpio.h"

extern EventGroupHandle_t KeyEventHandle;

#define LONG_PRESS_COUNT 	500	/*判断为长按时间（ms）*/

#define PRESSED		GPIO_PIN_RESET
#define RELEASED	GPIO_PIN_SET

//按键状态
#define BIND_SHORT_PRESS	    (0x01 << 0)	
#define BIND_LONG_PRESS	        (0x01 << 1)	
#define SETUP_SHORT_PRESS	    (0x01 << 2)	
#define SETUP_LONG_PRESS	    (0x01 << 3)	
#define POWERSWITCH_SHORT_PRESS	(0x01 << 4)	
#define POWERSWITCH_LONG_PRESS	(0x01 << 5)	

#define BIND_KEY_STATUS()       HAL_GPIO_ReadPin(KEY_BIND_GPIO_Port,KEY_BIND_Pin)
#define SETUP_KEY_STATUS()      HAL_GPIO_ReadPin(KEY_SETUP_GPIO_Port,KEY_SETUP_Pin)
#define POWER_KEY_STATUS()		HAL_GPIO_ReadPin(KEY_POWER_GPIO_Port,KEY_POWER_Pin)

void keyTask(void* param);

#endif
 
