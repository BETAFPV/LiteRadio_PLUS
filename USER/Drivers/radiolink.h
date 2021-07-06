#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"

#define D16_INTERVAL   9
#define D8_INTERVAL    9
#define SFHSS_INTERVAL 2
#define CRSF_INTERVAL  4

#define REQUEST_INFO_ID   0x11
#define REQUESET_SAVE_ID  0x12

#define CONFIGER_INFO_FLAG       0x8007064
#define CONFIGER_INFO_ADDR       0x8007066
#define CONFIGER_INFO_PROTOCOL   0x8007066 
#define CONFIGER_INFO_POWER      0x8007068
#define CONFIGER_INFO_RATE       0x800706A
#define CONFIGER_INFO_RADIO      0x800706C
#define CONFIGER_INFO_MODE       0x800706E

#define CACHE_CONFIGER_INFO_ADDR 0x8007166

#define RADIOLINK_BIND	        (0x01 << 0)	
#define RADIOLINK_DATA	        (0x01 << 1)	


extern TaskHandle_t radiolinkTaskHandle;
extern EventGroupHandle_t radioEventHandle;
void Version_Init(uint16_t protocolIndex);
void radiolinkTask(void* param);

#endif

