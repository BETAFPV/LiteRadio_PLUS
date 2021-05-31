#include "rgb.h"
#include "math.h"
#include "tim.h"
static uint8_t color[3] = {0};//color[0]为绿色，color[1]为红色，color[2]为蓝色
static uint16_t rgbBuff[25];
static uint16_t rgbBrightness;
static uint8_t rgbBreathStatus;
void Color_Set(uint8_t colorIndex,uint8_t brightness)
{
    switch (colorIndex)
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

void RGB_Set(uint8_t colorIndex,uint8_t brightness)
{
    uint8_t i;
    Color_Set(colorIndex,brightness);
    uint16_t memaddr = 0;
    /*  green data */
    for(i = 0; i < 8; i++)
    {
        rgbBuff[memaddr] = (color[0]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[0]= color[0]<<1;
        memaddr++;
    }

    /*  red data */
    for(i = 0; i < 8; i++)
    {   
        rgbBuff[memaddr] = (color[1]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[1]= color[1]<<1;
        memaddr++;
    }

    /*  blue data */
    for(i = 0; i < 8; i++)
    {
        rgbBuff[memaddr] = (color[2]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[2]= color[2]<<1;
        memaddr++;
    }
    rgbBuff[24] = 0;
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4,(uint32_t*)rgbBuff,25);
}

void RGB_Toggle(uint8_t status)
{
	if(status == 0)
	{
		RGB_Set(BLACK,BRIGHTNESS_MAX);
	}
	else
	{
        RGB_Set(RED,BRIGHTNESS_MAX);
	}
}

//任务未开始调度时需要使用HAL_Delay
void RGB_TwinkleForInit(uint8_t num,uint16_t twinkleDelayTime)
{
    uint8_t i;
    for(i=0; i<num; i++)
    {
        RGB_Set(BLUE,BRIGHTNESS_MAX);
        HAL_Delay(twinkleDelayTime);
        RGB_Set(BLACK,BRIGHTNESS_MAX);
        HAL_Delay(twinkleDelayTime);
    }
}

void RGB_Twinkle(uint8_t num,uint16_t twinkleDelayTime)
{
    uint8_t i;
    for(i=0; i<num; i++)
    {
        RGB_Set(RED,BRIGHTNESS_MAX);
        osDelay(twinkleDelayTime);
        RGB_Set(BLACK,BRIGHTNESS_MAX);
        osDelay(twinkleDelayTime);
    }
}

void RGB_Breath_Up(uint8_t colorIndex)
{
    rgbBrightness = 0;
    while(rgbBrightness<BRIGHTNESS_MAX)
    {
        RGB_Set(colorIndex,rgbBrightness);
        rgbBrightness++;
        osDelay(5);
    }
}
void RGB_Breath_Down(uint8_t colorIndex)
{
    rgbBrightness = BRIGHTNESS_MAX;
    while(rgbBrightness>0)
    {
        RGB_Set(colorIndex,rgbBrightness);
        rgbBrightness--;
        osDelay(5);
    }
}
void RGB_Breath()
{
    if(rgbBreathStatus == BREATH_DOWN)
    {
        if(rgbBrightness>0)
        {
            rgbBrightness--;
        }
        else
        {
            rgbBreathStatus = BREATH_UP;
        }
    }
    if(rgbBreathStatus == BREATH_UP)
    {
        if(rgbBrightness<BRIGHTNESS_MAX)
        {
            rgbBrightness++;
        }
        else
        {
            rgbBreathStatus = BREATH_DOWN;
        }
    }
    RGB_Set(RED,rgbBrightness);
}

