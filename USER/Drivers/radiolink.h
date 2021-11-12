#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"

#if defined(LiteRadio_Plus_CC2500) 
#define VERSION_INDEX 0
#elif defined(LiteRadio_Plus_SX1280)
#define VERSION_INDEX 1
#elif defined(LiteRadio_Plus_SX1276)
#define VERSION_INDEX 2
#endif

#define D16_INTERVAL   2
#define D8_INTERVAL    2
#define SFHSS_INTERVAL 2
#define CRSF_INTERVAL  2

#define REQUEST_INFO_ID  0x11

#define CONFIGER_INFO_ADDR                  0x08007060
#define CONFIGER_INFO_PROTOCOL_ADDR         0x08007060
#define CONFIGER_INFO_MODE_ADDR             0x08007062

#define INTERNAL_CONFIGER_INFO_POWER_ADDR   0x08007064  


#define RADIOLINK_BIND	        (0x01 << 0)	
#define RADIOLINK_DATA	        (0x01 << 1)	
#if defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276) || (LiteRadio_Plus_CC2500)
extern uint16_t channelData[16];
#endif
extern uint32_t radiolinkDelayTime ;
extern TaskHandle_t radiolinkTaskHandle;
extern EventGroupHandle_t radioEventHandle;

void Version_Init(uint16_t protocolIndex);
void radiolinkTask(void* param);
uint8_t Get_Protocol_Select(void);

#endif

