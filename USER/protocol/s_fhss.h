#ifndef __S_FHSS_
#define __S_FHSS_
#include "stm32f1xx_hal.h"
//Channel definitions
#define	CH1		0
#define	CH2		1
#define	CH3		2
#define	CH4		3
#define	CH5		4
#define	CH6		5
#define	CH7		6
#define	CH8		7
#define	CH9		8
#define	CH10	9
#define	CH11	10
#define	CH12	11
#define	CH13	12
#define	CH14	13
#define	CH15	14
#define	CH16	15

//#define	AILERON  0
//#define	ELEVATOR 1
//#define	THROTTLE 2
//#define	RUDDER   3

//Channel MIN MAX values
#define CHANNEL_MAX_100	1844	//	100%
#define CHANNEL_MIN_100	204		//	100%
#define CHANNEL_MAX_125	2047	//	125%
#define CHANNEL_MIN_125	0		//	125%



void initSFHSS(uint8_t protocolIndex);
uint16_t ReadSFHSS(uint16_t* controlData);
void SFHSS_SetBind(void);
#endif
