#ifndef _MIXES_H_
#define _MIXES_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "gimbal.h"
//RUDDER   = 0 ,       //yaw
//THROTTLE = 1 ,       //throttle
//AILERON  = 2 ,       //roll
//ELEVATOR = 3 ,       //pitch
typedef enum
{
  	MIX_RUDDER  	= 0 ,       
	MIX_THROTTLE 	= 1 ,      
	MIX_AILERON 	= 2 ,      
	MIX_ELEVATOR   	= 3 ,      
    MIX_SWA         = 4 ,       //2POS
    MIX_SWB         = 5 ,       //3POS
    MIX_SWC         = 6 ,       //3POS
    MIX_SWD         = 7 ,       //2POS
}gimbalChannel_e;

typedef struct
{
    gimbalChannel_e gimbalChannel;//所设置通道
    uint8_t inverse; // 0为正向，1为反向
    int8_t weight; // 范围-100~100
    int16_t offset; //范围为-100~100
    uint16_t output;
    
}mixData_t;


void mixesTask(void* param);
extern TaskHandle_t mixesTaskHandle;
extern QueueHandle_t mixesValQueue;
uint16_t Mixes_SwitchInverse(uint8_t inverse, uint16_t gimbalValCurr);
uint16_t Mixes_GimbalInverse(uint8_t inverse, uint16_t gimbalValCurr,uint16_t* outputcode);
uint16_t Mixes_GimbalWeight(uint8_t weight, uint16_t gimbalValCurr);
uint16_t Mixes_GimbalOffset(int16_t offset, uint16_t gimbalValCurr);

#endif
