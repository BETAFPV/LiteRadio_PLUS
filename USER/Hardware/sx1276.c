#include "sx1276.h"
#include "gpio.h"

const uint8_t SX127x_AllowedSyncwords[105] =
    {0, 5, 6, 7, 11, 12, 13, 15, 18,
     21, 23, 26, 29, 30, 31, 33, 34,
     37, 38, 39, 40, 42, 44, 50, 51,
     54, 55, 57, 58, 59, 61, 63, 65,
     67, 68, 71, 77, 78, 79, 80, 82,
     84, 86, 89, 92, 94, 96, 97, 99,
     101, 102, 105, 106, 109, 111, 113, 115,
     117, 118, 119, 121, 122, 124, 126, 127,
     129, 130, 138, 143, 161, 170, 172, 173,
     175, 180, 181, 182, 187, 190, 191, 192,
     193, 196, 199, 201, 204, 205, 208, 209,
     212, 213, 219, 220, 221, 223, 227, 229,
     235, 239, 240, 242, 243, 246, 247, 255};
     


uint8_t version;

SX1276_t SX1276;

void SX1276_Init()
{
    SX1276.headerExplMode = 0;
    SX1276.crcEnabled = 0;
    SX1276.currFreq = 0; // leave as 0 to ensure that it gets set
    SX1276.currSyncWord = SX127X_SYNC_WORD;
    SX1276.currPreambleLen = 0;
    SX1276.currBW = SX127x_BW_125_00_KHZ; //default values from datasheet
    SX1276.currSF = SX127x_SF_7;
    SX1276.currCR = SX127x_CR_4_5;
    SX1276.currOpmode = SX127x_OPMODE_SLEEP;
    SX1276.currPWR = 0x00;
    SX1276.ModFSKorLoRa = SX127x_OPMODE_LORA;
    SX1276.IQinverted = 0;
    SX1276.IRQneedsClear = 1;
    
}


uint8_t SX1276_DetectChip()
{
    uint8_t i = 0;
    uint8_t flagFound = 0;
    while ((i < 3) && !flagFound)
    {
        version = SX1276_HalReadRegister(SX127X_REG_VERSION);
        if (version == 0x12)
        {
            flagFound = 1;
        }
        else
        {
            HAL_Delay(200);
            i++;
        }
    }

    if (!flagFound)
    {
        return 0;
    }

    SX1276_HalSetRegValue(SX127X_REG_OP_MODE, SX127x_OPMODE_SLEEP, 2, 0);
    return 1;
}



void SX1276_Config(SX127x_Bandwidth bw, SX127x_SpreadingFactor sf, SX127x_CodingRate cr, uint32_t freq, uint8_t preambleLen, uint8_t InvertIQ)
{
  SX1276.IQinverted = InvertIQ;
  SX1276_ConfigLoraDefaults();
  SX1276_SetPreambleLength(preambleLen);
  SX1276_SetOutputPower(SX1276.currPWR);
  SX1276_SetSpreadingFactor(sf);
  SX1276_SetBandwidthCodingRate(bw, cr);
  SX1276_SetFrequencyReg(freq);
}



void SX1276_ConfigLoraDefaults()
{

  SX1276_HalWriteRegister(SX127X_REG_OP_MODE, SX127x_OPMODE_SLEEP);
  SX1276_HalWriteRegister(SX127X_REG_OP_MODE, SX1276.ModFSKorLoRa); //must be written in sleep mode
  SX1276_SetMode(SX127x_OPMODE_STANDBY);

  SX1276.IRQneedsClear = 1;
  SX1276_ClearIRQFlags();

  SX1276_HalWriteRegister(SX127X_REG_PAYLOAD_LENGTH, 8);
  SX1276_SetSyncWord(SX1276.currSyncWord);
  SX1276_HalWriteRegister(SX127X_REG_FIFO_TX_BASE_ADDR, SX127X_FIFO_TX_BASE_ADDR_MAX);
  SX1276_HalWriteRegister(SX127X_REG_FIFO_RX_BASE_ADDR, SX127X_FIFO_RX_BASE_ADDR_MAX);
  SX1276_HalSetRegValue(SX127X_REG_DIO_MAPPING_1, 0xC0, 7, 6); //undocumented "hack", looking at Table 18 from datasheet SX127X_REG_DIO_MAPPING_1 = 11 appears to be unspported by infact it generates an intterupt on both RXdone and TXdone, this saves switching modes.
  SX1276_HalWriteRegister(SX127X_REG_LNA, SX127X_LNA_BOOST_ON);
  SX1276_HalWriteRegister(SX1278_REG_MODEM_CONFIG_3, SX1278_AGC_AUTO_ON | SX1278_LOW_DATA_RATE_OPT_OFF);
  SX1276_HalSetRegValue(SX127X_REG_OCP, SX127X_OCP_ON | SX127X_OCP_150MA, 5, 0); //150ma max current
  SX1276_SetPreambleLength(SX127X_PREAMBLE_LENGTH_LSB);
  SX1276_HalSetRegValue(SX127X_REG_INVERT_IQ, (uint8_t)SX1276.IQinverted, 6, 6);
}

void SX1276_Reset(void)
{
    HAL_GPIO_WritePin(SX1280_RST_GPIO_Port, SX1280_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(SX1280_RST_GPIO_Port, SX1280_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
}



void SX1276_SetMode(SX127x_RadioOPmodes mode)
{ //if radio is not already in the required mode set it to the requested mod
  if (SX1276.currOpmode != mode)
  {
    SX1276_HalWriteRegister(SX1276.ModFSKorLoRa | SX127X_REG_OP_MODE, mode);
    SX1276.currOpmode = mode;
  }
}

void SX1276_ClearIRQFlags()
{
  if (SX1276.IRQneedsClear)
  {
    SX1276_HalWriteRegister(SX127X_REG_IRQ_FLAGS, 0xFF);
    SX1276.IRQneedsClear = 0;
  }
}


void SX1276_SetSyncWord(uint8_t syncWord)
{
  uint8_t _syncWord = syncWord;

  while (SyncWordOk(_syncWord) == 0)
  {
    _syncWord++;
  }

  if(syncWord != _syncWord){
//    Serial.print("Using syncword: ");
//    Serial.print(_syncWord);
//    Serial.print(" instead of: ");
//    Serial.println(syncWord);
  }

  SX1276_HalWriteRegister(SX127X_REG_SYNC_WORD, _syncWord);
  SX1276.currSyncWord = _syncWord;
}



uint8_t SyncWordOk(uint8_t syncWord)
{
  for (unsigned int i = 0; i < sizeof(SX127x_AllowedSyncwords); i++)
  {
    if (syncWord == SX127x_AllowedSyncwords[i])
    {
      return 1;
    }
  }
  return 0;
}


void SX1276_SetPreambleLength(uint8_t PreambleLen)
{
  if (SX1276.currPreambleLen != PreambleLen)
  {
    SX1276_HalWriteRegister(SX127X_REG_PREAMBLE_LSB, PreambleLen);
    SX1276.currPreambleLen = PreambleLen;
  }
}


void SX1276_SetOutputPower(uint8_t Power)
{
  SX1276_SetMode(SX127x_OPMODE_STANDBY);
  SX1276_HalWriteRegister(SX127X_REG_PA_CONFIG, SX127X_PA_SELECT_BOOST | SX127X_MAX_OUTPUT_POWER | Power);
  SX1276.currPWR = Power;
}

void SX1276_SetSpreadingFactor(SX127x_SpreadingFactor sf)
{
  if (SX1276.currSF != sf)
  {
    SX1276_HalSetRegValue(SX127X_REG_MODEM_CONFIG_2, sf | SX127X_TX_MODE_SINGLE, 7, 3);
    if (sf == SX127x_SF_6)
    {
      SX1276_HalSetRegValue(SX127X_REG_DETECT_OPTIMIZE, SX127X_DETECT_OPTIMIZE_SF_6, 2, 0);
      SX1276_HalWriteRegister(SX127X_REG_DETECTION_THRESHOLD, SX127X_DETECTION_THRESHOLD_SF_6);
    }
    else
    {
      SX1276_HalSetRegValue(SX127X_REG_DETECT_OPTIMIZE, SX127X_DETECT_OPTIMIZE_SF_7_12, 2, 0);
      SX1276_HalWriteRegister(SX127X_REG_DETECTION_THRESHOLD, SX127X_DETECTION_THRESHOLD_SF_7_12);
    }
    SX1276.currSF = sf;
  }
}

void SX1276_SetBandwidthCodingRate(SX127x_Bandwidth bw, SX127x_CodingRate cr)
{
    if ((SX1276.currBW != bw) || (SX1276.currCR != cr))
    {
        if (SX1276.currSF == SX127x_SF_6) // set SF6 optimizations
        {
            SX1276_HalWriteRegister(SX127X_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_IMPL_MODE);
            SX1276_HalSetRegValue(SX127X_REG_MODEM_CONFIG_2, SX1278_RX_CRC_MODE_OFF, 2, 2);
        }
        else
        {
            if (SX1276.headerExplMode)
            {
                SX1276_HalWriteRegister(SX127X_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_EXPL_MODE);
            }
            else
            {
                SX1276_HalWriteRegister(SX127X_REG_MODEM_CONFIG_1, bw | cr | SX1278_HEADER_IMPL_MODE);
            }

            if (SX1276.crcEnabled)
            {
                SX1276_HalSetRegValue(SX127X_REG_MODEM_CONFIG_2, SX1278_RX_CRC_MODE_ON, 2, 2);
            }
            else
            {
                SX1276_HalSetRegValue(SX127X_REG_MODEM_CONFIG_2, SX1278_RX_CRC_MODE_OFF, 2, 2);
            }
        }

        if (bw == SX127x_BW_500_00_KHZ)
        {
            //datasheet errata reconmendation http://caxapa.ru/thumbs/972894/SX1276_77_8_ErrataNote_1.1_STD.pdf
            SX1276_HalWriteRegister(0x36, 0x02);
            SX1276_HalWriteRegister(0x3a, 0x64);
        }
        else
        {
            SX1276_HalWriteRegister(0x36, 0x03);
        }
        SX1276.currCR = cr;
        SX1276.currBW = bw;
    }
}

void SX1276_SetFrequencyReg(uint32_t freq)
{
  SX1276.currFreq = freq;
  SX1276_SetMode(SX127x_OPMODE_STANDBY);

  uint8_t FRQ_MSB = (uint8_t)((freq >> 16) & 0xFF);
  uint8_t FRQ_MID = (uint8_t)((freq >> 8) & 0xFF);
  uint8_t FRQ_LSB = (uint8_t)(freq & 0xFF);

  uint8_t outbuff[3] = {FRQ_MSB, FRQ_MID, FRQ_LSB}; //check speedup

  SX1276_HalwriteRegisterBurst(SX127X_REG_FRF_MSB, outbuff, sizeof(outbuff));
}

void SX1276_RXnb()
{
  if (SX1276.currOpmode == SX127x_OPMODE_RXCONTINUOUS)
  {
  //   Serial.println("abort RX");
     return; // we were already TXing so abort
  }
  SX1276.IRQneedsClear = 1;
  SX1276_ClearIRQFlags();
  SX1276_SetMode(SX127x_OPMODE_STANDBY);
  SX1276_HalRXenable();
  SX1276_HalWriteRegister(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_RX_BASE_ADDR_MAX);
  SX1276_SetMode(SX127x_OPMODE_RXCONTINUOUS);
}

void SX1276_TXnb(uint8_t volatile *data, uint8_t length)
{
//    if (SX1276.currOpmode == SX127x_OPMODE_TX)
//    {
//        return; // we were already TXing so abort. this should never happen!!!
//    }

    SX1276_ClearIRQFlags();
    SX1276_SetMode(SX127x_OPMODE_STANDBY);
    SX1276_HalTXenable();

  //instance->TXstartMicros = micros();
  //instance->HeadRoom = instance->TXstartMicros - instance->TXdoneMicros;

  SX1276_HalWriteRegister(SX127X_REG_FIFO_ADDR_PTR, SX127X_FIFO_TX_BASE_ADDR_MAX);
  SX1276_HalWriteRegisterFIFO(data, length);

  SX1276_SetMode(SX127x_OPMODE_TX);
}

void SX1276_RXnbISR()
{
  //hal.TXRXdisable();
  SX1276.IRQneedsClear = 1;
  SX1276_ClearIRQFlags();
  SX1276_HalReadRegisterFIFO(SX1276.radioRXdataBuffer, 8);
  SX1276.LastPacketRSSI = SX1276_GetLastPacketRSSI();
  SX1276.LastPacketSNR = SX1276_GetLastPacketSNR();
  RXdoneISR();
}

void SX1276_TXnbISR()
{
    //hal.TXRXdisable();
    SX1276_ClearIRQFlags();
    SX1276.currOpmode = SX127x_OPMODE_STANDBY; //goes into standby after transmission
    //instance->TXdoneMicros = micros();
    TXdoneISR();
}


int8_t SX1276_GetLastPacketRSSI()
{
  return (-157 + SX1276_HalGetRegValue(SX127X_REG_PKT_RSSI_VALUE,7,0));
}

int8_t SX1276_GetLastPacketSNR()
{
  int8_t rawSNR = (int8_t)SX1276_HalGetRegValue(SX127X_REG_PKT_SNR_VALUE,7,0);
  return (rawSNR / 4.0);
}

