#include "delay.h"
#include "tim.h"

void Delay_us(uint32_t nus)
{
    uint16_t wait = nus;

    __HAL_TIM_SetCounter(&htim1,0);
    
    HAL_TIM_Base_Start(&htim1);
    
    while(__HAL_TIM_GET_COUNTER(&htim1) < wait) //查询计数器的计数值判断
    {
    }

    HAL_TIM_Base_Stop(&htim1);//频繁开启时，可以注释掉这句
}