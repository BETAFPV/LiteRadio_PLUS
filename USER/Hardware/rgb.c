#include "rgb.h"
#include "math.h"
#include "tim.h"
#include "gimbal.h"
#include "status.h"
#include "radiolink.h"
#include "stdbool.h"
#include "common.h"
#include "xinput.h"
#include "phoenix.h"
#include "mixes.h"
#include "stmflash.h"
EventGroupHandle_t rgbEventHandle;

static uint8_t bindStatus = 0;
static uint8_t color[3] = {0};//color[0] is green;color[1] is red;color[2] is blue;
uint16_t rgbBuff[25];
uint8_t rgbDelayCount;
uint16_t CHRG_Status_filter = 0;
static uint16_t rgbBrightness;
static uint8_t rgbBreathStatus;
static uint8_t highThrottleFlag;
static uint8_t rgbCountCurr = 0;
static uint64_t rgbNowTick;
static uint64_t rgbStartTick;
static uint64_t rgbStopTick;
static uint64_t rgbStopDelayTick;
static uint8_t rgbStartStatus = 0;
static uint8_t rgbStopStatus = 0;
static uint8_t rgbStopDelayStatus = 0;

uint8_t getHighThrottleFlag(void)
{
    return highThrottleFlag;
}
    
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
    /*  green data */
    for(i = 0; i < 8; i++)
    {
        rgbBuff[i] = (color[0]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[0]= color[0]<<1;
    }
    /*  red data */
    for(i = 8; i < 16; i++)
    {   
        rgbBuff[i] = (color[1]&0x80)?TIMING_ONE:TIMING_ZERO;    
        color[1]= color[1]<<1;
    }

    /*  blue data */
    for(i = 16; i < 24; i++)
    {
        rgbBuff[i] = (color[2]&0x80)?TIMING_ONE:TIMING_ZERO;
        color[2]= color[2]<<1;
    }
    
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4,(uint32_t*)rgbBuff,25);
}


/*use HAL_Delay before FreeRTOS init*/
void RGB_TwinkleForInit(uint8_t num,uint16_t twinkleDelayTime)
{
    uint8_t i;
    
#if defined(LiteRadio_Plus_CC2500) 
    if(num<=4)//内置射频模块，闪红灯
    {
       for(i=0; i<num; i++)
        {
            RGB_Set(RED,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
            RGB_Set(BLACK,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
        } 
    }
    else//使用外置射频模块，闪紫灯
    {
        for(i=0; i<3; i++)
        {
            RGB_Set(PURPLE,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
            RGB_Set(BLACK,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
        } 
    }
    
#elif defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)
	if(num<=1)//内置射频模块，闪红灯
    {
       for(i=0; i<num; i++)
        {
            RGB_Set(RED,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
            RGB_Set(BLACK,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
        } 
    }
    else//使用外置射频模块，闪紫灯
    {
        for(i=0; i<3; i++)
        {
            RGB_Set(PURPLE,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
            RGB_Set(BLACK,BRIGHTNESS_MAX);
            HAL_Delay(twinkleDelayTime);
        } 
    }
#endif
}

void RGB_LowElectricityTwinkle()
{
    RGB_Set(RED,BRIGHTNESS_MAX);
    osDelay(200);
    RGB_Set(BLACK,BRIGHTNESS_MAX);
    osDelay(200);
    RGB_Set(RED,BRIGHTNESS_MAX);
    osDelay(200);
    RGB_Set(BLACK,BRIGHTNESS_MAX);
    osDelay(200);
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

void RGB_SetupTwinkle(uint8_t rgbNum)
{
    rgbNowTick = HAL_GetTick();    
    if(rgbStartStatus||rgbStopStatus||rgbStopDelayStatus)
    {
        if(rgbStartStatus)
        {
            if(rgbNowTick - rgbStartTick > RGB_START_INTERVAL)
            {
                rgbStartStatus = 0;
                rgbStopStatus = 1;
                RGB_Set(BLACK,BRIGHTNESS_MAX);
                rgbStopTick = HAL_GetTick(); 
            }
        }
        if(rgbStopStatus)
        {
            if(rgbNowTick - rgbStopTick > RGB_STOP_INTERVAL)
            {
                rgbStopStatus = 0;
                if(rgbCountCurr <( rgbNum -1))
                {
                    rgbStartStatus = 1;
                    RGB_Set(RED,BRIGHTNESS_MAX);  
                    rgbStartTick = HAL_GetTick();                       
                    rgbCountCurr++;
                }
                else
                {
                    rgbCountCurr = 0;
                    rgbStopDelayStatus = 1;
                    rgbStopDelayTick = HAL_GetTick();
                }
            }    
        }
        if(rgbStopDelayStatus)
        {
            if(rgbNowTick - rgbStopDelayTick > RGB_STOP_DELAY_INTERVAL)
            {   
                rgbStopDelayStatus = 0;
                rgbStartStatus = 1;
                RGB_Set(RED,BRIGHTNESS_MAX);    
                rgbStartTick = HAL_GetTick();              
            }                
        
        }
    }
    else
    {
        rgbStartStatus = 1;
        rgbStartTick = HAL_GetTick();
        RGB_Set(RED,BRIGHTNESS_MAX); 
        rgbCountCurr = 0;
    }    
    
    
}

void RGB_Breath_Up(uint8_t colorIndex, uint32_t time)
{
    if(BRIGHTNESS_MAX){
        time = time / BRIGHTNESS_MAX;
    }else{
        time = 10;
    }
    rgbBrightness = BRIGHTNESS_MIN;
    while(rgbBrightness<BRIGHTNESS_MAX)
    {
        rgbBrightness++;
        RGB_Set(colorIndex,rgbBrightness);
        osDelay(time);
    }
}
void RGB_Breath_Down(uint8_t colorIndex, uint32_t time)
{
    if(BRIGHTNESS_MAX){
        time = time / BRIGHTNESS_MAX;
    }else{
        time = 10;
    }
    rgbBrightness = BRIGHTNESS_MAX;
    while(rgbBrightness > BRIGHTNESS_MIN)
    {
        rgbBrightness--;
        RGB_Set(colorIndex,rgbBrightness);
        osDelay(time);
    }
}
void RGB_Breath(uint8_t colorIndex, uint32_t time)
{
    if(BRIGHTNESS_MAX){
        time = time / BRIGHTNESS_MAX / 2;
    }else{
        time = 10;
    }
    if(rgbBreathStatus == BREATH_DOWN)
    {
        if(rgbBrightness > BRIGHTNESS_MIN)
        {
            rgbBrightness--;
        }
        else
        {
            rgbBreathStatus = BREATH_UP;
        }
        osDelay(time);
    }
    if(rgbBreathStatus == BREATH_UP)
    {
        if(rgbBrightness < BRIGHTNESS_MAX)
        {
            rgbBrightness++;
        }
        else
        {
            rgbBreathStatus = BREATH_DOWN;
        }
        osDelay(time);
    }
    RGB_Set(colorIndex,rgbBrightness);

}



static bool Tim1IsOpen = false;
void rgbTask(void* param)
{
    EventBits_t rgbEvent;
    uint8_t xboxPromptOnlyOnce = 0;
    highThrottleFlag = 1;
    
    uint8_t  rgbFlashChanged = 0;               // flash内容有改变此标志位置1
    uint32_t rgbFlashSaveTick = HAL_GetTick();  // 每当要保存到flash的变量发生改变，记录此变量改变时刻
    uint16_t rgbChargingBreathEnable;           // 充电状态下呼吸灯的开关，短按setup切换
    STMFLASH_Read(ChargingRgbSwitch_ADDR, &rgbChargingBreathEnable, 1);
    if(rgbChargingBreathEnable > 1){
        // 检查参数范围
        rgbChargingBreathEnable = 1;
        rgbFlashChanged = 1;
    }
    
    while(1)
    {   
        vTaskDelay(1);       
        rgbEvent = xEventGroupWaitBits( rgbEventHandle,
                                        POWER_ON_RGB | POWER_OFF_RGB | BIND_RGB |
                                        LOW_ELECTRICITY_RGB | SETUP_RGB | SETUP_RGB1 |
                                        DATA_RGB | SHUTDOWN_RGB | CHRG_AND_JOYSTICK_RGB |
                                        RGB_KEY_SETUP_SHORT,
                                        pdTRUE,
                                        pdFALSE,
                                        0);
        /* 切换呼吸灯动画开关 */
        if((rgbEvent & RGB_KEY_SETUP_SHORT) == RGB_KEY_SETUP_SHORT){
            if(rgbChargingBreathEnable == 1){
                rgbChargingBreathEnable = 0;
                RGB_Set(BLACK, BRIGHTNESS_MAX);// 熄灭
            }else{
                rgbChargingBreathEnable = 1;
            }
            rgbFlashChanged = 1;
            // 记录最新的切换时间，1s后没有发生改变就会保存至flash
            rgbFlashSaveTick = HAL_GetTick();
        }
        if(rgbFlashChanged && HAL_GetTick()-rgbFlashSaveTick >= 1000){
            rgbFlashChanged = 0;
            // 保存到flash
            STMFLASH_Write(ChargingRgbSwitch_ADDR, &rgbChargingBreathEnable, 1);
        }
        
        /*POWER RGB*/
        if((rgbEvent & POWER_ON_RGB) == POWER_ON_RGB)
        {
            RGB_Breath_Up(RED, 2500);
            rgbEvent &= ~CHRG_AND_JOYSTICK_RGB;
        }
        if((rgbEvent & POWER_OFF_RGB) == POWER_OFF_RGB)
        {
            RGB_Breath_Down(BLUE, 2500);
            rgbEvent &= ~DATA_RGB;
        }
        
        /*Enter xbox mode*/
        if(xboxPromptOnlyOnce == 0)
        {
            if(isXboxMode == 1)
            {
                xboxPromptOnlyOnce = 1;
                for(uint8_t i=0; i<3; i++)
                {
                    RGB_Set(WHITE,BRIGHTNESS_MAX);
                    osDelay(200);
                    RGB_Set(BLACK,BRIGHTNESS_MAX);
                    osDelay(200);
                }
            }
        }
        
        /*LOW ELECTRICITY RGB*/
        if((rgbEvent & LOW_ELECTRICITY_RGB) == LOW_ELECTRICITY_RGB)
        {
            RGB_LowElectricityTwinkle();
        }   
        
        /*SETUP RGB*/
        if((rgbEvent & BIND_RGB) == BIND_RGB)
        {
            bindStatus = 1;
            RGB_BindTwinkle();
        }        
        
        if((rgbEvent & SETUP_RGB1) == SETUP_RGB1)
        {
            RGB_SetupTwinkle(3);
        }
        else if((rgbEvent & SETUP_RGB) == SETUP_RGB)
        {
            RGB_SetupTwinkle(2);
        }
        if((rgbEvent & DATA_RGB) == DATA_RGB)
        {
            if(bindStatus == 0)
            {
                if(highThrottleFlag)
                {
                    highThrottleFlag = Check_HighThrottle();
                    RGB_Set(RED,BRIGHTNESS_MAX);
					HAL_TIM_Base_Stop_IT(&htim1);
                }
                else
                {
                    RGB_Set(BLUE,BRIGHTNESS_MAX); 
#if defined(LiteRadio_Plus_CC2500)  
		if(Get_Protocol_Select() <= CC2500_FHSS)//CC2500使用内置射频模块才打开定时器1
        {
            if(Tim1IsOpen == false)
            {
                Tim1IsOpen = true;
                HAL_TIM_Base_Start_IT(&htim1);
            }
            
        }
					
#elif defined(LiteRadio_Plus_SX1280)		
			HAL_TIM_Base_Start_IT(&htim1);
#endif                      
                    
                }
            }
        }
        if((rgbEvent & SHUTDOWN_RGB) == SHUTDOWN_RGB)
        {
            RGB_Set(YELLOW,BRIGHTNESS_MAX);
        }
        
        if((rgbEvent & CHRG_AND_JOYSTICK_RGB) == CHRG_AND_JOYSTICK_RGB)
        {
            if(configFlag)
            {
                RGB_Set(PURPLE,BRIGHTNESS_MAX);
            }
            else 
            {
                if(HAL_GPIO_ReadPin(CHRG_IN_GPIO_Port,CHRG_IN_Pin) == GPIO_PIN_RESET)
                {
                    CHRG_Status_filter = 0;       //充电状态标志清0
                    if(rgbChargingBreathEnable){
                        RGB_Breath(RED, 2500);
                    }
                }
                else
                {              
                    CHRG_Status_filter ++; 
                    if(CHRG_Status_filter > 500)   //因为电池在充满的边界时，充满状态引脚可能会不稳定，等到连续n次都是高电平，那么才认定为真正的充满。
                    {
                        if(rgbChargingBreathEnable){
                            RGB_Breath(GREEN, 2500);
                        }
                    }
                }
            }
        }
    }
}

