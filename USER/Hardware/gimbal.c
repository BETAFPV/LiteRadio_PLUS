#include "gimbal.h"
#include "adc.h"
#include "stmflash.h"
#include "key.h"
#include "task.h"
#include "rgb.h"
#include "buzzer.h"
#include "radiolink.h"
#include "status.h"
#include "mixes.h"
uint16_t adc_test1,adc_test2,adc_test3,adc_test4;
static uint8_t calibrationMode = 0;//校准模式标志 1：进入校准模式 0：未进入校准模式
static uint8_t calibrationStatus = 0;
QueueHandle_t gimbalValQueue = NULL;
EventGroupHandle_t gimbalEventHandle = NULL;
static uint32_t gimbalDelayTime;

uint16_t Sampling_MaxMinData[4][3] = 
{
	{ADC_INPUT_MAX , ADC_INPUT_MID , ADC_INPUT_MIN},
	{ADC_INPUT_MAX , ADC_INPUT_MID , ADC_INPUT_MIN},
	{ADC_INPUT_MAX , ADC_INPUT_MID , ADC_INPUT_MIN},
	{ADC_INPUT_MAX , ADC_INPUT_MID , ADC_INPUT_MIN},
};

/******************************************************
function : return Gimbal channle Value
input:  channel  -> RUDDER\THROTTLE\ELEVATOR\AILERON 
return : Gimbal Channel Value
******************************************************/
uint16_t Get_GimbalValue(gimbalChannelTypeDef channel)
{
	uint16_t ADTemp = 0 ; 
    uint16_t outputTemp = 0 ;
	if((channel == RUDDER) || (channel == ELEVATOR) || (channel == AILERON) ||(channel == THROTTLE))
	{
		if(calibrationMode == 0)   //如果当前运行状态不在校准模式，正常执行
		{
			if(Sampling_MaxMinData[channel][MAXDAT] < MAX_VALUE_MIN)
			{
				Sampling_MaxMinData[channel][MAXDAT] = MAX_VALUE_MIN;
			}
			if(Sampling_MaxMinData[channel][MINDAT] > MIN_VALUE_MAX)  
			{
				Sampling_MaxMinData[channel][MINDAT] = MIN_VALUE_MAX; 
			}
			if(Sampling_MaxMinData[channel][MIDDAT] > AD_MIDVALUE_MAX)  
			{
				Sampling_MaxMinData[channel][MIDDAT] = AD_MIDVALUE_MAX; 
			}
			else if(Sampling_MaxMinData[channel][MIDDAT] < AD_MIDVALUE_MIN) 
			{
				Sampling_MaxMinData[channel][MIDDAT] = AD_MIDVALUE_MIN; 
			}
		}
		//限定AD值的采样范围
		if(Get_AdcValue(channel) > Sampling_MaxMinData[channel][MAXDAT]) 
		{
			ADTemp = Sampling_MaxMinData[channel][MAXDAT] ; 
		}
		else if(Get_AdcValue(channel) < Sampling_MaxMinData[channel][MINDAT])
		{
			ADTemp = Sampling_MaxMinData[channel][MINDAT] ; 
		}
		else								     
		{
			ADTemp = Get_AdcValue(channel); 
		}
		
		//将三个摇杆AD输出数据映射为发送数据
	  	if(ADTemp >= Sampling_MaxMinData[channel][MIDDAT]) 
		{ 
			outputTemp = (uint16_t)(CHANNEL_OUTPUT_MID + (((float)ADTemp - Sampling_MaxMinData[channel][MIDDAT]) * ((((float)CHANNEL_OUTPUT_MAX - CHANNEL_OUTPUT_MID ))/(Sampling_MaxMinData[channel][MAXDAT] - Sampling_MaxMinData[channel][MIDDAT])))) ; 
        }
		else
		{ 
			outputTemp = (uint16_t)(CHANNEL_OUTPUT_MID - (((float)Sampling_MaxMinData[channel][MIDDAT] - ADTemp) * ((((float)CHANNEL_OUTPUT_MID - CHANNEL_OUTPUT_MIN))/(Sampling_MaxMinData[channel][MIDDAT] - Sampling_MaxMinData[channel][MINDAT]))));

        }
		
		//查表换算前，检查数据范围防止越界(查表范围不能超过512)
		if(outputTemp > (CHANNEL_OUTPUT_MAX)) 
		{
			outputTemp = (CHANNEL_OUTPUT_MAX) ; 
		}
		
		if(outputTemp < (CHANNEL_OUTPUT_MIN)) 
		{
			outputTemp = (CHANNEL_OUTPUT_MIN) ;
		}
	}
	else
	{

	}
	return outputTemp;
}


uint8_t Check_HighThrottle(void)
{
    uint16_t throttleValue = 0;
    if(controlMode == 0)
    {
        throttleValue = Get_GimbalValue(THROTTLE);
    }
    else
    {
        throttleValue = 2*CHANNEL_OUTPUT_MID - Get_GimbalValue(ELEVATOR);
    }
    if(throttleValue < HIGH_THROTTLE_THRESHOLD) 
    {        
        return 0;
    }
    else
    {
        return 1;
    }
}


void SaveCalibrationValueToFlash(void)
{
	STMFLASH_Write(ELEVATOR_MAXVALUE_ADDR,&Sampling_MaxMinData[ELEVATOR][MAXDAT],1);
	STMFLASH_Write(ELEVATOR_MIDVALUE_ADDR,&Sampling_MaxMinData[ELEVATOR][MIDDAT],1);
	STMFLASH_Write(ELEVATOR_MINVALUE_ADDR,&Sampling_MaxMinData[ELEVATOR][MINDAT],1);
	
	STMFLASH_Write(AILERON_MAXVALUE_ADDR,&Sampling_MaxMinData[AILERON][MAXDAT],1);
	STMFLASH_Write(AILERON_MIDVALUE_ADDR,&Sampling_MaxMinData[AILERON][MIDDAT],1);
	STMFLASH_Write(AILERON_MINVALUE_ADDR,&Sampling_MaxMinData[AILERON][MINDAT],1);
	
	STMFLASH_Write(RUDDER_MAXVALUE_ADDR,&Sampling_MaxMinData[RUDDER][MAXDAT],1);
	STMFLASH_Write(RUDDER_MIDVALUE_ADDR,&Sampling_MaxMinData[RUDDER][MIDDAT],1);
	STMFLASH_Write(RUDDER_MINVALUE_ADDR,&Sampling_MaxMinData[RUDDER][MINDAT],1);
	
	STMFLASH_Write(THROTTLE_MAXVALUE_ADDR,&Sampling_MaxMinData[THROTTLE][MAXDAT],1);
	STMFLASH_Write(THROTTLE_MIDVALUE_ADDR,&Sampling_MaxMinData[THROTTLE][MIDDAT],1);
	STMFLASH_Write(THROTTLE_MINVALUE_ADDR,&Sampling_MaxMinData[THROTTLE][MINDAT],1);
	
}

void ReadCalibrationValueForFlash(void)
{
    STMFLASH_Read(ELEVATOR_MAXVALUE_ADDR,&Sampling_MaxMinData[ELEVATOR][MAXDAT],1);
    STMFLASH_Read(ELEVATOR_MIDVALUE_ADDR,&Sampling_MaxMinData[ELEVATOR][MIDDAT],1);
    STMFLASH_Read(ELEVATOR_MINVALUE_ADDR,&Sampling_MaxMinData[ELEVATOR][MINDAT],1);
    
    STMFLASH_Read(AILERON_MAXVALUE_ADDR,&Sampling_MaxMinData[AILERON][MAXDAT],1);
    STMFLASH_Read(AILERON_MIDVALUE_ADDR,&Sampling_MaxMinData[AILERON][MIDDAT],1);
    STMFLASH_Read(AILERON_MINVALUE_ADDR,&Sampling_MaxMinData[AILERON][MINDAT],1);
    
    STMFLASH_Read(RUDDER_MAXVALUE_ADDR,&Sampling_MaxMinData[RUDDER][MAXDAT],1);
    STMFLASH_Read(RUDDER_MIDVALUE_ADDR,&Sampling_MaxMinData[RUDDER][MIDDAT],1);
    STMFLASH_Read(RUDDER_MINVALUE_ADDR,&Sampling_MaxMinData[RUDDER][MINDAT],1);
    
    STMFLASH_Read(THROTTLE_MAXVALUE_ADDR,&Sampling_MaxMinData[THROTTLE][MAXDAT],1);
    STMFLASH_Read(THROTTLE_MIDVALUE_ADDR,&Sampling_MaxMinData[THROTTLE][MIDDAT],1);
    STMFLASH_Read(THROTTLE_MINVALUE_ADDR,&Sampling_MaxMinData[THROTTLE][MINDAT],1);

}

void GimbalCalibrateProcess(void)
{
	static uint8_t MidValueGetSta = 0;

	//status = GetSetupKeyClickTime();
	switch(calibrationStatus)
	{
		case 1:
        {            //获取中值
			calibrationMode = 1;   //进入校准模式
			//RGB_Twinkle(2);
            xEventGroupSetBits(buzzerEventHandle,SETUP_MID_RING);
			Sampling_MaxMinData[ELEVATOR][MIDDAT]  = Get_AdcValue(ELEVATOR);
			Sampling_MaxMinData[AILERON][MIDDAT]   = Get_AdcValue(AILERON);
			Sampling_MaxMinData[RUDDER][MIDDAT]    = Get_AdcValue(RUDDER);
			Sampling_MaxMinData[THROTTLE][MIDDAT]  = Get_AdcValue(THROTTLE);
        
			break;
        }
		case 2:
        {            
            //保存中值并获取边界值
			if(MidValueGetSta == 0x00)
			{
				//获取中值，方便边界值判断
				Sampling_MaxMinData[THROTTLE][MAXDAT] = Sampling_MaxMinData[THROTTLE][MIDDAT];
				Sampling_MaxMinData[THROTTLE][MINDAT] = Sampling_MaxMinData[THROTTLE][MIDDAT];
				Sampling_MaxMinData[RUDDER][MAXDAT] = Sampling_MaxMinData[RUDDER][MIDDAT];
				Sampling_MaxMinData[RUDDER][MINDAT] = Sampling_MaxMinData[RUDDER][MIDDAT];
				Sampling_MaxMinData[ELEVATOR][MAXDAT] = Sampling_MaxMinData[ELEVATOR][MIDDAT];
				Sampling_MaxMinData[ELEVATOR][MINDAT] = Sampling_MaxMinData[ELEVATOR][MIDDAT];
				Sampling_MaxMinData[AILERON][MAXDAT] = Sampling_MaxMinData[AILERON][MIDDAT];
				Sampling_MaxMinData[AILERON][MINDAT] = Sampling_MaxMinData[AILERON][MIDDAT];
				MidValueGetSta = 0x01;          //设置已经保存标志位，防止重复保存读写Flash 容易损坏flash
			}
            xEventGroupSetBits(buzzerEventHandle,SETUP_MINMAX_RING);  
			if(Get_AdcValue(THROTTLE) > Sampling_MaxMinData[THROTTLE][MAXDAT])    
				Sampling_MaxMinData[THROTTLE][MAXDAT] = Get_AdcValue(THROTTLE);
			if(Get_AdcValue(THROTTLE) < Sampling_MaxMinData[THROTTLE][MINDAT])    
				Sampling_MaxMinData[THROTTLE][MINDAT] = Get_AdcValue(THROTTLE);

			if(Get_AdcValue(RUDDER) > Sampling_MaxMinData[RUDDER][MAXDAT])    
				Sampling_MaxMinData[RUDDER][MAXDAT] = Get_AdcValue(RUDDER);
			if(Get_AdcValue(RUDDER) < Sampling_MaxMinData[RUDDER][MINDAT])    
				Sampling_MaxMinData[RUDDER][MINDAT] = Get_AdcValue(RUDDER);

			if(Get_AdcValue(ELEVATOR) > Sampling_MaxMinData[ELEVATOR][MAXDAT])    
				Sampling_MaxMinData[ELEVATOR][MAXDAT] = Get_AdcValue(ELEVATOR);
			if(Get_AdcValue(ELEVATOR) < Sampling_MaxMinData[ELEVATOR][MINDAT])    
				Sampling_MaxMinData[ELEVATOR][MINDAT] = Get_AdcValue(ELEVATOR);

			if(Get_AdcValue(AILERON) > Sampling_MaxMinData[AILERON][MAXDAT])    
				Sampling_MaxMinData[AILERON][MAXDAT] = Get_AdcValue(AILERON);
			if(Get_AdcValue(AILERON) < Sampling_MaxMinData[AILERON][MINDAT])    
				Sampling_MaxMinData[AILERON][MINDAT] = Get_AdcValue(AILERON); 
			break; 
        }            
		case 3:
        {
            //保存边界值并退出校准模式
            xEventGroupSetBits(buzzerEventHandle,SETUP_END_RING);
            xEventGroupSetBits(gimbalEventHandle,GIMBAL_CALIBRATE_END);
            SaveCalibrationValueToFlash();	
            calibrationStatus = 0;
			break;                  
        }  
		default:
        {            
			 MidValueGetSta = 0x00;
			 calibrationMode = 0;
			 break;
        }
	}
}

void Gimbal_Init()
{
    Adc_Start();
    ReadCalibrationValueForFlash();
}

void gimbalTask(void* param)
{
    EventBits_t gimbalEvent;
    static uint16_t gimbalBuff[4] = {0};
    
	gimbalDelayTime = Get_ProtocolDelayTime();
    
    gimbalValQueue = xQueueCreate(20,sizeof(gimbalBuff));
    while(1)
    {
        vTaskDelay(gimbalDelayTime);

        gimbalBuff[THROTTLE] = Get_GimbalValue(THROTTLE);
        gimbalBuff[RUDDER] = 2*CHANNEL_OUTPUT_MID - Get_GimbalValue(RUDDER);
        gimbalBuff[AILERON] = Get_GimbalValue(AILERON);
        gimbalBuff[ELEVATOR] = 2*CHANNEL_OUTPUT_MID - Get_GimbalValue(ELEVATOR);

        gimbalEvent= xEventGroupWaitBits( gimbalEventHandle,
		                                  GIMBAL_CALIBRATE_IN,
		                                  pdTRUE,
	                                      pdFALSE,
		                                  0);
        if((gimbalEvent & GIMBAL_CALIBRATE_IN) == GIMBAL_CALIBRATE_IN)
        {
            calibrationStatus += 1;
        }
        if(calibrationStatus > 0 && calibrationStatus <= 3)
        {
            GimbalCalibrateProcess();
        }
        xQueueSend(gimbalValQueue,gimbalBuff,0);
    }
}
