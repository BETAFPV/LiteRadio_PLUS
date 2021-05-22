#ifndef __STATUS_H_
#define __STATUS_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

typedef enum
{
  	radio_bindstatus  	= 0 ,       
	radio_datastatus 	= 1 ,      
	joystickstatus   	= 2 ,      
}RCstatus_e;
void statusTask(void* param);

#endif  