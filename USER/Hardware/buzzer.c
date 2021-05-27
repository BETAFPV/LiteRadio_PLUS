#include "buzzer.h"
#include "rgb.h"
uint8_t buzzer_curr_count = 0;
uint32_t buzzer_lastmillis = 0;
uint32_t buzzer_millis = 0;
//uint8_t buzzer_status = 0;//为1时处于发声时间，为0时处于静止时间

uint8_t buzzer_intervalstatus = 0;//为1时处于间隔时间，为0时处于间隔时间
uint32_t buzzer_intervalmillis = 0;

EventGroupHandle_t buzzerEventHandle = NULL;

void buzzerTask(void* param)
{
	EventBits_t R_event;
	while(1)
	{
		R_event= xEventGroupWaitBits( buzzerEventHandle,
		                              POWER_ON_RING|POWER_OFF_RING|SETUP_MID_RING|SETUP_MINMAX_RING|SETUP_END_RING,
		                              pdTRUE,
	                                  pdFALSE,
		                              0);
		if((R_event & POWER_ON_RING) == POWER_ON_RING)
		{         
            Buzzer_bee_up();
		}
        if((R_event & POWER_OFF_RING) == POWER_OFF_RING)
        {				
            Buzzer_bee_down();
        } 
        if((R_event & SETUP_MID_RING) == SETUP_MID_RING)
        {
            Buzzer_Bee(Do,2);
        }
        if((R_event & SETUP_MINMAX_RING) == SETUP_MINMAX_RING)
        {
            Buzzer_Bee(Do,3);
        }
        if((R_event & SETUP_END_RING) == SETUP_END_RING)
        {
            Buzzer_bee_time(Do,1000);
        }
        vTaskDelay(5);
	}
}

void Buzzer_bee_time(uint8_t tone,uint32_t buzzer_time)
{
    buzzer_start();
    Buzzer_On(tone);
    osDelay(buzzer_time);
    buzzer_stop();
}

void Buzzer_bee_up()
{
    buzzer_start();
    Buzzer_On(Do);
    osDelay(400);
    Buzzer_On(Re);
    osDelay(400);
    Buzzer_On(Mi);
    osDelay(400);
    buzzer_stop();
}

void Buzzer_bee_down()
{
    buzzer_start();
    Buzzer_On(Mi);
    osDelay(400);
    Buzzer_On(Re);
    osDelay(400);
    Buzzer_On(Do);
    osDelay(400);
    buzzer_stop();
}

void Buzzer_Bee(uint8_t tone,uint8_t buzzer_count)
{
    while(buzzer_curr_count < buzzer_count)
    {
        buzzer_start();
        Buzzer_On(tone);
        osDelay(100);
        buzzer_stop();
        osDelay(100);
        buzzer_curr_count++;
     
    }
    osDelay(500);
    buzzer_curr_count = 0;
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

void buzzer_start()
{
	HAL_TIM_PWM_Start(&htim3,BUZZER_PWM_CH);
}

void buzzer_stop()
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
