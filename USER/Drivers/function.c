#include "function.h"
#include "mixes.h"
#define SBUS_VALUE  993

uint16_t channelDataBuff[4];

const unsigned char crc8tab[256] = {
  0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54,
  0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
  0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06,
  0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
  0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0,
  0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
  0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2,
  0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
  0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9,
  0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
  0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B,
  0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
  0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D,
  0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
  0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F,
  0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
  0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB,
  0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
  0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9,
  0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
  0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F,
  0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
  0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D,
  0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
  0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26,
  0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
  0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74,
  0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
  0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82,
  0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
  0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0,
  0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
};



uint16_t map(double Oxy, double Omin, double Omax, double Nmin, double Nmax)
{
	uint16_t Nxy;
//    float temp = 0.0;
    //temp = ((Nmax-Nmin)/(Omax-Omin)*(Oxy-1500.0)+(Nmin+Nmax)/2.0);
	Nxy = (Nmax-Nmin)/(Omax-Omin)*(Oxy-Omin)+Nmin;
   // Nxy = (uint16_t)temp;
	return Nxy;
}

/*继电器特性*/
uint8_t Cal_ElectricRelay(uint16_t currentValue,uint8_t currentStatus,uint16_t downValueLimit,uint16_t upValueLimit)
{
    if(currentStatus)
    {
        if(currentValue > downValueLimit)
        {
            return UP_VALUE_STATUS;
        }
        else
        {
            return DOWN_VALUE_STATUS;
        }
    }
    else
    {
        if(currentValue < upValueLimit)
        {
            return DOWN_VALUE_STATUS;
        }
        else
        {
            return UP_VALUE_STATUS;
        }        
    
    }
    
}

void Get_ChipID(union ChipID *chipID)
{
    chipID->ChipUniqueID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
    chipID->ChipUniqueID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
    chipID->ChipUniqueID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 

uint16_t GetUniqueID(void)
{
	uint16_t ID = 0 ; 
    union ChipID chipID;
	Get_ChipID(&chipID);
	ID = chipID.IDbyte[0] + chipID.IDbyte[2] + chipID.IDbyte[4] + chipID.IDbyte[6] + chipID.IDbyte[8] + chipID.IDbyte[10];
	ID = (ID << 8) + chipID.IDbyte[1] + chipID.IDbyte[3] + chipID.IDbyte[5] + chipID.IDbyte[7] + chipID.IDbyte[9] + chipID.IDbyte[11];
	return ID;
}

void Get_CRSFUniqueID(uint8_t *masterUID)
{
	union ChipID chipID;
	Get_ChipID(&chipID);
	masterUID[0] = chipID.IDbyte[5];
    masterUID[1] = chipID.IDbyte[4];
    masterUID[2] = chipID.IDbyte[3];
    masterUID[3] = chipID.IDbyte[2];
    masterUID[4] = chipID.IDbyte[1];
    masterUID[5] = chipID.IDbyte[0];
}

/*SBUS串口输出*/
void GetSbusPackage(uint8_t* channelToSBUS,uint16_t* controlDataBuff)
{	
	uint16_t switch_A_Temp;
	uint16_t switch_B_Temp;
	uint16_t switch_C_Temp;
	uint16_t switch_D_Temp;    

	channelDataBuff[MIX_ELEVATOR] = map(controlDataBuff[MIX_ELEVATOR],1000,2000,178,1811);
	channelDataBuff[MIX_AILERON] = map(controlDataBuff[MIX_AILERON],1000,2000,178,1811);
	channelDataBuff[MIX_THROTTLE] = map(controlDataBuff[MIX_THROTTLE],1000,2000,178,1811);
	channelDataBuff[MIX_RUDDER] = map(controlDataBuff[MIX_RUDDER] ,1000,2000,178,1811);
    
	switch_A_Temp = controlDataBuff[4];
	switch_B_Temp = controlDataBuff[5];
	switch_C_Temp = controlDataBuff[6];
	switch_D_Temp = controlDataBuff[7];
	
	channelToSBUS[0] = 0x0F;  
	channelToSBUS[1] = (uint8_t) (channelDataBuff[MIX_AILERON]   & 0x07FF);
    channelToSBUS[2] = (uint8_t) ((channelDataBuff[MIX_AILERON]  & 0x07FF)>>8  | (channelDataBuff[MIX_ELEVATOR] & 0x07FF)<<3);
    channelToSBUS[3] = (uint8_t) ((channelDataBuff[MIX_ELEVATOR] & 0x07FF)>>5  | (channelDataBuff[MIX_THROTTLE] & 0x07FF)<<6);
    channelToSBUS[4] = (uint8_t) ((channelDataBuff[MIX_THROTTLE] & 0x07FF)>>2);
    channelToSBUS[5] = (uint8_t) ((channelDataBuff[MIX_THROTTLE] & 0x07FF)>>10 | (channelDataBuff[MIX_RUDDER]   & 0x07FF)<<1);
    channelToSBUS[6] = (uint8_t) ((channelDataBuff[MIX_RUDDER]   & 0x07FF)>>7  | (switch_A_Temp & 0x07FF)<<4);
    channelToSBUS[7] = (uint8_t) ((switch_A_Temp & 0x07FF)>>4 | (switch_B_Temp & 0x07FF)<<7);
    channelToSBUS[8] = (uint8_t) ((switch_B_Temp & 0x07FF)>>1);
    channelToSBUS[9] = (uint8_t) ((switch_B_Temp & 0x07FF)>>9 | (switch_C_Temp & 0x07FF)<<2);
    channelToSBUS[10]= (uint8_t) ((switch_C_Temp & 0x07FF)>>6 | (switch_D_Temp & 0x07FF)<<5);
    channelToSBUS[11]= (uint8_t) ((switch_D_Temp& 0x07FF)>>3);
	channelToSBUS[12] = (SBUS_VALUE  >> 0 ) & 0xff;
    channelToSBUS[13] = (SBUS_VALUE << 3 | SBUS_VALUE  >> 8  ) & 0xff;
    channelToSBUS[14] = (SBUS_VALUE << 8 | SBUS_VALUE >> 5  ) & 0xff;
    channelToSBUS[15] = (SBUS_VALUE >> 2 ) & 0xff;
    channelToSBUS[16] = (SBUS_VALUE << 1 | SBUS_VALUE >> 10 ) & 0xff;
    channelToSBUS[17] = (SBUS_VALUE << 4 | SBUS_VALUE >> 7  ) & 0xff;
    channelToSBUS[18] = (SBUS_VALUE << 7 | SBUS_VALUE >> 4 ) & 0xff;
    channelToSBUS[19] = (SBUS_VALUE >> 1 ) & 0xff;
    channelToSBUS[20] = (SBUS_VALUE << 2 | SBUS_VALUE >> 9 ) & 0xff;
    channelToSBUS[21] = (SBUS_VALUE << 5 | SBUS_VALUE >> 6 ) & 0xff;
    channelToSBUS[22] = (SBUS_VALUE >> 3 ) & 0xff;
	channelToSBUS[23] = 0x00;
	channelToSBUS[24] = 0x00;
}

uint8_t crc8(const uint8_t * ptr, uint32_t len)
{
  uint8_t crc = 0;
  for (uint32_t i=0; i<len; i++) {
    crc = crc8tab[crc ^ *ptr++];
  }
  return crc;
}
