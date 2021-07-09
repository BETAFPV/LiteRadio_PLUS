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
#define CRSF_FRAMETYPE_PARAMETER_WRITE     0x2D

#define LinkStatisticsFrameLength 10 

typedef enum
{
    CONFIG_CRSF_OFF = 0,
    CONFIG_CRSF_ON = 1
} crsfConfigStatus_e; 

typedef enum
{
    RATE_500HZ = 0,
    RATE_250HZ = 1,
    RATE_200HZ = 2,
    RATE_150HZ = 3,
    RATE_100HZ = 4,
    RATE_50HZ = 5,
    RATE_25HZ = 6,
    RATE_4HZ = 7,
    RATE_ENUM_MAX = 8
} expresslrs_RFrates_e; 

typedef enum
{
    TLM_RATIO_NO_TLM = 0,
    TLM_RATIO_1_128 = 1,
    TLM_RATIO_1_64 = 2,
    TLM_RATIO_1_32 = 3,
    TLM_RATIO_1_16 = 4,
    TLM_RATIO_1_8 = 5,
    TLM_RATIO_1_4 = 6,
    TLM_RATIO_1_2 = 7

} expresslrs_tlm_ratio_e;

typedef struct
{
    uint8_t setDataFlag;
    uint8_t setDataType;
    uint8_t setDataParameter;
    uint8_t lastConfigStatus;
    uint8_t configStatus;
    uint8_t inBindingMode;
    uint8_t webUpdateMode;
    uint8_t RSSI;
    uint8_t power;
    uint8_t regulatoryDomainIndex;
    uint8_t rate;
    uint8_t TLM;
}crsfData_t;

extern crsfData_t crsfData;

void CRSF_SetBind(void);
void CRSF_Init(uint8_t protocolIndex);
uint16_t CRSF_Process(uint16_t* controlData);
void Send_CRSFParameterPackage(uint8_t dataType,uint8_t dataParameter);
void Get_CRSFPackage(uint8_t* channelToCRSF,uint16_t* controlDataBuff);
void Get_LinkStatis(uint8_t* CRSF_RXPacket);
#endif
