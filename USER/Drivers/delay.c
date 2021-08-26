#include "delay.h"
#include "tim.h"

void Delay_US(uint32_t nus)
{
    uint64_t now = Get_SysTimeUs();
    while ((Get_SysTimeUs() - now) < nus);
}

uint64_t Get_SysTimeUs(void)
{
    uint64_t ms;
    uint64_t value;
    ms = xTaskGetTickCount();
    value = ms * 1000 + (SysTick->LOAD - SysTick->VAL) * 1000 / SysTick->LOAD;
    return value;
}
