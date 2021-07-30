#include "common.h"
#include "sx1280.h"
#include "fhss.h"
#include "radiolink.h"
#include "function.h"
#include "crsf.h"
#define RX_CONNECTION_LOST_TIMEOUT 3000LU // After 3000ms of no TLM response consider that slave has lost connection

volatile uint8_t NonceTX;
crsfLinkStatistics_t linkStatistics;

//// MSP Data Handling ///////
uint8_t NextPacketIsMspData = 0;  // if true the next packet will contain the msp data

////////////SYNC PACKET/////////
/// sync packet spamming on mode change vars ///
#define syncSpamAResidualTimeMS 1500 // we spam some more after rate change to help link get up to speed
#define syncSpamAmount 3
volatile uint8_t syncSpamCounter = 0;
uint32_t rfModeLastChangedMS = 0;
////////////////////////////////////////////////

uint32_t SyncPacketLastSent = 0;

volatile uint32_t LastTLMpacketRecvMillis = 0;
uint32_t TLMpacketReported = 0;

//LQCALC<10> LQCalc;
//LPF LPD_DownlinkLQ(1);

volatile uint8_t busyTransmitting;
volatile uint8_t UpdateParamReq = 0;
uint32_t HWtimerPauseDuration = 0;

uint8_t WaitRXresponse = 0;
uint8_t WaitEepromCommit = 0;

uint8_t InBindingMode = 0;
uint8_t BindingPackage[5];
uint8_t BindingSendCount = 0;

uint16_t firmwareRev;

uint8_t baseMac[6];
uint16_t inCRC;
void ProcessTLMpacket()
{
    inCRC = (((uint16_t)(SX1280.radioRXdataBuffer[0] & 0xFC)) << 6 ) | SX1280.radioRXdataBuffer[7];

    SX1280.radioRXdataBuffer[0] &= 0x03;
    uint16_t calculatedCRC = calcCrc14(SX1280.radioRXdataBuffer, 7, CRCInitializer);

    uint8_t type = SX1280.radioRXdataBuffer[0] & TLM_PACKET;
    uint8_t TLMheader = SX1280.radioRXdataBuffer[1];

    if ((inCRC != calculatedCRC))
    {
        return;
    }

    if (type != TLM_PACKET)
    {
        return;
    }

    if (connectionState != connected)
    {
        connectionState = connected;
        // LPD_DownlinkLQ.init(100);
        //Serial.println("got downlink conn");
    }

    LastTLMpacketRecvMillis = HAL_GetTick();
    // LQCalc.add();

    switch(TLMheader & ELRS_TELEMETRY_TYPE_MASK)
    {
        case ELRS_TELEMETRY_TYPE_LINK:
            // RSSI received is signed, proper polarity (negative value = -dBm)
            linkStatistics.uplink_RSSI_1 = SX1280.radioRXdataBuffer[2];
            linkStatistics.uplink_RSSI_2 = SX1280.radioRXdataBuffer[3];
            linkStatistics.uplink_SNR = SX1280.radioRXdataBuffer[4];
            linkStatistics.uplink_Link_quality = SX1280.radioRXdataBuffer[5];
          //  linkStatistics.uplink_TX_Power = POWERMGNT.powerToCrsfPower(POWERMGNT.currPower());
            linkStatistics.downlink_SNR = SX1280.LastPacketSNR;
            linkStatistics.downlink_RSSI = SX1280.LastPacketRSSI;
            //linkStatistics.downlink_Link_quality = LPD_DownlinkLQ.update(LQCalc.getLQ()) + 1; // +1 fixes rounding issues with filter and makes it consistent with RX LQ Calculation
          //  linkStatistics.rf_Mode = (uint8_t)RATE_4HZ - (uint8_t)ExpressLRS_currAirRate_Modparams->enum_rate;
            //MspSender.ConfirmCurrentPayload(Radio.RXdataBuffer[6] == 1);
            break;

    }
}

void GenerateSyncPacketData()
{
    const uint8_t SwitchEncMode = 0x01;
    uint8_t Index;
    uint8_t TLMrate;
    if (syncSpamCounter)
    {
        Index = (tx_config.rate & 0x03);
        TLMrate = (tx_config.tlm & 0x07);
    }
    else
    {
        Index = (ExpressLRS_currAirRate_Modparams->index & 0x03);
        TLMrate = (ExpressLRS_currAirRate_Modparams->TLMinterval & 0x07);
    }

    SX1280.radioTXdataBuffer[0] = SYNC_PACKET & 0x03;
    SX1280.radioTXdataBuffer[1] = FHSSgetCurrIndex();
    SX1280.radioTXdataBuffer[2] = NonceTX;
    SX1280.radioTXdataBuffer[3] = (Index << 6) + (TLMrate << 3) + (SwitchEncMode << 1);
    SX1280.radioTXdataBuffer[4] = UID[3];
    SX1280.radioTXdataBuffer[5] = UID[4];
    SX1280.radioTXdataBuffer[6] = UID[5];
  
    SyncPacketLastSent = HAL_GetTick();
    if (syncSpamCounter)
        --syncSpamCounter;
}

void SetRFLinkRate(uint8_t index) // Set speed of RF link (hz)
{
    expresslrs_mod_settings_s *const ModParams = get_elrs_airRateConfig(index);
    expresslrs_rf_pref_params_s *const RFperf = get_elrs_RFperfParams(index);
    uint8_t invertIQ = UID[5] & 0x01;
//    if ((ModParams == ExpressLRS_currAirRate_Modparams)
//        && (RFperf == ExpressLRS_currAirRate_RFperfParams)
//        && (invertIQ == SX1280.IQinverted))
//    return;

    SX1280_Config(ModParams->bw, ModParams->sf, ModParams->cr, GetInitialFreq(), ModParams->PreambleLen, invertIQ);

    ExpressLRS_currAirRate_Modparams = ModParams;
    ExpressLRS_currAirRate_RFperfParams = RFperf;

    connectionState = disconnected;
    rfModeLastChangedMS = HAL_GetTick();
}

void HandleFHSS()
{
    if (InBindingMode)
    {
        return;
    }

    uint8_t modresult = (NonceTX) % ExpressLRS_currAirRate_Modparams->FHSShopInterval;

    if (modresult == 0) // if it time to hop, do so.
    {
        SX1280_SetFrequencyReg(FHSSgetNextFreq());
    }
}

void HandleTLM()
{
    if (ExpressLRS_currAirRate_Modparams->TLMinterval > 0)
    {
        uint8_t modresult = (NonceTX) % TLMratioEnumToValue(ExpressLRS_currAirRate_Modparams->TLMinterval);
        if (modresult != 0) // wait for tlm response because it's time
        {
            return;
        }
        SX1280_RXnb();
        WaitRXresponse = 1;
    }
}

void SX1280_SetBind()
{
    //EnterBindingMode();
    tx_config.rate = 0x03;
    tx_config.tlm = 0x01;


    syncSpamCounter = syncSpamAmount;
    tx_config.modify = 1;
}




uint16_t SendRCdataToRF(uint16_t* crsfcontrol_data)
{

    busyTransmitting = 1;
    uint8_t *data;
    uint8_t maxLength;
    uint8_t packageIndex;

    /////// This Part Handles the Telemetry Response ///////
    if ((uint8_t)ExpressLRS_currAirRate_Modparams->TLMinterval > 0)
    {
        uint8_t modresult = (NonceTX) % TLMratioEnumToValue(ExpressLRS_currAirRate_Modparams->TLMinterval);
        if (modresult == 0)
        { // wait for tlm response
            if (WaitRXresponse == 1)
            {
                WaitRXresponse = 0;
                //LQCalc.inc();
                return 0;
            }
            else
            {
                NonceTX++;
            }
        }
    }
    if(tx_config.modify && (syncSpamCounter == 0) )
    {
        SX1280_SetPower((PowerLevels_e)DefaultPowerEnum);
        SetRFLinkRate(3);
        tx_config.modify = 0;
            radiolinkDelayTime = 20;
    }
    uint32_t SyncInterval;

    SyncInterval = (connectionState == connected) ? ExpressLRS_currAirRate_RFperfParams->SyncPktIntervalConnected : ExpressLRS_currAirRate_RFperfParams->SyncPktIntervalDisconnected;

    uint8_t skipSync = 0;

    uint8_t NonceFHSSresult = NonceTX % ExpressLRS_currAirRate_Modparams->FHSShopInterval;
    uint8_t NonceFHSSresultWindow = (NonceFHSSresult == 1 || NonceFHSSresult == 2) ? 1 : 0; // restrict to the middle nonce ticks (not before or after freq chance)
    uint8_t WithinSyncSpamResidualWindow = (HAL_GetTick() - rfModeLastChangedMS < syncSpamAResidualTimeMS) ? 1 : 0;
    if((syncSpamCounter || WithinSyncSpamResidualWindow) && NonceFHSSresultWindow)
    {
        GenerateSyncPacketData();
    }
    else if((!skipSync) && ((HAL_GetTick() > (SyncPacketLastSent + SyncInterval)) && (SX1280.currFreq == GetInitialFreq()) && NonceFHSSresultWindow)) // don't sync just after we changed freqs (helps with hwTimer.init() being in sync from the get go)
    {
        GenerateSyncPacketData();
    }
    else
    {
        if(NextPacketIsMspData && StubbornSender_IsActive())
        {
            StubbornSender_GetCurrentPayload(&packageIndex, &maxLength, &data);
            
            SX1280.radioTXdataBuffer[0] = MSP_DATA_PACKET & 0x03;
            SX1280.radioTXdataBuffer[1] = packageIndex; 
            SX1280.radioTXdataBuffer[2] = maxLength > 0 ? *data : 0;
            SX1280.radioTXdataBuffer[3] = maxLength >= 1 ? *(data + 1) : 0;
            SX1280.radioTXdataBuffer[4] = maxLength >= 2 ? *(data + 2) : 0;
            SX1280.radioTXdataBuffer[5] = maxLength >= 3 ? *(data + 3) : 0;
            SX1280.radioTXdataBuffer[6] = maxLength >= 4 ? *(data + 4) : 0;           
            // send channel data next so the channel messages also get sent during msp transmissions
            NextPacketIsMspData = 0;
            // counter can be increased even for normal msp messages since it's reset if a real bind message should be sent
            BindingSendCount++;
            if (InBindingMode)
            {
                // exit bind mode if package after some repeats
                if (BindingSendCount > 6) 
                {
                    ExitBindingMode();
                }
            }
        }
        else
        {
            // always enable msp after a channel package since the slot is only used if MspSender has data to send
            NextPacketIsMspData = 1;
            GenerateChannelDataHybridSwitch8(SX1280.radioTXdataBuffer, crsfcontrol_data);
        }
    }

    ///// Next, Calculate the CRC and put it into the buffer /////

    uint16_t crc = calcCrc14(SX1280.radioTXdataBuffer, 7, CRCInitializer);
    SX1280.radioTXdataBuffer[0] |= (crc >> 6) & 0xFC;
    SX1280.radioTXdataBuffer[7] = crc & 0xFF;

    SX1280_TXnb(SX1280.radioTXdataBuffer, 8);
}

/*
 * Called as the timer ISR when transmitting
 */
void timerCallbackNormal()
{
// Do not send a stale channels packet to the RX if one has not been received from the handset
// *Do* send data if a packet has never been received from handset and the timer is running
//     this is the case when bench testing and TXing without a handset
//  uint32_t lastRcData = crsf.GetRCdataLastRecv();
//  if (!lastRcData || (micros() - lastRcData < 1000000))
//  {
//    busyTransmitting = 1;
//    SendRCdataToRF();
//  }
}

/*
 * Called as the timer ISR while waiting for eeprom flush
 */
void timerCallbackIdle()
{
    NonceTX++;
    if (NonceTX % ExpressLRS_currAirRate_Modparams->FHSShopInterval == 0)
    {
        FHSSptr++;
    }
}


void ParamUpdateReq()
{
    UpdateParamReq = 1;
}



void RXdoneISR()
{
    ProcessTLMpacket();
    busyTransmitting = 0;
}

void TXdoneISR()
{
    busyTransmitting = 0;
    NonceTX++; // must be done before callback
    HandleFHSS();
    HandleTLM();
}


void setup(uint8_t protocolIndex)
{
    Get_CRSFUniqueID(MasterUID);
    UID[0] = MasterUID[0];
    UID[1] = MasterUID[1];
    UID[2] = MasterUID[2];
    UID[3] = MasterUID[3];
    UID[4] = MasterUID[4];
    UID[5] = MasterUID[5];
    
    CRCInitializer = (UID[4] << 8) | UID[5];
    FHSSrandomiseFHSSsequence(UID); 
    
    SX1280_Reset();
    while(firmwareRev == 0|| (firmwareRev == 65535))
    {
        firmwareRev= SX1280_GetFirmwareVersion();
    }

//    #if !defined(Regulatory_Domain_ISM_2400)
//    //Radio.currSyncWord = UID[3];
//    #endif
    SX1280_Init();

    SX1280_SetPower((PowerLevels_e)DefaultPowerEnum);
    SetRFLinkRate(RATE_DEFAULT);
    generateCrc14Table();
 // ExpressLRS_currAirRate_Modparams->TLMinterval = TLM_RATIO_1_64;
}

void loop()
{
    uint32_t now = HAL_GetTick();
    
    //HandleUpdateParameter();
    //CheckConfigChangePending();

    if (now > (RX_CONNECTION_LOST_TIMEOUT + LastTLMpacketRecvMillis))
    {
        connectionState = disconnected;
    }
    else
    {
        connectionState = connected;
    }



    // only send msp data when binding is not active
    if (InBindingMode)
    {
        // exit bind mode if package after some repeats
        if (BindingSendCount > 6) 
        {
            ExitBindingMode();
        }
    }
}

//void OnRFModePacket(mspPacket_t *packet)
//{
//  // Parse the RF mode
//  uint8_t rfMode = packet->readByte();
//  CHECK_PACKET_PARSING();

//  switch (rfMode)
//  {
//  case RATE_200HZ:
//  case RATE_100HZ:
//  case RATE_50HZ:
//    SetRFLinkRate(enumRatetoIndex((expresslrs_RFrates_e)rfMode));
//    break;
//  default:
//    // Unsupported rate requested
//    break;
//  }
//}

//void OnTxPowerPacket(mspPacket_t *packet)
//{
//  // Parse the TX power
//  uint8_t txPower = packet->readByte();
//  CHECK_PACKET_PARSING();
//  //Serial.println("TX setpower");

//  if (txPower < PWR_COUNT)
//    SX1280_SetPower((PowerLevels_e)txPower);
//}

//void OnTLMRatePacket(mspPacket_t *packet)
//{
//  // Parse the TLM rate
//  // uint8_t tlmRate = packet->readByte();
//  CHECK_PACKET_PARSING();

//  // TODO: Implement dynamic TLM rates
//  // switch (tlmRate) {
//  // case TLM_RATIO_NO_TLM:
//  //   break;
//  // case TLM_RATIO_1_128:
//  //   break;
//  // default:
//  //   // Unsupported rate requested
//  //   break;
//  // }
//}


void EnterBindingMode()
{
    if (InBindingMode) 
    {
        // Don't enter binding if we're already binding
        return;
    }

    // Disable the TX timer and wait for any TX to complete
    // hwTimer.stop();
    while (busyTransmitting);

    // Queue up sending the Master UID as MSP packets
    SendUIDOverMSP();

    // Set UID to special binding values
    UID[0] = BindingUID[0];
    UID[1] = BindingUID[1];
    UID[2] = BindingUID[2];
    UID[3] = BindingUID[3];
    UID[4] = BindingUID[4];
    UID[5] = BindingUID[5];

    CRCInitializer = 0;
    InBindingMode = 1;

    // Start attempting to bind
    // Lock the RF rate and freq while binding
    SetRFLinkRate(0);
    SX1280.currFreq = GetInitialFreq(); //set frequency first or an error will occur!!!
    SX1280_SetFrequencyReg(SX1280.currFreq); 
    // Start transmitting again
}

void ExitBindingMode()
{
    if (!InBindingMode)
    {
        // Not in binding mode
        return;
    }

    // Reset UID to defined values
    UID[0] = MasterUID[0];
    UID[1] = MasterUID[1];
    UID[2] = MasterUID[2];
    UID[3] = MasterUID[3];
    UID[4] = MasterUID[4];
    UID[5] = MasterUID[5];

    CRCInitializer = (UID[4] << 8) | UID[5];

    InBindingMode = 0;
    SetRFLinkRate(RATE_DEFAULT); //return to original rate
    StubbornSender_ResetState();
    radiolinkDelayTime = 4;
}

void SendUIDOverMSP()
{
    BindingPackage[0] = MSP_ELRS_BIND;
    BindingPackage[1] = MasterUID[2];
    BindingPackage[2] = MasterUID[3];
    BindingPackage[3] = MasterUID[4];
    BindingPackage[4] = MasterUID[5];
    StubbornSender_ResetState();
    BindingSendCount = 0;
    StubbornSender_SetDataToTransmit(5, BindingPackage, ELRS_MSP_BYTES_PER_CALL);
    InBindingMode = 1;
}


