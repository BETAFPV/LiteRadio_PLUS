#ifndef __STATUS_H_
#define __STATUS_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

typedef enum
{
    initStatus = 0,
  	radio_bindstatus  	= 1 ,       
	radio_datastatus 	= 2 ,      
	joystickstatus   	= 3 ,      
}RCstatus_e;

void status_init(void);
void statusTask(void* param);

#endif  
