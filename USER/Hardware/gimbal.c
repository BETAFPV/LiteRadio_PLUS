#include "gimbal.h"
#include "adc.h"
#include "stmflash.h"
#include "key.h"
#include "task.h"
#include "led.h"
#include "buzzer.h"
uint16_t adc_test1,adc_test2,adc_test3,adc_test4;
static uint8_t calibration_mode = 0;//校准模式标志 1：进入校准模式 0：未进入校准模式
static uint8_t HighThrottle_flg = 1;//开机油门标志 1：油门没有打到最底 0：油门打到底
uint8_t calibrate_status = 0;
QueueHandle_t gimbalVal_Queue = NULL;



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
uint16_t Get_GimbalValue(GimbalChannelTypeDef channel)
{
	uint16_t ADTemp = 0 ; 
	uint16_t OutputTemp = 0 ;
	if((channel == RUDDER) || (channel == ELEVATOR) || (channel == AILERON) ||(channel == THROTTLE))
	{
		if(calibration_mode == 0)   //如果当前运行状态不在校准模式，正常执行
		{
			if(Sampling_MaxMinData[channel][MAXDAT] < MAX_VALUE_Min)
			{
				Sampling_MaxMinData[channel][MAXDAT] = MAX_VALUE_Min;
			}
			if(Sampling_MaxMinData[channel][MINDAT] > MIN_VALUE_Max)  
			{
				Sampling_MaxMinData[channel][MINDAT] = MIN_VALUE_Max; 
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
		if(getAdcValue(channel) > Sampling_MaxMinData[channel][MAXDAT]) 
		{
			ADTemp = Sampling_MaxMinData[channel][MAXDAT] ; 
		}
		else if(getAdcValue(channel) < Sampling_MaxMinData[channel][MINDAT])
		{
			ADTemp = Sampling_MaxMinData[channel][MINDAT] ; 
		}
		else								     
		{
			ADTemp = getAdcValue(channel); 
		}
		
		//将三个摇杆AD输出数据映射为发送数据
	  	if(ADTemp >= Sampling_MaxMinData[channel][MIDDAT]) 
		{ 
			OutputTemp = (uint16_t)(CHANNEL_OUTPUT_MID + (((float)ADTemp - Sampling_MaxMinData[channel][MIDDAT]) * ((((float)CHANNEL_OUTPUT_MAX - CHANNEL_OUTPUT_MID ))/(Sampling_MaxMinData[channel][MAXDAT] - Sampling_MaxMinData[channel][MIDDAT])))) ; 
        }
		else
		{ 
			OutputTemp = (uint16_t)(CHANNEL_OUTPUT_MID - (((float)Sampling_MaxMinData[channel][MIDDAT] - ADTemp) * ((((float)CHANNEL_OUTPUT_MID - CHANNEL_OUTPUT_MIN))/(Sampling_MaxMinData[channel][MIDDAT] - Sampling_MaxMinData[channel][MINDAT]))));

        }
		
		//查表换算前，检查数据范围防止越界(查表范围不能超过512)
		if(OutputTemp > (CHANNEL_OUTPUT_MAX)) 
		{
			OutputTemp = (CHANNEL_OUTPUT_MAX) ; 
		}
		
		if(OutputTemp < (CHANNEL_OUTPUT_MIN)) 
		{
			OutputTemp = (CHANNEL_OUTPUT_MIN) ;
		}
        
	}
	else
	{

	}
	return OutputTemp;
}


void HighThrottleCheck(void)
{
    static uint16_t TimeCount = 0;
    uint16_t Throttle_Value = 0;
    if(TimeCount<1000) TimeCount++;
    if(TimeCount==1000)
    {
#ifdef MODE2
        Throttle_Value = Get_GimbalValue(THROTTLE);
#else
        Throttle_Value = 2*CHANNEL_OUTPUT_MID - Get_GimbalValue(THROTTLE);
#endif
    if(Throttle_Value<HIGH_THROTTLE_THRESHOLD)        
        HighThrottle_flg = 0;
    }
}

uint8_t Get_HighThrottle_flg(void)
{
	return HighThrottle_flg;
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

//获取Setup按下次数，最大三次
//按下第一次，返回1：进入校准模式
//按下第二次，返回2：记录摇杆中间值
//按下第三次，返回3，记录摇杆边界值，校准结束
//static uint8_t GetSetupKeyClickTime()
//{
//	static uint8_t ClickTime = 0;
//	static uint8_t SetupDownKeyState = 0;   //记录Setup按键短按按下状态
//	static uint8_t SetupKeybackup = 0;      //记录Setup上一次按键状态
//	uint8_t key_status = getKeyState();
//	if(key_status == SETUP_SHORT_PRESS)     //触发Setup按键短按
//	{
//		SetupDownKeyState = SETUP_SHORT_PRESS;
//	}
//	else
//	{
//		SetupDownKeyState = 0x00;
//	}
//	if(SetupDownKeyState != SetupKeybackup)
//	{
//		key_status = getKeyState();			
//		if(SetupDownKeyState != key_status)
//		{
//			ClickTime++;
//			if(ClickTime > 3)
//			{
//				ClickTime = 0;
//			}
//		}
//		SetupKeybackup = SetupDownKeyState;
//	}
//	return ClickTime;
//}

void GimbalCalibrateProcess(void)
{
	static uint8_t MidValueGetSta = 0;

	//status = GetSetupKeyClickTime();
	switch(calibrate_status)
	{
		case 1:
        {            //获取中值
			calibration_mode = 1;   //进入校准模式
			Led_Twinkle(2);
            xEventGroupSetBits(buzzerEventHandle,SETUP_MID_RING);
			Sampling_MaxMinData[ELEVATOR][MIDDAT]  = getAdcValue(ELEVATOR);
			Sampling_MaxMinData[AILERON][MIDDAT]   = getAdcValue(AILERON);
			Sampling_MaxMinData[RUDDER][MIDDAT]    = getAdcValue(RUDDER);
			Sampling_MaxMinData[THROTTLE][MIDDAT]  = getAdcValue(THROTTLE);
        
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
			Led_Twinkle(2);
            xEventGroupSetBits(buzzerEventHandle,SETUP_MINMAX_RING);  
			if(getAdcValue(THROTTLE) > Sampling_MaxMinData[THROTTLE][MAXDAT])    
				Sampling_MaxMinData[THROTTLE][MAXDAT] = getAdcValue(THROTTLE);
			if(getAdcValue(THROTTLE) < Sampling_MaxMinData[THROTTLE][MINDAT])    
				Sampling_MaxMinData[THROTTLE][MINDAT] = getAdcValue(THROTTLE);

			if(getAdcValue(RUDDER) > Sampling_MaxMinData[RUDDER][MAXDAT])    
				Sampling_MaxMinData[RUDDER][MAXDAT] = getAdcValue(RUDDER);
			if(getAdcValue(RUDDER) < Sampling_MaxMinData[RUDDER][MINDAT])    
				Sampling_MaxMinData[RUDDER][MINDAT] = getAdcValue(RUDDER);

			if(getAdcValue(ELEVATOR) > Sampling_MaxMinData[ELEVATOR][MAXDAT])    
				Sampling_MaxMinData[ELEVATOR][MAXDAT] = getAdcValue(ELEVATOR);
			if(getAdcValue(ELEVATOR) < Sampling_MaxMinData[ELEVATOR][MINDAT])    
				Sampling_MaxMinData[ELEVATOR][MINDAT] = getAdcValue(ELEVATOR);

			if(getAdcValue(AILERON) > Sampling_MaxMinData[AILERON][MAXDAT])    
				Sampling_MaxMinData[AILERON][MAXDAT] = getAdcValue(AILERON);
			if(getAdcValue(AILERON) < Sampling_MaxMinData[AILERON][MINDAT])    
				Sampling_MaxMinData[AILERON][MINDAT] = getAdcValue(AILERON); 
			break; 
        }            
		case 3:
        {
            //保存边界值并退出校准模式
            xEventGroupSetBits(buzzerEventHandle,SETUP_END_RING);
            calibrate_status = 0;
			break;                  
        }  
		default:
        {            
			 MidValueGetSta = 0x00;
			 calibration_mode = 0;
			 break;
        }
	}
}



void gimbalTask(void* param)
{

	EventBits_t R_event = pdPASS;
    static uint16_t gimbal_buff[4] = {0};
	gimbalVal_Queue = xQueueCreate(20,sizeof(gimbal_buff));
	while(1)
	{
		vTaskDelay(9);
        gimbal_buff[THROTTLE] = Get_GimbalValue(THROTTLE);
		gimbal_buff[AILERON] =  Get_GimbalValue(AILERON);
        gimbal_buff[RUDDER] =   Get_GimbalValue(RUDDER);//反的
		gimbal_buff[ELEVATOR] = Get_GimbalValue(ELEVATOR);//反的

		R_event= xEventGroupWaitBits( KeyEventHandle,
		                              SETUP_SHORT_PRESS,
		                              pdTRUE,
	                                  pdTRUE,
		                              0);
		if((R_event & SETUP_SHORT_PRESS) == SETUP_SHORT_PRESS)
		{
			calibrate_status +=1;
		}
        if(calibrate_status > 0 && calibrate_status <= 3)
        {
            GimbalCalibrateProcess();
        }
		xQueueSend(gimbalVal_Queue,gimbal_buff,0);
	}
}

