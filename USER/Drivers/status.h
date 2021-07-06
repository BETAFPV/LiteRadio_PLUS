#ifndef __STATUS_H_
#define __STATUS_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

typedef enum
{
    RC_INIT              = 0,
    RC_SHUTDOWN             ,
    RC_RADIOLINK 	        ,    
    RC_CHRG_AND_JOYSTICK    ,
}RCstatus_e;

typedef enum
{
    RF_DATA           = 0,
    RF_CALIBARATION      ,   
    RF_BIND              ,
}RFstatus_e;


uint32_t Get_ProtocolDelayTime(void);
void Status_Init(void);
void Status_Update(void);
void statusTask(void* param);

#endif  
