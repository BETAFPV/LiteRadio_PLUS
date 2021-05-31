#ifndef __BUZZER_H_
#define __BUZZER_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim3;
extern EventGroupHandle_t buzzerEventHandle;

#define BUZZER_PWM_CH TIM_CHANNEL_1         //蜂鸣器PWM通道

#define BIND_SHORT_PRESS_RING	        (0x01 << 0)	
#define BIND_LONG_PRESS_RING	        (0x01 << 1)	
#define SETUP_MID_RING	                (0x01 << 2)	
#define SETUP_MINMAX_RING	            (0x01 << 3)
#define SETUP_END_RING                  (0x01 << 4)
#define POWER_ON_RING	                (0x01 << 5)	
#define POWER_OFF_RING	                (0x01 << 6)	


typedef enum
{
    power_longpress_ring = 1,
    setup_ring_two,
    
}buzzer_status;

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
void Buzzer_Start(void);
void Buzzer_Stop(void);
void Buzzer_On(uint8_t tone);
void Buzzer_bee_time(uint8_t tone,uint32_t buzzer_time);
void Buzzer_Bee_Up(void);
void Buzzer_Bee_Down(void);
void Buzzer_Bee(uint8_t tone,uint8_t buzzer_count);
void buzzerTask(void* param);  
#endif
