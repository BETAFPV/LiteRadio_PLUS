#ifndef __BUZZER_H_
#define __BUZZER_H_
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim1;

#define BUZZER_PWM_CH TIM_CHANNEL_3          //蜂鸣器PWM通道

typedef enum
{
	Do = 1,
	Re,
	Mi,
	Fa,
	So,
	La,
	Si
}tone;

void HAL_TIM_SET_COMPARE(uint16_t compare);
void HAL_TIM_SET_AUTORELOAD(uint16_t arr);
void buzzer_start(void);
void buzzer_stop(void);
void Buzzer_On(uint8_t tone);
#endif
