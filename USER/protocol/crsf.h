#ifndef __CRSF_H_
#define __CRSF_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define DEFAULT_VALUE  993

/*crsf address*/
#define CRSF_ADDRESS_RADIO_TRANSMITTER     0xEA
#define CRSF_ADDRESS_CRSF_TRANSMITTER      0xEE

/*crsf frame type*/
#define CRSF_FRAMETYPE_LINK_STATISTICS     0x14
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED  0x16
#define CRSF_FRAMETYPE_RADIO_ID            0x3A

#define LinkStatisticsFrameLength 10 

void CRSF_SetBind(void);
void CRSF_Init(uint8_t protocolIndex);
uint16_t CRSF_Process(uint16_t* controlData);
void Send_CRSFParameterPackage(uint8_t dataType,uint8_t dataParameter);
void Get_CRSFPackage(uint8_t* channelToCRSF,uint16_t* controlDataBuff);
void Get_LinkStatis(uint8_t* CRSF_RXPacket);
#endif
