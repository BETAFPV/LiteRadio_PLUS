#include "fhss.h"

//获取芯片ID
uint16_t GetUniqueID(void)
{
	uint16_t ID = 0 ; 
	union ChipID chipID;
	Get_ChipID(&chipID);
	ID = chipID.IDbyte[0] + chipID.IDbyte[2] + chipID.IDbyte[4] + chipID.IDbyte[6] + chipID.IDbyte[8] + chipID.IDbyte[10];
	ID = (ID << 8) + chipID.IDbyte[1] + chipID.IDbyte[3] + chipID.IDbyte[5] + chipID.IDbyte[7] + chipID.IDbyte[9] + chipID.IDbyte[11];
	return ID;
}

void Get_ChipID(union ChipID *chipID)
{
    chipID->ChipUniqueID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
    chipID->ChipUniqueID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
    chipID->ChipUniqueID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 
//由ID生成跳频表
void Frsky_init_hop(uint16_t TransmitterID,uint8_t* FRSKYD16_HOPChannel)
{
  uint8_t val;
  uint8_t channel = TransmitterID&0x07;
  uint8_t channel_spacing = TransmitterID&0xFF;
  //Filter bad tables
  if(channel_spacing<0x02) channel_spacing+=0x02;
  if(channel_spacing>0xE9) channel_spacing-=0xE7;
  if(channel_spacing%0x2F==0) channel_spacing++;
    
  for(uint8_t i=0;i<50;i++)
  {
    channel=(channel+channel_spacing) % 0xEB;
    val=channel;
    if((val==0x00) || (val==0x5A) || (val==0xDC))
      val++;
    FRSKYD16_HOPChannel[i]=i>46?0:val;
  }

}
