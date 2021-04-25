#include "buzzer.h"

void Buzzer_On(uint8_t tone)
{
	switch(tone)
	{
		case Do:
			HAL_TIM_SET_AUTORELOAD(916);
			HAL_TIM_SET_COMPARE(458);
			break;
		case Re:
			HAL_TIM_SET_AUTORELOAD(816);
			HAL_TIM_SET_COMPARE(408);
			break;
		case Mi:
			HAL_TIM_SET_AUTORELOAD(727);
			HAL_TIM_SET_COMPARE(363);
			break;
		case Fa:
			HAL_TIM_SET_AUTORELOAD(698);
			HAL_TIM_SET_COMPARE(349);
			break;
		case So:
			HAL_TIM_SET_AUTORELOAD(611);
			HAL_TIM_SET_COMPARE(458);
			break;
		case La:
			HAL_TIM_SET_AUTORELOAD(544);
			HAL_TIM_SET_COMPARE(272);
			break;
		case Si:
			HAL_TIM_SET_AUTORELOAD(485);
			HAL_TIM_SET_COMPARE(242);
			break;
	}
}

void buzzer_start()
{
	HAL_TIM_PWM_Start(&htim1,BUZZER_PWM_CH);
}

void buzzer_stop()
{
	HAL_TIM_PWM_Stop(&htim1,BUZZER_PWM_CH);
}

void HAL_TIM_SET_COMPARE(uint16_t compare)
{	
	htim1.Instance->CCR3 = compare;
}

void HAL_TIM_SET_AUTORELOAD(uint16_t arr)
{
	htim1.Instance->ARR = arr;
	htim1.Init.Period = arr;
}
