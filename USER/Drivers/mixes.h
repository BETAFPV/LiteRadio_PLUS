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
  	mix_RUDDER  	= 0 ,       
	mix_THROTTLE 	= 1 ,      
	mix_AILERON 	= 2 ,      
	mix_ELEVATOR   	= 3 ,      
    mix_SWA         = 4 ,       //2POS
    mix_SWB         = 5 ,       //3POS
    mix_SWC         = 6 ,       //3POS
    mix_SWD         = 7 ,       //2POS
}mixsetChannel_e;

typedef struct
{
    mixsetChannel_e GimbalChannel;//所设置通道
    uint8_t mix_inverse; // 0为正向，1为反向
    int8_t mix_weight; // 范围-100~100
    int16_t mix_offset; //范围为-100~100
    uint16_t mix_output_data;
    
}mixdata_t;


void mixesTask(void* param);
extern TaskHandle_t mixesTaskHandle;
extern QueueHandle_t mixesdataVal_Queue;
uint16_t mixes_sw_inverse(uint8_t inverse, uint16_t gimbal_val_curr);

uint16_t mixes_gimbal_inverse(uint8_t inverse, uint16_t gimbal_val_curr,uint16_t* outputcode);
uint16_t mixes_gimbal_weight(uint8_t weight, uint16_t gimbal_val_curr);
uint16_t mixes_gimbal_offset(int16_t offset, uint16_t gimbal_val_curr);



#endif
