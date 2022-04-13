#ifndef __COMMON_H_
#define __COMMON_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "fhss.h"

#if defined(Regulatory_Domain_AU_915) || defined(Regulatory_Domain_EU_868) || defined(Regulatory_Domain_FCC_915) || defined(Regulatory_Domain_AU_433) || defined(Regulatory_Domain_EU_433)
#include "sx1276.h"
#endif

#if defined(Regulatory_Domain_ISM_2400)
#include "sx1280.h"
#endif


#define FIRST_FLASH_MARK_ADDR           0x8007A00

#define LITE_RADIO_HARDWARE_TYPE_ADDR   0x8007A02
#define INTERNAL_RADIO_TYPE_ADDR        0x8007A04
#define THROTTLE_ROCKER_POSITION_ADDR   0x8007A06
#define HARDWARE_MAJOR_VERSION_ADDR     0x8007A08
#define HARDWARE_MINOR_VERSION_ADDR     0x8007A10
#define HARDWARE_PATCH_VERSION_ADDR     0x8007A12

#define FIRMWARE_MAJOR_VERSION (0x02U)
#define FIRMWARE_MINOR_VERSION (0x00U)
#define FIRMWARE_PITCH_VERSION (0x02U)
typedef struct
{
    uint8_t MAJOR_VERSION;
    uint8_t MINOR_VERSION;
    uint8_t PITCH_VERSIO;
}Hardware_Version_t;

enum{
    SX1280_ELRS,
    SX1280_CRSF,
};

enum{
    CC2500_FCC_D16,
    CC2500_LBT_D16,
    CC2500_D8,
    CC2500_FHSS,
    CC2500_CRSF,
};

#define INTERNAL_ELRS_CONFIGER_INFO_ADDR           0x08007068
#define INTERNAL_ELRS_CONFIGER_INFO_POWER_ADDR     0x08007068
#define INTERNAL_ELRS_CONFIGER_INFO_Rate_ADDR      0x0800706A
#define INTERNAL_ELRS_CONFIGER_INFO_TLM_ADDR       0x0800706C

#define ELRS_CRC_LEN 256
#define ELRS_CRC_POLY 0x07 // 0x83
#define ELRS_CRC14_POLY 0x2E57 // 0x372B

#define ELRS_TELEMETRY_TYPE_LINK 0x01
#define ELRS_TELEMETRY_TYPE_DATA 0x02
#define ELRS_TELEMETRY_TYPE_MASK 0x03
#define ELRS_TELEMETRY_SHIFT 2
#define ELRS_TELEMETRY_BYTES_PER_CALL 5
#define ELRS_TELEMETRY_MAX_PACKAGES (255 >> ELRS_TELEMETRY_SHIFT)

#define ELRS_MSP_BYTES_PER_CALL 5
#define ELRS_MSP_MAX_PACKAGES 4
#define ELRS_MSP_BUFFER 15
#define ELRS_MSP_REQ_TIMEOUT_MS 1000

#define MSP_ELRS_BIND       0x09

// expresslrs packet header types
// 00 -> standard 4 channel data packet
// 01 -> switch data packet
// 11 -> tlm packet
// 10 -> sync packet with hop data
#define RC_DATA_PACKET 0x00
#define MSP_DATA_PACKET 0x01
#define TLM_PACKET 0x03
#define SYNC_PACKET 0x02

// current and sent switch values
#define N_SWITCHES 8

#define AUX1 4
#define AUX2 5
#define AUX3 6
#define AUX4 7
#define AUX5 8
#define AUX6 9
#define AUX7 10
#define AUX8 11

extern uint8_t BindingUID[6];
extern uint8_t UID[6];
extern uint8_t MasterUID[6];
extern uint16_t CRCInitializer;

typedef enum {
    SENDER_IDLE = 0,
    SENDING,
    SEND_NEXT,
    WAIT_UNTIL_NEXT_CONFIRM,
    RESYNC
} stubborn_sender_state_s;


typedef struct {
    uint32_t    version;
    uint32_t    lastRate;
    uint32_t    rate;
    uint32_t    lastTLM;
    uint32_t    tlm;
    uint32_t    lastPower;
    uint32_t    power;
    uint8_t     modify;
} tx_config_t;

extern tx_config_t tx_config;

typedef struct
{

    uint8_t *data;
    uint8_t length;
    uint8_t bytesPerCall;
    uint8_t currentOffset;
    uint8_t currentPackage;
    uint8_t waitUntilTelemetryConfirm;
    uint8_t resetState;
    uint16_t waitCount;
    uint16_t maxWaitCount;
    uint8_t maxPackageIndex;
    volatile stubborn_sender_state_s senderState;
}StubbornSender_t;

extern StubbornSender_t StubbornSender;



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

typedef enum
{
    bad_sync_retry = 4,
    bad_sync = 3,
    connected = 2,
    tentative = 1,
    disconnected = 0
} connectionState_e;

typedef enum
{
    tim_disconnected = 0,
    tim_tentative = 1,
    tim_locked = 2
} RXtimerState_e;

extern connectionState_e connectionState;
extern connectionState_e connectionStatePrev;

typedef enum
{
    RF_DOWNLINK_INFO = 0,
    RF_UPLINK_INFO = 1,
    RF_AIRMODE_PARAMETERS = 2
} expresslrs_tlm_header_e;

typedef enum
{
    FREQ_2400_RATE_500HZ = 0,
    FREQ_2400_RATE_250HZ = 1,
    FREQ_2400_RATE_150HZ = 2,
    FREQ_2400_RATE_50HZ = 3,
} expresslrs_RF_FREQ_2400_rates_e; 

typedef enum
{
    FREQ_900_RATE_200HZ = 0,
    FREQ_900_RATE_100HZ = 1,
    FREQ_900_RATE_50HZ = 2,
    FREQ_900_RATE_25HZ = 3,
} expresslrs_RF_FREQ_900_rates_e; 

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
} expresslrs_RFrates_e; // Max value of 16 since only 4 bits have been assigned in the sync package.

typedef struct expresslrs_rf_pref_params_s
{
    int8_t index;
    expresslrs_RFrates_e enum_rate; // Max value of 16 since only 4 bits have been assigned in the sync package.
    int32_t RXsensitivity;          // expected RF sensitivity based on
    uint32_t TOA;                   // time on air in microseconds
    uint32_t DisconnectTimeoutMs;
    uint32_t RxLockTimeoutMs;
    uint32_t SyncPktIntervalDisconnected; // how often to send the SYNC_PACKET packet (ms) when there is no response from RX
    uint32_t SyncPktIntervalConnected;    // how often to send the SYNC_PACKET packet (ms) when there we have a connection

} expresslrs_rf_pref_params_s;

#if  defined(Regulatory_Domain_EU_868) || defined(Regulatory_Domain_FCC_915)
#define RATE_MAX 4
#define RATE_DEFAULT 0
typedef struct expresslrs_mod_settings_s
{
    int8_t index;
    expresslrs_RFrates_e enum_rate; // Max value of 16 since only 4 bits have been assigned in the sync package.
    SX127x_Bandwidth bw;
    SX127x_SpreadingFactor sf;
    SX127x_CodingRate cr;
    uint32_t interval;                  // interval in us seconds that corresponds to that frequency
    expresslrs_tlm_ratio_e TLMinterval; // every X packets is a response TLM packet, should be a power of 2
    uint8_t FHSShopInterval;            // every X packets we hop to a new frequency. Max value of 16 since only 4 bits have been assigned in the sync package.
    uint8_t PreambleLen;

} expresslrs_mod_settings_s;

#endif

#if defined(Regulatory_Domain_ISM_2400)
#define RATE_MAX 4
#define RATE_DEFAULT 3
#define RATE_BINDING 3
typedef struct expresslrs_mod_settings_s
{
    int8_t index;
    expresslrs_RFrates_e enum_rate; // Max value of 16 since only 4 bits have been assigned in the sync package.
    SX1280_RadioLoRaBandwidths_t bw;
    SX1280_RadioLoRaSpreadingFactors_t sf;
    SX1280_RadioLoRaCodingRates_t cr;
    uint32_t interval;                  // interval in us seconds that corresponds to that frequency
    expresslrs_tlm_ratio_e TLMinterval; // every X packets is a response TLM packet, should be a power of 2
    uint8_t FHSShopInterval;            // every X packets we hop to a new frequency. Max value of 16 since only 4 bits have been assigned in the sync package.
    uint8_t PreambleLen;
    uint8_t PayloadLength;            // Number of OTA bytes to be sent.
} expresslrs_mod_settings_s;

#endif

#if  defined(Regulatory_Domain_EU_868) || defined(Regulatory_Domain_FCC_915) || defined(Regulatory_Domain_ISM_2400)

expresslrs_mod_settings_s *get_elrs_airRateConfig(int8_t index);
expresslrs_rf_pref_params_s *get_elrs_RFperfParams(int8_t index);

uint8_t TLMratioEnumToValue(expresslrs_tlm_ratio_e enumval);
uint16_t RateEnumToHz(expresslrs_RFrates_e eRate);

extern expresslrs_mod_settings_s *ExpressLRS_currAirRate_Modparams;
extern expresslrs_rf_pref_params_s *ExpressLRS_currAirRate_RFperfParams;
extern uint8_t ExpressLRS_nextAirRateIndex;
//extern expresslrs_mod_settings_s *ExpressLRS_nextAirRate;
//extern expresslrs_mod_settings_s *ExpressLRS_prevAirRate;
uint8_t enumRatetoIndex(expresslrs_RFrates_e rate);

void GenerateChannelDataHybridSwitch8(volatile uint8_t* Buffer, uint16_t* controlDataBuff);
int8_t CRSF_GetNextSwitchIndex(void);
void CRSF_SetSentSwitch(uint8_t index, uint8_t value);
void CRSF_UpdateSwitchValues(uint16_t* elrsControlData);
uint8_t CRSF_to_BIT(uint16_t val);
uint16_t CRSF_to_N(uint16_t val, uint16_t cnt);


void ExpressLRS_Init(uint8_t protocolIndex);

void StubbornSender_GetCurrentPayload(uint8_t *packageIndex, uint8_t *count, uint8_t **currentData);
void StubbornSender_SetDataToTransmit(uint8_t lengthToTransmit, uint8_t* dataToTransmit, uint8_t bytesPerCall);
void StubbornSender_ResetState(void);
uint8_t StubbornSender_IsActive(void);

void generateCrc14Table(void);
uint16_t calcCrc14(uint8_t *data, uint8_t len, uint16_t crc);
uint32_t uidMacSeedGet(void);
#endif

#endif
