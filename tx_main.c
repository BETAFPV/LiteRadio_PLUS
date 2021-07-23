#include "sx1280.h"
#include "fhss.h"
#include "common.h"
void setup()
{
  FHSSrandomiseFHSSsequence(UID);

  SX1280_Init();

  SX1280_SetPower((PowerLevels_e)DefaultPowerEnum);

  // Set the pkt rate, TLM ratio, and power from the stored eeprom values
//  SetRFLinkRate(config.GetRate());
  ExpressLRS_currAirRate_Modparams->TLMinterval = TLM_RATIO_1_64;
  //SX1280_SetPower((PowerLevels_e)config.GetPower());
}

void RXdoneISR()
{
  ProcessTLMpacket();
}

void TXdoneISR()
{
//  NonceTX++; // must be done before callback
//  HandleFHSS();
//  HandleTLM();
}


void ProcessTLMpacket()
{
    uint16_t inCRC = (((uint16_t)(SX1280.radioRXdataBuffer[0] & 0xFC)) << 6 ) | SX1280.radioRXdataBuffer[7];
    uint16_t calculatedCRC = calcCrc14(SX1280.radioRXdataBuffer, 7, CRCInitializer); 
    if ((inCRC != calculatedCRC))
    {
        return;
    }

}