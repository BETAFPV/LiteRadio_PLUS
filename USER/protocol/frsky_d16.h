#ifndef __FRSKY_D16_
#define __FRSKY_D16_
//#include "frsky_common.h"
//#include <stdbool.h>
#include "stm32f1xx.h"
#include "cmsis_os.h"

//#define THR_Output_Max 1995 
//#define THR_Output_Mid 1500
//#define THR_Output_Min 1005

////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//// 发送最大值 必须大于 发送最小值
////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#define Output_Max 2012 
//#define Output_Mid 1500
//#define Output_Min 988

#define LBT 1
#define FCC 0

//extern uint8_t versionSelectFlg;

//extern bool CommunicationError_flg ;

//#define RF_TypeVersion		      0x46						//遥控器类型 'F' -> FRSKYD16
//#define MasterInitProtocolVersion 0x01						//无线协议版本号
//#define PTOTOCOL_MAX_CHANNEL      8						//协议支持最大发送通道 8 通道(固定发送16通道数据)
//#define TRANSMITTER_CHANNEL       8

//extern bool CommunicationError_flg ; 
//extern bool highThrottleFlg ; 
//extern bool Bind_flg ; 
//extern uint16_t TransmitterID ; 
////////////////////////////////////////////////////////////////////////////////
//SFHSS 8通 无线数据发送数据
////////////////////////////////////////////////////////////////////////////////

extern TaskHandle_t frskyd16TaskHandle;

void FRSKYD16_Init(uint8_t protocolIndex);
uint16_t ReadFRSKYD16(uint16_t* controlData);
void SetBind(void) ; 
void FRSKYD16_build_Data_packet(uint16_t* controlData);

#endif
