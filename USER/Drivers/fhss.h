#ifndef _FHSS_H_
#define _FHSS_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

union ChipID{
	uint32_t ChipUniqueID[3];
	uint8_t IDbyte[12];
};



uint16_t GetUniqueID(void);
void Get_ChipID(union ChipID *chipID);
void Frsky_init_hop(uint16_t TransmitterID,uint8_t* FRSKYD16_HOPChannel);
#endif
