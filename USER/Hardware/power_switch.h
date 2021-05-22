#ifndef _POWER_SWITCH_H_
#define _POWER_SWITCH_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define POWER_ON	        (0x01 << 0)	
#define POWER_OFF	        (0x01 << 1)	


#define POWER_ON_PORT           GPIOC
#define POWER_ON_PIN            GPIO_PIN_15

#define POWER_PIN_HOLD_UP()   HAL_GPIO_WritePin(POWER_ON_PORT,POWER_ON_PIN,GPIO_PIN_SET)
#define POWER_PIN_HOLD_DOWN() HAL_GPIO_WritePin(POWER_ON_PORT,POWER_ON_PIN,GPIO_PIN_RESET)

extern EventGroupHandle_t powerEventHandle;
extern TaskHandle_t powerTaskHandle;

void powerswitchTask(void* param);
void powerOn(void);
#endif

