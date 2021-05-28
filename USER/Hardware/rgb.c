#include "rgb.h"
#include "math.h"
#include "tim.h"
uint8_t color[3] = {0};//color[0]为绿色，color[1]为红色，color[2]为蓝色
uint16_t rgb_buffer[25];
uint16_t rgb_breath_brightness;
uint8_t rgb_breath_status;
void color_write(uint8_t color_set,uint8_t brightness)
{
    switch (color_set)
    {
        case RED:
        {
            color[1] = brightness;
            color[0] = 0;
            color[2] = 0;
            break;
        }
        case GREEN:
        {
            color[1] = 0;
            color[0] = brightness;
            color[2] = 0;
            break;            
        }
        case BLUE:
        {
            color[1] = 0;
            color[0] = 0;
            color[2] = brightness;
            break;            
        }
        case YELLOW:
        {
            color[1] = brightness;
            color[0] = brightness;
            color[2] = 0;
            break;            
        }
        case BLACK:
        {
            color[1] = 0;
            color[0] = 0;
            color[2] = 0;
            break;            
        }
        default:
        {
            break;
        }
    }
}

void Rgb_Set(uint8_t color_set,uint8_t brightness)
{
    uint8_t i;
    color_write(color_set,brightness);
    uint16_t memaddr = 0;
    /*  green data */
    for(i = 0; i < 8; i++)
    {
        rgb_buffer[memaddr] = (color[0]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[0]= color[0]<<1;
        memaddr++;
    }

    /*  red data */
    for(i = 0; i < 8; i++)
    {   
        rgb_buffer[memaddr] = (color[1]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[1]= color[1]<<1;
        memaddr++;
    }

    /*  blue data */
    for(i = 0; i < 8; i++)
    {
        rgb_buffer[memaddr] = (color[2]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[2]= color[2]<<1;
        memaddr++;
    }
    rgb_buffer[24] = 0;
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4,(uint32_t*)rgb_buffer,25);
}

void Led_On_Off(uint8_t status)
{
	if(0 == status)
	{
		Rgb_Set(BLACK,255);
	}
	else
	{
        Rgb_Set(RED,255);
	}
}


void Led_Twinkle_Init(uint8_t num)
{
    uint8_t i;
    for(i=0; i<num; i++)
    {
        Rgb_Set(BLUE,255);
        HAL_Delay(400);
        Rgb_Set(BLACK,255);
        HAL_Delay(400);
    }
}


void Led_Twinkle(uint8_t num)
{
    uint8_t i;
    for(i=0; i<num; i++)
    {
        Rgb_Set(RED,255);
        osDelay(200);
        Rgb_Set(BLACK,255);
        osDelay(200);
    }
}

void Rgb_breath_up(uint8_t color_set)
{
    rgb_breath_brightness = 0;
    while(rgb_breath_brightness<255)
    {
        Rgb_Set(color_set,rgb_breath_brightness);
        rgb_breath_brightness++;
        osDelay(5);
    }
}
void Rgb_breath_down(uint8_t color_set)
{
    rgb_breath_brightness = 255;
    while(rgb_breath_brightness>0)
    {
        Rgb_Set(color_set,rgb_breath_brightness);
        rgb_breath_brightness--;
        osDelay(5);
    }
}
void Rgb_breath()
{
    if(rgb_breath_status == BREATH_DOWN)
    {
        if(rgb_breath_brightness>0)
        {
            rgb_breath_brightness--;
        }
        else
        {
            rgb_breath_status = BREATH_UP;
        }
    }
    if(rgb_breath_status == BREATH_UP)
    {
        if(rgb_breath_brightness<255)
        {
            rgb_breath_brightness++;
        }
        else
        {
            rgb_breath_status = BREATH_DOWN;
        }
    }
    Rgb_Set(RED,rgb_breath_brightness);
}

