#include "gimbal.h"
#include "adc.h"

uint16_t adc_test1,adc_test2,adc_test3,adc_test4;

void gimbalTask(void* param)
{
	while(1)
	{
		adc_test1 = getAdcValue(0);
		adc_test2 = getAdcValue(1);
		adc_test3 = getAdcValue(2);
		adc_test4 = getAdcValue(3);
	}
}
