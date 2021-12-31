#ifndef __SX1280_H_
#define __SX1280_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "sx1280hal.h"
#include "sx1280reg.h"
#include "common.h"

#define TXRXBuffSize 8 

#if defined(TARGET_TX_BETAFPV_2400_V1)
#define MaxPower PWR_100mW
#define DefaultPowerEnum PWR_100mW
#endif

typedef enum
{
    SX1280_INTERRUPT_NONE,
    SX1280_INTERRUPT_RX_DONE,
    SX1280_INTERRUPT_TX_DONE
}SX1280_InterruptAssignment_e;

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
    PowerLevels_e currentPower;
}POWERMGNT_t;

extern POWERMGNT_t POWERMGNT;

typedef struct
{
    uint8_t radioTXdataBuffer[TXRXBuffSize]; // ELRS uses max of 8 bytes
    uint8_t radioRXdataBuffer[TXRXBuffSize];
    SX1280_RadioOperatingModes_t currOpmode;
    SX1280_RadioLoRaBandwidths_t currBW;
    SX1280_RadioLoRaSpreadingFactors_t currSF;
    SX1280_RadioLoRaCodingRates_t currCR;
    uint32_t currFreq;
    int8_t LastPacketRSSI;
    int8_t LastPacketSNR;
    uint8_t IQinverted;
    SX1280_InterruptAssignment_e InterruptAssignment;
}SX1280_t;

extern SX1280_t SX1280;
void SX1280_init(uint8_t protocolIndex);
void ProcessTLMpacket(void);
void setup(void);
void loop(void);
uint16_t SX1280_Process(uint16_t* crsfcontrol_data);

void RXdoneISR(void);
void TXdoneISR(void);
void EnterBindingMode(void);
void ExitBindingMode(void);
void SendUIDOverMSP(void);

void SX1280_Init(void);
void SX1280_Reset(void);
uint16_t SX1280_GetFirmwareVersion(void);
void SX1280_SetMode(SX1280_RadioOperatingModes_t OPmode);
void SX1280_ConfigLoRaModParams(SX1280_RadioLoRaBandwidths_t bw, SX1280_RadioLoRaSpreadingFactors_t sf, SX1280_RadioLoRaCodingRates_t cr);
void SX1280_SetPacketParams(uint8_t PreambleLength, SX1280_RadioLoRaPacketLengthsModes_t HeaderType, uint8_t PayloadLength, SX1280_RadioLoRaCrcModes_t crc, SX1280_RadioLoRaIQModes_t InvertIQ);
void SX1280_SetFrequencyReg(uint32_t freq);
void SX1280_SetFIFOaddr(uint8_t txBaseAddr, uint8_t rxBaseAddr);    
void SX1280_SetDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask);
uint16_t SX1280_GetIrqStatus(void);
void SX1280_ClearIrqStatus(uint16_t irqMask);
uint8_t SX1280_GetRxBufferAddr(void);
void SX1280_GetLastPacketStats(void);
uint8_t SX1280_ReadRegister(uint16_t address);
void SX1280_WriteRegister(uint16_t address, uint8_t value);

void SetRFLinkRate(uint8_t index); // Set speed of RF link (hz)
void SX1280_Config(SX1280_RadioLoRaBandwidths_t bw, SX1280_RadioLoRaSpreadingFactors_t sf, SX1280_RadioLoRaCodingRates_t cr, uint32_t freq, uint8_t PreambleLength, uint8_t InvertIQ);

void SX1280_TXnb(volatile uint8_t *data, uint8_t length);
void SX1280_RXnb(void);
void SX1280_TXnbISR(void);
void SX1280_RXnbISR(void);
void  SX1280_IsrCallback(void);
uint16_t SendRCdataToRF(uint16_t* crsfcontrol_data);
/*power*/
PowerLevels_e SX1280_SetPower(PowerLevels_e Power);

#endif
