#include "adc.h"
extern ADC_HandleTypeDef hadc1;
uint16_t adc_value[ADC_CHAL_NUM * ADC_SAMPLE_NUM];

void Adc_Start()
{
	if(HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&adc_value[0],ADC_CHAL_NUM * ADC_SAMPLE_NUM*sizeof(uint16_t)) != HAL_OK) 
	{
		Error_Handler();
	}
}

uint16_t getAdcValue(uint8_t axis)
{
	uint32_t sum=0;
	for(uint8_t i=0;i<ADC_SAMPLE_NUM;i++)
	{
		sum += adc_value[ADC_CHAL_NUM*i+axis];
	}
	return sum/ADC_SAMPLE_NUM;
}
