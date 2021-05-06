#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#define ADC_SAMPLE_NUM	10
#define ADC_CHAL_NUM    5

void Adc_Start(void);
uint16_t getAdcValue(uint8_t axis);
#endif
