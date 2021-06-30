#include "rgb.h"
#include "math.h"
#include "tim.h"
EventGroupHandle_t rgbEventHandle;

static uint16_t onDelayCount = 0;
static uint16_t stopDelayCount = 0;
static uint8_t bindStatus = 0;
static uint8_t color[3] = {0};//color[0]为绿色，color[1]为红色，color[2]为蓝色
uint8_t rgbBuff[25];
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
    rgbBuff[0] = 0; 
    
    /*  green data */
    for(i = 0; i < 8; i++)
    {
        rgbBuff[memaddr] = 2;
        rgbBuff[i] = (color[0]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[0]= color[0]<<1;
        memaddr++;
    }
    rgbBuff[7] = 2;
    /*  red data */
    for(i = 8; i < 16; i++)
    {   
        rgbBuff[i] = (color[1]&0x80)?TIMING_ONE:TIMING_ZERO;    
        color[1]= color[1]<<1;
        memaddr++;
    }

    /*  blue data */
    for(i = 16; i < 24; i++)
    {
        rgbBuff[memaddr] = 2;
        rgbBuff[i] = (color[2]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[2]= color[2]<<1;
        memaddr++;
    }
    rgbBuff[15] = 2;
    rgbBuff[24] = 0;
    
//  taskENTER_CRITICAL();	/*进入临界*/
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4,(uint32_t*)rgbBuff,25);
//	taskEXIT_CRITICAL();
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

void RGB_BindTwinkle()
{
    uint8_t i;
    for(i=0; i<8; i++)
    {
        RGB_Set(RED,BRIGHTNESS_MAX);
        osDelay(200);
        RGB_Set(BLACK,BRIGHTNESS_MAX);
        osDelay(200);
    }
    bindStatus = 0;
}

void RGB_SetupTwinkle()
{
    
    if(onDelayCount == 0)
    {
        RGB_Set(RED,BRIGHTNESS_MAX);    
    }
    if(onDelayCount < 20)
    {
        onDelayCount++;
    }
    else
    {
        RGB_Set(BLACK,BRIGHTNESS_MAX);
        if(stopDelayCount < 100)
        {
            stopDelayCount++;
        }
        else
        {
            stopDelayCount = 0;
            onDelayCount = 0;
            osDelay(200);
        }        
    }
   
}

void RGB_Breath_Up(uint8_t colorIndex)
{
    rgbBrightness = BRIGHTNESS_MIN;
    while(rgbBrightness<BRIGHTNESS_MAX)
    {
        rgbBrightness+=20;
        RGB_Set(colorIndex,rgbBrightness);
        osDelay(200);
    }
}
void RGB_Breath_Down(uint8_t colorIndex)
{
    rgbBrightness = BRIGHTNESS_MAX;
    while(rgbBrightness>BRIGHTNESS_MIN)
    {
        rgbBrightness-=20;
        RGB_Set(colorIndex,rgbBrightness);
        osDelay(200);
    }
}
void RGB_Breath(uint8_t colorIndex)
{
    if(rgbBreathStatus == BREATH_DOWN)
    {
        if(rgbBrightness>BRIGHTNESS_MIN)
        {
            rgbBrightness-=20;
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
            rgbBrightness+=20;
        }
        else
        {
            rgbBreathStatus = BREATH_DOWN;
        }
    }
    RGB_Set(colorIndex,rgbBrightness);
    osDelay(200);
}

void rgbTask(void* param)
{
	EventBits_t rgbEvent;
    uint8_t rgbStatus = RGB_INIT;
    uint8_t lastRGBStatus = RGB_INIT;
	while(1)
	{   

		rgbEvent = xEventGroupWaitBits( rgbEventHandle,
		                               POWER_ON_RGB|POWER_OFF_RGB|BIND_RGB|SETUP_RGB|DATA_RGB|SHUTDOWN_RGB|CHRG_AND_JOYSTICK_RGB,
		                               pdTRUE,
	                                   pdFALSE,
		                               0);
		/*POWER RGB*/
        if((rgbEvent & POWER_ON_RGB) == POWER_ON_RGB)
		{         
            RGB_Breath_Up(BLUE);
            rgbEvent &= ~CHRG_AND_JOYSTICK_RGB;
		}
        if((rgbEvent & POWER_OFF_RGB) == POWER_OFF_RGB)
        {				
            RGB_Breath_Down(RED);
            rgbEvent &= ~DATA_RGB;
        }
        
        /*SETUP RGB*/
        if((rgbEvent & BIND_RGB) == BIND_RGB)
        {
            bindStatus = 1;
            RGB_BindTwinkle();
        }        
        if((rgbEvent & SETUP_RGB) == SETUP_RGB)
        {
            RGB_SetupTwinkle();
        }
        if((rgbEvent & DATA_RGB) == DATA_RGB)
        {
            if(bindStatus == 0)
            {
                RGB_Set(BLUE,BRIGHTNESS_MAX);
            }
        }
        if((rgbEvent & SHUTDOWN_RGB) == SHUTDOWN_RGB)
        {
            RGB_Set(YELLOW,BRIGHTNESS_MAX);
        }
        
        if((rgbEvent & CHRG_AND_JOYSTICK_RGB) == CHRG_AND_JOYSTICK_RGB)
        {
            if(HAL_GPIO_ReadPin(CHRG_IN_GPIO_Port,CHRG_IN_Pin) == GPIO_PIN_RESET)
            {
                RGB_Breath(RED);
            }
            else
            {
                RGB_Breath(GREEN);                
            }   
        }
        vTaskDelay(1);       
    }
}

