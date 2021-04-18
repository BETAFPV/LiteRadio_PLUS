#include "led.h"
void Led_Red(GPIO_PinState status)
{
    HAL_GPIO_WritePin(LED_RED_PORT,LED_RED_PIN,status);
}

void Led_Blue(GPIO_PinState status)
{
    HAL_GPIO_WritePin(LED_BLUE_PORT,LED_BLUE_PIN,status);
}

void Led_On_Off(uint8_t status)
{
	if(0 == status)
	{
		Led_Red(OFF);
	}
	else
	{
		Led_Red(ON);
	}
}


void Led_Twinkle(uint8_t num)
{
    uint8_t i;
    for(i=0; i<num; i++)
    {
        Led_Red(ON);
        //Delay_ms(100);
        Led_Red(OFF);
        //Delay_ms(100);
    }
}


