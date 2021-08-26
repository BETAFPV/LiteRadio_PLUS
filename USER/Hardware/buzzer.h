#ifndef __BUZZER_H_
#define __BUZZER_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim3;
extern EventGroupHandle_t buzzerEventHandle;

#define BUZZER_PWM_CH TIM_CHANNEL_1         //蜂鸣器PWM通道

#define SETUP_MID_RING	                (0x01 << 1)		
#define SETUP_MINMAX_RING	            (0x01 << 2)
#define SETUP_END_RING                  (0x01 << 3)
#define POWER_ON_RING	                (0x01 << 4)	
#define POWER_OFF_RING	                (0x01 << 5)	
#define LOW_ELECTRICITY_RING            (0x01 << 6)
#define RISS_WARNING_RING	            (0x01 << 7)	


#define BUZZER_START_INTERVAL       150
#define BUZZER_STOP_INTERVAL        150
#define BUZZER_STOP_DELAY_INTERVAL  500

typedef enum
{
    BUZZER_NORMAL = 0,
    SETUP_RING_TWO,
    SETUP_RING_THREE,
    SETUP_RING_BEE,
}buzzerStatus_e;

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
void Buzzer_BeeStay(uint8_t tone,uint32_t buzzer_time);
void Buzzer_BeeUp(void);
void Buzzer_BeeDown(void);
void Buzzer_LowElectricity(void);
void Buzzer_RSSIwarning(void);
void Buzzer_BeeNumInit(uint8_t buzzerNumInit);
void Buzzer_BeeNum(uint8_t tone,uint8_t buzzerNum);
void buzzerTask(void* param);  
#endif
