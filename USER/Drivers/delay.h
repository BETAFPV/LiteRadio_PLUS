#ifndef _DELAY_H_
#define _DELAY_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

void Delay_US(uint32_t nus);
uint64_t Get_SysTimeUs(void);
#endif

