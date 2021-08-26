#ifndef __SX1276_H_
#define __SX1276_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "sx1276hal.h"
#include "sx1276reg.h"

#define TXRXBuffSize 8 

#define MaxPower PWR_100mW
#define DefaultPowerEnum PWR_100mW

typedef enum
{
    PWR_10mW = 0,
    PWR_25mW = 1,
    PWR_50mW = 2,
    PWR_100mW = 3,
    PWR_250mW = 4,
    PWR_500mW = 5,
    PWR_1000mW = 6,
    PWR_2000mW = 7,
    PWR_COUNT = 8
} PowerLevels_e;

typedef struct
{
    uint8_t radioTXdataBuffer[TXRXBuffSize]; // ELRS uses max of 8 bytes
    uint8_t radioRXdataBuffer[TXRXBuffSize];

    uint8_t headerExplMode;
    uint8_t crcEnabled;

    //// Parameters ////
    uint32_t currFreq; // leave as 0 to ensure that it gets set
    uint8_t currSyncWord;
    uint8_t currPreambleLen;
    SX127x_Bandwidth currBW; //default values from datasheet
    SX127x_SpreadingFactor currSF;
    SX127x_CodingRate currCR;
    SX127x_RadioOPmodes currOpmode;
    uint8_t currPWR;
    SX127x_ModulationModes ModFSKorLoRa;
    uint8_t IQinverted;
    ///////////////////////////////////

    /////////////Packet Stats//////////
    int8_t LastPacketRSSI;
    int8_t LastPacketSNR;
    uint32_t TimeOnAir;
    uint32_t TXstartMicros;
    uint32_t TXspiTime;
    uint32_t HeadRoom;
    uint32_t LastTXdoneMicros;
    uint32_t TXdoneMicros;
    uint8_t IRQneedsClear;
    SX1276_InterruptAssignment InterruptAssignment;
    
}SX1276_t;

extern SX1276_t SX1276;


void EnterBindingMode(void);
void ExitBindingMode(void);
void SendUIDOverMSP(void);

void setup(void);
void loop(void);
uint16_t SendRCdataToRF(uint16_t* crsfcontrol_data);
uint16_t SX1276_Process(uint16_t* controlDataBuff);
void ProcessTLMpacket(void);


uint8_t SX1276_DetectChip(void); 
void SX1276_Init(void);
void SX1276_Config(SX127x_Bandwidth bw, SX127x_SpreadingFactor sf, SX127x_CodingRate cr, uint32_t freq, uint8_t preambleLen, uint8_t InvertIQ);
void SX1276_ConfigLoraDefaults(void);
void SX1276_Reset(void);
void SX1276_SetMode(SX127x_RadioOPmodes mode);
void SX1276_ClearIRQFlags(void);
void SX1276_SetSyncWord(uint8_t syncWord);
uint8_t SyncWordOk(uint8_t syncWord);
void SX1276_SetPreambleLength(uint8_t PreambleLen);
void SX1276_SetOutputPower(uint8_t Power);
void SX1276_SetSpreadingFactor(SX127x_SpreadingFactor sf);
void SX1276_SetBandwidthCodingRate(SX127x_Bandwidth bw, SX127x_CodingRate cr);
void SX1276_SetFrequencyReg(uint32_t freq);
int8_t SX1276_GetLastPacketRSSI(void);
int8_t SX1276_GetLastPacketSNR(void);
void SX1276_HalRXenable(void);
void SX1276_HalTXenable(void);
void SX1276_RXnb(void);
void SX1276_TXnb(uint8_t volatile *data, uint8_t length);

void SX1276_RXnbISR(void);
void SX1276_TXnbISR(void);

void RXdoneISR(void);
void TXdoneISR(void);
#endif
