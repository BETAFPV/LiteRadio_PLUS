#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"

#define D16_INTERVAL 9
#define D8_INTERVAL 9
#define SFHSS_INTERVAL 2
#define CRSF_INTERVAL 7


#define RADIOLINK_BIND	        (0x01 << 0)	
#define RADIOLINK_DATA	        (0x01 << 1)	

extern TaskHandle_t radiolinkTaskHandle;
extern EventGroupHandle_t radioEventHandle;
void Version_Init(uint16_t protocol_Index);
void radiolinkTask(void* param);

#endif

