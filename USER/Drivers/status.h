#ifndef __STATUS_H_
#define __STATUS_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

typedef enum
{
    RC_INIT              = 0,
  	RC_CALIBARATION      = 1,       
	RC_DATA 	         = 2,      
    RC_CHRG_AND_JOYSTICK = 3,
}RCstatus_e;

void Status_Init(void);
void Status_Update(void);
void statusTask(void* param);

#endif  
