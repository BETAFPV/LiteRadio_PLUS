#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"

#define D16_INTERVAL   9
#define D8_INTERVAL    9
#define SFHSS_INTERVAL 2
#define CRSF_INTERVAL  4

#define REQUEST_INFO_ID  0x11
#define REQUESET_SAVE_ID 0x12
#define REQUESET_CRSF_ID 0x13

#define CONFIGER_INFO_ADDR                  0x08007064
#define CONFIGER_INFO_PROTOCOL_ADDR         0x08007064
#define CONFIGER_INFO_MODE_ADDR             0x08007066
#define INTERNAL_CONFIGER_INFO_POWER_ADDR   0x08007068  


#define RADIOLINK_BIND	        (0x01 << 0)	
#define RADIOLINK_DATA	        (0x01 << 1)	


extern TaskHandle_t radiolinkTaskHandle;
extern EventGroupHandle_t radioEventHandle;
void Version_Init(uint16_t protocolIndex);
void radiolinkTask(void* param);


#endif

