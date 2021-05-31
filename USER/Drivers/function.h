#ifndef __FUNCTION_H
#define __FUNCTION_H
#include "stm32f1xx_hal.h"
#include "gimbal.h"
#include "switches.h"

union ChipID{
	uint32_t ChipUniqueID[3];
	uint8_t IDbyte[12];
};

void Get_ChipID(union ChipID *chipID);
uint16_t GetUniqueID(void);
void GetSbusPackage(uint8_t* ChannelToSbus);
uint8_t crc8(const uint8_t * ptr, uint32_t len);

uint16_t map(float Oxy, float Omin, float Omax, float Nmin, float Nmax);
//定义各通道名称(最大支持8通道)
typedef enum
{
//  RUDDER   	= 0 , 
//	THROTTLE 	= 1 , 
//	ELEVATOR 	= 2 , 
//	AILERON  	= 3 , 
	AUX1  		= 4 , 
	AUX2		= 5 , 
	AUX3		= 6 , 
	AUX4		= 7 , 
	AUX5  		= 8 , 
	AUX6		= 9 , 
	AUX7		= 10 , 
	AUX8		= 11 , 
	AUX9  		= 12 , 
	AUX10		= 13 , 
	AUX11		= 14 , 
	AUX12		= 15 , 
}ChannelTypeDef ;


#endif
