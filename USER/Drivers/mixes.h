#ifndef _MIXES_H_
#define _MIXES_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "gimbal.h"
//RUDDER   = 0 ,       //yaw
//THROTTLE = 1 ,       //throttle
//AILERON  = 2 ,       //roll
//ELEVATOR = 3 ,       //pitch

#define CHANNEILS_INPUT_ID 0x01
#define CHANNEILS_REVERSE_ID 0x02
#define CHANNEILS_WEIGHT_ID 0x03
#define CHANNEILS_OFFSET_ID 0x04
#define CONFIGER_INFO_ID 0x05

#define MIX_INPUT_UPDATE	        (0x01 << 0)	
#define MIX_REVERSE_UPDATE	        (0x01 << 1)	
#define MIX_WEIGHT_UPDATE	        (0x01 << 2)	
#define MIX_OFFSET_UPDATE	        (0x01 << 3)
//#define SETUP_END_RING                  (0x01 << 4)
//#define POWER_ON_RING	                (0x01 << 5)	
//#define POWER_OFF_RING	                (0x01 << 6)	
//#define RISS_WARNING_RING	            (0x01 << 7)	

//typedef enum
//{
//  	MIX_RUDDER  	= 0 ,       
//	MIX_THROTTLE 	= 1 ,      
//	MIX_AILERON 	= 2 ,      
//	MIX_ELEVATOR   	= 3 ,      
//    MIX_SWA         = 4 ,       //2POS
//    MIX_SWB         = 5 ,       //3POS
//    MIX_SWC         = 6 ,       //3POS
//    MIX_SWD         = 7 ,       //2POS
//}gimbalChannel_e;

#define MIX_CHANNEL_INPUT_ADDR 0x08007000
#define MIX_CHANNEL_INVERSE_ADDR 0x08007016
#define MIX_CHANNEL_WEIGHT_ADDR 0x08007032
#define MIX_CHANNEL_OFFSET_ADDR 0x08007048



#define MIX_RUDDER     0x00      
#define MIX_THROTTLE   0x01      
#define MIX_AILERON    0x02    
#define MIX_ELEVATOR   0x03    
#define MIX_SWA        0x04       //2POS
#define MIX_SWB        0x05       //3POS
#define MIX_SWC        0x06       //3POS
#define MIX_SWD        0x07       //2POS

typedef struct
{
    uint8_t gimbalChannel;//所设置通道
    uint8_t inverse; // 0为正向，1为反向
    uint8_t weight; // 范围0~100
    uint8_t offset; //范围为0~200
    uint16_t output;
    
}mixData_t;


void mixesTask(void* param);
extern TaskHandle_t mixesTaskHandle;
extern QueueHandle_t mixesValQueue;
void Mixes_Init(void);

void Mixes_ChannelUpdate(void);
void Mixes_InverseUpdate(void);
void Mixes_WeightUpdate(void);
void Mixes_OffsetUpdate(void);
uint16_t Mixes_SwitchInverse(uint8_t inverse, uint16_t gimbalValCurr);
uint16_t Mixes_GimbalInverse(uint8_t inverse, uint16_t gimbalValCurr,uint16_t* outputcode);
uint16_t Mixes_GimbalWeight(uint8_t weight, uint16_t gimbalValCurr);
uint16_t Mixes_GimbalOffset(uint8_t offset, uint16_t gimbalValCurr);
//void Get_MixesSet(void);

#endif
