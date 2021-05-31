#include "buzzer.h"
#include "rgb.h"
uint8_t buzzerCountCurr = 0;

EventGroupHandle_t buzzerEventHandle = NULL;

void Buzzer_BeeStay(uint8_t tone,uint32_t stayTime)
{
    Buzzer_Start();
    Buzzer_On(tone);
    osDelay(stayTime);
    Buzzer_Stop();
}

void Buzzer_BeeUp()
{
    Buzzer_Start();
    Buzzer_On(Do);
    osDelay(400);
    Buzzer_On(Re);
    osDelay(400);
    Buzzer_On(Mi);
    osDelay(400);
    Buzzer_Stop();
}

void Buzzer_BeeDown()
{
    Buzzer_Start();
    Buzzer_On(Mi);
    osDelay(400);
    Buzzer_On(Re);
    osDelay(400);
    Buzzer_On(Do);
    osDelay(400);
    Buzzer_Stop();
}

void Buzzer_BeeNum(uint8_t tone,uint8_t buzzerNum)
{
    while(buzzerCountCurr < buzzerNum)
    {
        Buzzer_Start();
        Buzzer_On(tone);
        osDelay(100);
        Buzzer_Stop();
        osDelay(100);
        buzzerCountCurr++;
    }
    osDelay(500);
    buzzerCountCurr = 0;
}
     
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

void Buzzer_Start()
{
	HAL_TIM_PWM_Start(&htim3,BUZZER_PWM_CH);
}

void Buzzer_Stop()
{
	HAL_TIM_PWM_Stop(&htim3,BUZZER_PWM_CH);
}

void HAL_TIM_SET_COMPARE(uint16_t compare)
{	
	htim3.Instance->CCR1 = compare;
}

void HAL_TIM_SET_AUTORELOAD(uint16_t arr)
{
	htim3.Instance->ARR = arr;
	htim3.Init.Period = arr;
}

void buzzerTask(void* param)
{
	EventBits_t buzzerEvent;
	while(1)
	{
		buzzerEvent= xEventGroupWaitBits( buzzerEventHandle,
		                              POWER_ON_RING|POWER_OFF_RING|SETUP_MID_RING|SETUP_MINMAX_RING|SETUP_END_RING,
		                              pdTRUE,
	                                  pdFALSE,
		                              0);
		if((buzzerEvent & POWER_ON_RING) == POWER_ON_RING)
		{         
            Buzzer_BeeUp();
		}
        if((buzzerEvent & POWER_OFF_RING) == POWER_OFF_RING)
        {				
            Buzzer_BeeDown();
        } 
        if((buzzerEvent & SETUP_MID_RING) == SETUP_MID_RING)
        {
            Buzzer_BeeNum(Do,2);
        }
        if((buzzerEvent & SETUP_MINMAX_RING) == SETUP_MINMAX_RING)
        {
            Buzzer_BeeNum(Do,3);
        }
        if((buzzerEvent & SETUP_END_RING) == SETUP_END_RING)
        {
            Buzzer_BeeStay(Do,1000);
        }
        vTaskDelay(5);
	}
}
