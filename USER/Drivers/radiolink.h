#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"


#define RADIOLINK_BIND	        (0x01 << 0)	
#define RADIOLINK_DATA	        (0x01 << 1)	

extern TaskHandle_t radiolinkTaskHandle;
extern EventGroupHandle_t radioEventHandle;
void radiolinkTask(void* param);

#endif

