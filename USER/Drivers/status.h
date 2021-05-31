#ifndef __STATUS_H_
#define __STATUS_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

typedef enum
{
    initStatus = 0,
  	RADIO_CALIBARATION  	= 1 ,       
	RADIO_DATA 	= 2 ,      
	joystickstatus   	= 3 ,      
}RCstatus_e;

void Status_Init(void);
void statusTask(void* param);

#endif  
