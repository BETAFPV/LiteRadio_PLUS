#include "delay.h"
#include "tim.h"

void Delay_us(uint32_t nus)
{
    uint64_t now = getSysTimeUs();
    while ((getSysTimeUs() - now) < nus);
}

uint64_t getSysTimeUs(void)
{
    uint64_t ms;
    uint64_t value;
    ms = xTaskGetTickCount();
    value = ms * 1000 + (SysTick->LOAD - SysTick->VAL) * 1000 / SysTick->LOAD;
    return value;
}
