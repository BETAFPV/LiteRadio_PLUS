#include "common.h"
#include "function.h"
#include "mixes.h"
#include "crsf.h"
uint8_t UID[6];
uint16_t crc14tab[ELRS_CRC_LEN] = {0};
uint16_t elrsControlData[8] = {0};
StubbornSender_t StubbornSender;
tx_config_t tx_config;

static uint8_t currentSwitches[N_SWITCHES] = {0};
static uint8_t sentSwitches[N_SWITCHES] = {0};
static uint8_t nextSwitchIndex = 0; // for round-robin sequential switches
#if defined(Regulatory_Domain_AU_915) || defined(Regulatory_Domain_EU_868) || defined(Regulatory_Domain_FCC_915) 

#include "sx1276.h"

expresslrs_mod_settings_s ExpressLRS_AirRateConfig[RATE_MAX] = {
    {0, RATE_200HZ, SX127x_BW_500_00_KHZ, SX127x_SF_6, SX127x_CR_4_7, 5000, TLM_RATIO_1_64, 4, 8},
    {1, RATE_100HZ, SX127x_BW_500_00_KHZ, SX127x_SF_7, SX127x_CR_4_7, 10000, TLM_RATIO_1_64, 4, 8},
    {2, RATE_50HZ, SX127x_BW_500_00_KHZ, SX127x_SF_8, SX127x_CR_4_7, 20000, TLM_RATIO_NO_TLM, 4, 10},
    {3, RATE_25HZ, SX127x_BW_500_00_KHZ, SX127x_SF_9, SX127x_CR_4_7, 40000, TLM_RATIO_NO_TLM, 4, 10}};

expresslrs_rf_pref_params_s ExpressLRS_AirRateRFperf[RATE_MAX] = {
    {0, RATE_200HZ, -112, 4380, 3000, 2500, 2000, 4000},
    {1, RATE_100HZ, -117, 8770, 3500, 2500, 2000, 4000},
    {2, RATE_50HZ, -120, 17540, 4000, 2500, 2000, 4000},
    {3, RATE_25HZ, -123, 17540, 6000, 4000, 2000, 4000}};
#endif

#if defined(Regulatory_Domain_ISM_2400)

#include "sx1280.h"

expresslrs_mod_settings_s ExpressLRS_AirRateConfig[RATE_MAX] = {
  {0, RATE_500HZ, SX1280_LORA_BW_0800, SX1280_LORA_SF5, SX1280_LORA_CR_LI_4_6, 2000, TLM_RATIO_1_128, 4, 12, 8},
    {1, RATE_250HZ, SX1280_LORA_BW_0800, SX1280_LORA_SF6, SX1280_LORA_CR_LI_4_7, 4000, TLM_RATIO_1_64, 4, 14, 8},
    {2, RATE_150HZ, SX1280_LORA_BW_0800, SX1280_LORA_SF7, SX1280_LORA_CR_LI_4_7, 6666, TLM_RATIO_1_32, 4, 12, 8},
    {3, RATE_50HZ, SX1280_LORA_BW_0800, SX1280_LORA_SF9, SX1280_LORA_CR_LI_4_6, 20000, TLM_RATIO_NO_TLM, 2, 12, 8}};

expresslrs_rf_pref_params_s ExpressLRS_AirRateRFperf[RATE_MAX] = {
    {0, RATE_500HZ, -105, 1665, 2500, 2500, 3, 5000},
    {1, RATE_250HZ, -108, 3300, 3000, 2500, 6, 5000},
    {2, RATE_150HZ, -112, 5871, 3500, 2500, 10, 5000},
    {3, RATE_50HZ, -117, 18443, 4000, 2500, 0, 5000}};
#endif

//const expresslrs_mod_settings_s * ExpressLRS_nextAirRate;
expresslrs_mod_settings_s *ExpressLRS_currAirRate;
expresslrs_mod_settings_s *ExpressLRS_prevAirRate;

expresslrs_mod_settings_s *get_elrs_airRateConfig(int8_t index)
{
    // Protect against out of bounds rate
    if (index < 0)
    {
        // Set to first entry in the array
        return &ExpressLRS_AirRateConfig[0];
    }
    else if (index > (RATE_MAX - 1))
    {
        // Set to last usable entry in the array
        return &ExpressLRS_AirRateConfig[RATE_MAX - 1];
    }
    return &ExpressLRS_AirRateConfig[index];
}

expresslrs_rf_pref_params_s *get_elrs_RFperfParams(int8_t index)
{
    // Protect against out of bounds rate
    if (index < 0)
    {
        // Set to first entry in the array
        return &ExpressLRS_AirRateRFperf[0];
    }
    else if (index > (RATE_MAX - 1))
    {
        // Set to last usable entry in the array
        return &ExpressLRS_AirRateRFperf[RATE_MAX - 1];
    }
    return &ExpressLRS_AirRateRFperf[index];
}

uint8_t enumRatetoIndex(expresslrs_RFrates_e rate)
{ // convert enum_rate to index
    for (int i = 0; i < RATE_MAX; i++)
    {
        expresslrs_mod_settings_s *const ModParams = get_elrs_airRateConfig(i);
        if (ModParams->enum_rate == rate)
        {
            return i;
        }
    }
    // If 25Hz selected and not available, return the slowest rate available
    // else return the fastest rate available (500Hz selected but not available)
    return (rate == RATE_25HZ) ? RATE_MAX - 1 : 0;
}

expresslrs_mod_settings_s *ExpressLRS_currAirRate_Modparams;
expresslrs_rf_pref_params_s *ExpressLRS_currAirRate_RFperfParams;

uint8_t ExpressLRS_nextAirRateIndex = 0;

connectionState_e connectionState = disconnected;
connectionState_e connectionStatePrev = disconnected;

uint8_t BindingUID[6] = {0, 1, 2, 3, 4, 5}; // Special binding UID values

uint8_t MasterUID[6]; // Special binding UID values

uint16_t CRCInitializer;

#define RSSI_FLOOR_NUM_READS 5 // number of times to sweep the noise foor to get avg. RSSI reading
#define MEDIAN_SIZE 20

uint8_t TLMratioEnumToValue(expresslrs_tlm_ratio_e enumval)
{
    switch (enumval)
    {
        case TLM_RATIO_NO_TLM:
            return 1;
        case TLM_RATIO_1_2:
            return 2;
        case TLM_RATIO_1_4:
            return 4;
        case TLM_RATIO_1_8:
            return 8;
        case TLM_RATIO_1_16:
            return 16;
        case TLM_RATIO_1_32:
            return 32;
        case TLM_RATIO_1_64:
            return 64;
        case TLM_RATIO_1_128:
            return 128;
        default:
            return 0;
    }
}

uint16_t RateEnumToHz(expresslrs_RFrates_e eRate)
{
    switch(eRate)
    {
    case RATE_500HZ: return 500;
    case RATE_250HZ: return 250;
    case RATE_200HZ: return 200;
    case RATE_150HZ: return 150;
    case RATE_100HZ: return 100;
    case RATE_50HZ: return 50;
    case RATE_25HZ: return 25;
    case RATE_4HZ: return 4;
    default: return 1;
    }
}

void GenerateChannelDataHybridSwitch8(volatile uint8_t* Buffer, uint16_t* elrsControlData)
{
    uint32_t dataVal[16];
    uint16_t elrsChannelDataBuff[4];
    elrsChannelDataBuff[MIX_ELEVATOR] = map(elrsControlData[MIX_ELEVATOR],1000,2000,192,1792);
    elrsChannelDataBuff[MIX_AILERON] = map(elrsControlData[MIX_AILERON],1000,2000,192,1792);
    elrsChannelDataBuff[MIX_THROTTLE] = map(elrsControlData[MIX_THROTTLE],1000,2000,192,1792);
    elrsChannelDataBuff[MIX_RUDDER] = map(elrsControlData[MIX_RUDDER],1000,2000,192,1792);  
    
    dataVal[0] = elrsChannelDataBuff[MIX_AILERON];
    dataVal[1] = elrsChannelDataBuff[MIX_ELEVATOR];
    dataVal[2] = elrsChannelDataBuff[MIX_THROTTLE];
    dataVal[3] = elrsChannelDataBuff[MIX_RUDDER];  
    
    Buffer[0] = RC_DATA_PACKET & 0x03;
    Buffer[1] = ((dataVal[0]) >> 3);
    Buffer[2] = ((dataVal[1]) >> 3);
    Buffer[3] = ((dataVal[2]) >> 3);
    Buffer[4] = ((dataVal[3]) >> 3);
    Buffer[5] = ((dataVal[0] & 0x06) << 5) |
                           ((dataVal[1] & 0x06) << 3) |
                           ((dataVal[2] & 0x06) << 1) |
                           ((dataVal[3] & 0x06) >> 1);
                           
    CRSF_UpdateSwitchValues(elrsControlData);
    
    uint8_t nextSwitchIndex = CRSF_GetNextSwitchIndex();
    // Actually send switchIndex - 1 in the packet, to shift down 1-7 (0b111) to 0-6 (0b110)
    // If the two high bits are 0b11, the receiver knows it is the last switch and can use
    // that bit to store data
    uint8_t bitclearedSwitchIndex = nextSwitchIndex - 1;
    // currentSwitches[] is 0-15 for index 6, 0-2 for index 0-5
    // Rely on currentSwitches to *only* have values in that rang
    uint8_t value = currentSwitches[nextSwitchIndex];

    Buffer[6] =   currentSwitches[0] << 6 |bitclearedSwitchIndex << 3 | value;
    
    CRSF_SetSentSwitch(nextSwitchIndex, value);
}

void CRSF_UpdateSwitchValues(uint16_t* elrsControlData)
{
    uint32_t dataVal[16];

    dataVal[4] = map(elrsControlData[4],1000,2000,192,1792);
    dataVal[5] = map(elrsControlData[5],1000,2000,192,1792);
    dataVal[6] = map(elrsControlData[6],1000,2000,192,1792);
    dataVal[7] = map(elrsControlData[7],1000,2000,192,1792);  
    dataVal[8] = DEFAULT_VALUE;
    dataVal[9] = DEFAULT_VALUE;
    dataVal[10] = DEFAULT_VALUE;    
    dataVal[11] = DEFAULT_VALUE;  
    dataVal[12] = DEFAULT_VALUE;
    dataVal[13] = DEFAULT_VALUE;
    dataVal[14] = DEFAULT_VALUE;
    dataVal[15] = DEFAULT_VALUE;
    
    // AUX1 is arm switch, one bit
    currentSwitches[0] = CRSF_to_BIT(dataVal[4]);

    // AUX2-(N-1) are Low Resolution, "7pos" (6+center)
    const uint16_t CHANNEL_BIN_COUNT = 6;
    const uint16_t CHANNEL_BIN_SIZE = CRSF_CHANNEL_VALUE_SPAN / CHANNEL_BIN_COUNT;
    for (int i = 1; i < N_SWITCHES-1; i++)
    {
        uint16_t ch = dataVal[i + 4];
        // If channel is within 1/4 a BIN of being in the middle use special value 7
        if (ch < (CRSF_CHANNEL_VALUE_MID-CHANNEL_BIN_SIZE/4)
            || ch > (CRSF_CHANNEL_VALUE_MID+CHANNEL_BIN_SIZE/4))
            currentSwitches[i] = CRSF_to_N(ch, CHANNEL_BIN_COUNT) & 0x07;
        else
            currentSwitches[i] = 7;
    } // for N_SWITCHES

    // AUXx is High Resolution 16-pos (4-bit)
    currentSwitches[N_SWITCHES-1] = CRSF_to_N(dataVal[N_SWITCHES-1 + 4], 16) & 0x0F;
}

void CRSF_SetSentSwitch(uint8_t index, uint8_t value)
{
    sentSwitches[index] = value;
}

int8_t CRSF_GetNextSwitchIndex()
{
    int firstSwitch = 0; // sequential switches includes switch 0

    firstSwitch = 1; // skip 0 since it is sent on every packet

    // look for a changed switch
    int i;
    for (i = firstSwitch; i < N_SWITCHES; i++)
    {
        if (currentSwitches[i] != sentSwitches[i])
            break;
    }
    // if we didn't find a changed switch, we get here with i==N_SWITCHES
    if (i == N_SWITCHES)
    {
        i = nextSwitchIndex;
    }

    // keep track of which switch to send next if there are no changed switches
    // during the next call.
    nextSwitchIndex = (i + 1) % 8;


    // for hydrid switches 0 is sent on every packet, skip it in round-robin
    if (nextSwitchIndex == 0)
    {
        nextSwitchIndex = 1;
    }

    return i;
}

// Returns 1 if val is greater than CRSF_CHANNEL_VALUE_MID
uint8_t CRSF_to_BIT(uint16_t val)
{
    return (val > CRSF_CHANNEL_VALUE_MID) ? 1 : 0;
}


// Convert CRSF (172-1811) to 0-(cnt-1)
uint16_t CRSF_to_N(uint16_t val, uint16_t cnt)
{
    // The span is increased by one to prevent the max val from returning cnt
    return (val - CRSF_CHANNEL_VALUE_MIN) * cnt / (CRSF_CHANNEL_VALUE_SPAN + 1);
}


void StubbornSender_GetCurrentPayload(uint8_t *packageIndex, uint8_t *count, uint8_t **currentData)
{
    switch (StubbornSender.senderState)
    {
        case RESYNC:
            *packageIndex = StubbornSender.maxPackageIndex;
            *count = 0;
            *currentData = 0;
            break;
        case SENDING:
            *currentData = StubbornSender.data + StubbornSender.currentOffset;
            *packageIndex = StubbornSender.currentPackage;
            if (StubbornSender.bytesPerCall > 1)
            {
                if (StubbornSender.currentOffset + StubbornSender.bytesPerCall <= StubbornSender.length)
                {
                    *count = StubbornSender.bytesPerCall;
                }
                else
                {
                    *count = StubbornSender.length-StubbornSender.currentOffset;
                }
            }
            else
            {
                *count = 1;
            }
            break;
        default:
            *count = 0;
            *currentData = 0;
            *packageIndex = 0;
    }
}

void StubbornSender_SetDataToTransmit(uint8_t lengthToTransmit, uint8_t* dataToTransmit, uint8_t bytesPerCall)
{
    StubbornSender.maxPackageIndex = 4;
    if (StubbornSender.senderState != SENDER_IDLE || lengthToTransmit / bytesPerCall >= StubbornSender.maxPackageIndex)
    {
        return;
    }

    StubbornSender.length = lengthToTransmit;
    StubbornSender.data = dataToTransmit;
    StubbornSender.currentOffset = 0;
    StubbornSender.currentPackage = 1;
    StubbornSender.waitCount = 0;
    StubbornSender.bytesPerCall = bytesPerCall;
    StubbornSender.senderState = SENDING;
}


void StubbornSender_ResetState()
{
    StubbornSender.data = 0;
    StubbornSender.bytesPerCall = 1;
    StubbornSender.currentOffset = 0;
    StubbornSender.currentPackage = 0;
    StubbornSender.length = 0;
    StubbornSender.waitUntilTelemetryConfirm = 1;
    StubbornSender.waitCount = 0;
    StubbornSender.maxWaitCount = 1000;
    StubbornSender.senderState = SENDER_IDLE;
}

uint8_t StubbornSender_IsActive()
{
    return StubbornSender.senderState != SENDER_IDLE;
}

void generateCrc14Table(void)
{
    uint16_t crc;
    for (uint16_t i = 0; i < ELRS_CRC_LEN; i++) {
        crc = i << (14 - 8);
        for (uint8_t j = 0; j < 8; j++) {
            crc = (crc << 1) ^ ((crc & 0x2000) ? ELRS_CRC14_POLY : 0);
        }
        crc14tab[i] = crc;
    }
}

uint16_t calcCrc14(uint8_t *data, uint8_t len, uint16_t crc)
{
    while (len--) {
        crc = (crc << 8) ^ crc14tab[((crc >> 6) ^ (uint16_t) *data++) & 0x00FF];
    }
    return crc & 0x3FFF;
}

uint32_t uidMacSeedGet(void)
{
    const uint32_t macSeed = ((uint32_t)UID[2] << 24) + ((uint32_t)UID[3] << 16) +
                             ((uint32_t)UID[4] << 8) + UID[5];
    return macSeed;
}
