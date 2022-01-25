#ifndef _MIXES_H_
#define _MIXES_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "gimbal.h"
//RUDDER   = 0 ,       //yaw
//THROTTLE = 1 ,       //throttle
//AILERON  = 2 ,       //roll
//ELEVATOR = 3 ,       //pitch

#define MIX_CHANNEL_OUTPUT_MAX 2012
#define MIX_CHANNEL_OUTPUT_MID 1500
#define MIX_CHANNEL_OUTPUT_MIN 988

#define CHANNEILS_INFO_ID 0x01

#define LITE_CONFIGER_INFO_ID     0x05
#define INTERNAL_CONFIGER_INFO_ID 0x06
#define EXTERNAL_CONFIGER_INFO_ID 0X07
#define EXTRA_CUSTOM_CONFIG_ID 0x08
#define DEVICE_INFO_ID 0x5a
#define UID_BYTES_ID 0x22

#define CHANNEILS_INPUT_ID 0x01
#define CHANNEILS_REVERSE_ID 0x02
#define CHANNEILS_WEIGHT_ID 0x03
#define CHANNEILS_OFFSET_ID 0x04

#define MIX_INPUT_UPDATE	        (0x01 << 0)	
#define MIX_REVERSE_UPDATE	        (0x01 << 1)	
#define MIX_WEIGHT_UPDATE	        (0x01 << 2)	
#define MIX_OFFSET_UPDATE	        (0x01 << 3)

#define MIX_CHANNEL_INFO_ADDR       0x8007000
#define MIX_CHANNEL_1_INFO_ADDR     0x8007000
#define MIX_CHANNEL_2_INFO_ADDR     0x8007008
#define MIX_CHANNEL_3_INFO_ADDR     0x8007010
#define MIX_CHANNEL_4_INFO_ADDR     0x8007018
#define MIX_CHANNEL_5_INFO_ADDR     0x8007020
#define MIX_CHANNEL_6_INFO_ADDR     0x8007028
#define MIX_CHANNEL_7_INFO_ADDR     0x8007030
#define MIX_CHANNEL_8_INFO_ADDR     0x8007038

#define CACHE_MIX_CHANNEL_INFO_ADDR 0x8007100

#define MasterUidUseChipIDFlag_ADDR  0x8007200
#define MasterID1FromBindPhrase_ADDR 0x8007202
#define MasterID2FromBindPhrase_ADDR 0x8007204
#define MasterID3FromBindPhrase_ADDR 0x8007206
#define MasterID4FromBindPhrase_ADDR 0x8007208
#define MasterID5FromBindPhrase_ADDR 0x8007210
#define MasterID6FromBindPhrase_ADDR 0x8007212

#define JoystickDeadZonePercent_ADDR 0x8007214
#define BuzzerSwitch_ADDR 0x8007216

#define DeafultDeadZonePercent 4
#define MinDeadZonePercent 1
#define MaxDeadZonePercent 10

#define MIX_AILERON    0x00      
#define MIX_ELEVATOR   0x01  
#define MIX_THROTTLE   0x02   
#define MIX_RUDDER     0x03    
#define MIX_SWA        0x04       //2POS
#define MIX_SWB        0x05       //3POS
#define MIX_SWC        0x06       //3POS
#define MIX_SWD        0x07       //2POS

typedef struct
{
    uint8_t gimbalChannel;//所设置通道
    uint8_t reverse; // 0为正向，1为反向
    uint8_t weight; // 范围0~100
    uint8_t offset; //范围为0~200
    uint16_t output;
}mixData_t;

extern uint16_t controlMode;
extern mixData_t mixData[8];
extern uint8_t mixUpdateFlag;

void mixesTask(void* param);
extern TaskHandle_t mixesTaskHandle;
extern QueueHandle_t mixesValQueue;
void Mixes_Init(void);
void Mixes_Update(void);
void Mixes_ChannelInit(uint8_t channel);
void Mixes_ChannelUpdate(uint8_t channel);
uint16_t Mixes_Switchreverse(uint8_t reverse, uint16_t gimbalValCurr);
uint16_t Mixes_Gimbalreverse(uint8_t reverse, uint16_t gimbalValCurr,uint16_t* outputcode);
uint16_t Mixes_GimbalWeight(uint8_t weight, uint16_t gimbalValCurr);
uint16_t Mixes_GimbalOffset(uint8_t offset, uint16_t gimbalValCurr);
//void Get_MixesSet(void);

#endif
