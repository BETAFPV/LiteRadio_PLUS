#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"

extern TaskHandle_t radiolinkTaskHandle;

void radiolinkTask(void* param);

#endif

