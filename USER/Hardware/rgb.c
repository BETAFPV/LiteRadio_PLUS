#include "rgb.h"
#include "math.h"
#include "tim.h"
#include "gimbal.h"
EventGroupHandle_t rgbEventHandle;

static uint16_t onDelayCount = 0;
static uint16_t stopDelayCount = 0;
static uint8_t bindStatus = 0;
static uint8_t color[3] = {0};//color[0] is green;color[1] is red;color[2] is blue;
uint8_t rgbBuff[25];
uint8_t rgbDelayCount;
static uint16_t rgbBrightness;
static uint8_t rgbBreathStatus;
static uint8_t highThrottleFlag;
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
        case BLACK:
        {
            color[1] = 0;
            color[0] = 0;
            color[2] = 0;
            break;            
        }
        case WHITE:
        {
            color[1] = brightness;
            color[0] = brightness;
            color[2] = brightness;
            break;            
        }
        case PURPLE:
        {
            color[1] = 128;
            color[0] = 0;
            color[2] = 128;
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
        rgbBuff[memaddr] = 2;
        rgbBuff[i] = (color[0]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[0]= color[0]<<1;
        memaddr++;
    }
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
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4,(uint32_t*)rgbBuff,25);
}


/*use HAL_Delay before FreeRTOS init*/
void RGB_TwinkleForInit(uint8_t num,uint16_t twinkleDelayTime)
{
    uint8_t i;
    for(i=0; i<num; i++)
    {
        RGB_Set(RED,BRIGHTNESS_MAX);
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
    if(onDelayCount < 500)
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
            osDelay(500);
        }        
    }
   
}

void RGB_Breath_Up(uint8_t colorIndex)
{
    rgbBrightness = BRIGHTNESS_MIN;
    while(rgbBrightness<BRIGHTNESS_MAX)
    {
        rgbBrightness++;
        RGB_Set(colorIndex,rgbBrightness);
        osDelay(10);
    }
}
void RGB_Breath_Down(uint8_t colorIndex)
{
    rgbBrightness = BRIGHTNESS_MAX;
    while(rgbBrightness>BRIGHTNESS_MIN)
    {
        rgbBrightness--;
        RGB_Set(colorIndex,rgbBrightness);
        osDelay(10);
    }
}
void RGB_Breath(uint8_t colorIndex)
{
    if(rgbBreathStatus == BREATH_DOWN)
    {
        if(rgbBrightness>BRIGHTNESS_MIN)
        {
            rgbBrightness--;
        }
        else
        {
            rgbBreathStatus = BREATH_UP;
        }
        osDelay(10);
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
        osDelay(5);
    }
    RGB_Set(colorIndex,rgbBrightness);

}

void rgbTask(void* param)
{
    EventBits_t rgbEvent;
    
    highThrottleFlag = 1;
    
    while(1)
    {   
        vTaskDelay(1);       
        rgbEvent = xEventGroupWaitBits( rgbEventHandle,
                                        POWER_ON_RGB|POWER_OFF_RGB|BIND_RGB|SETUP_RGB|DATA_RGB|SHUTDOWN_RGB|CHRG_AND_JOYSTICK_RGB,
                                        pdTRUE,
                                        pdFALSE,
                                        0);
        /*POWER RGB*/
        if((rgbEvent & POWER_ON_RGB) == POWER_ON_RGB)
        {         
            RGB_Breath_Up(RED);
            rgbEvent &= ~CHRG_AND_JOYSTICK_RGB;
        }
        if((rgbEvent & POWER_OFF_RGB) == POWER_OFF_RGB)
        {				
            RGB_Breath_Down(BLUE);
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
                if(highThrottleFlag)
                {
                    highThrottleFlag = Check_HighThrottle();
                    RGB_Set(RED,BRIGHTNESS_MAX);
                }
                else
                {
                   RGB_Set(BLUE,BRIGHTNESS_MAX); 
                }
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
    }
}

