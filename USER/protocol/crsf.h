#ifndef __CRSF_H_
#define __CRSF_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define RSSI_WARNING_VALUE 80

#define DEFAULT_VALUE  CRSF_CHANNEL_VALUE_MID

#define CRSF_CHANNEL_VALUE_MIN 172
#define CRSF_CHANNEL_VALUE_MID 992
#define CRSF_CHANNEL_VALUE_MAX 1811
#define CRSF_CHANNEL_VALUE_SPAN (CRSF_CHANNEL_VALUE_MAX - CRSF_CHANNEL_VALUE_MIN)

/*crsf address*/
#define CRSF_ADDRESS_RADIO_TRANSMITTER     0xEA
#define CRSF_ADDRESS_CRSF_TRANSMITTER      0xEE

/*crsf frame type*/
#define CRSF_FRAMETYPE_LINK_STATISTICS     0x14
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED  0x16
#define CRSF_FRAMETYPE_DEVICE_PING         0x28
#define CRSF_FRAMETYPE_PARAMETER_READ      0x2C
#define CRSF_FRAMETYPE_RADIO_ID            0x3A
#define CRSF_FRAMETYPE_PARAMETER_WRITE     0x2D

#define LinkStatisticsFrameLength 10 

#define FREQ_AU_915  1
#define FREQ_FCC_915 2
#define FREQ_EU_868 3
#define FREQ_AU_433 4
#define FREQ_EU_433 5
#define FREQ_ISM_2400 6


enum{
    rate =1,
    tlm = 2,
    power =6,
    regulatoryDomain = 66,
};
#define RADIO_ADDRESS                  0xEA
#define UART_SYNC                      0xC8
extern uint8_t maxPackSize;


enum{
    NANO_TX_915Mhz = 0x02,
    NANO_TX_868Mhz = 0x03,
    NANO_TX_2400Mhz = 0x06,
};

enum{
    power10mw,
    power25mw,
    power50mw,
    power100mw,
    power250mw,
    power500mw,
    power1000mw,
    power2000mw,
};

enum{
    power915Mhz100mw,
    power915Mhz250mw,
    power915Mhz500mw,
};

typedef enum
{
    CONFIG_CRSF_OFF = 0,
    CONFIG_CRSF_ON = 1
} crsfConfigStatus_e; 

typedef struct
{
    uint8_t power;
    uint8_t rate;
    uint8_t TLM;  
}
crsfParameter_t;
extern crsfParameter_t externalRFprarmeter;


typedef struct
{
    uint8_t connectionState;
    uint8_t setDataFlag;
    uint8_t setDataType;
    uint8_t setDataParameter;
    uint8_t configUpdateFlag;
    uint8_t configSetFlag;
    uint8_t lastConfigStatus;
    uint8_t configStatus;
    uint8_t inBindingMode;
    uint8_t webUpdateMode;
    uint8_t RSSI;
    uint8_t regulatoryDomainIndex;
    crsfParameter_t crsfParameter;
    crsfParameter_t lastCRSFparameter;
}crsfData_t;

typedef struct 
{
    uint8_t uplink_RSSI_1;
    uint8_t uplink_RSSI_2;
    uint8_t uplink_Link_quality;
    int8_t uplink_SNR;
    uint8_t active_antenna;
    uint8_t rf_Mode;
    uint8_t uplink_TX_Power;
    uint8_t downlink_RSSI;
    uint8_t downlink_Link_quality;
    int8_t downlink_SNR;
} crsfLinkStatistics_t;
extern crsfLinkStatistics_t linkStatistics;

extern crsfData_t internalCRSFdata;
extern crsfData_t externalCRSFdata;

void CRSF_SetBind(void);
void CRSF_Init(uint8_t protocolIndex);
uint16_t CRSF_Process(uint16_t* controlData);
void Send_CRSFParameterPackage(uint8_t dataType,uint8_t dataParameter);
void GetExternalRFParameter(uint8_t dataType,uint8_t chunk);
void GetExternalRFWorkFrequency(void);
void Get_CRSFPackage(uint8_t* channelToCRSF,uint16_t* controlDataBuff);
void Get_LinkStatis(uint8_t* CRSF_RXPacket);
#endif
