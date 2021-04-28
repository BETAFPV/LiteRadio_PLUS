#include "gimbal.h"
#include "adc.h"
#include "stmflash.h"
#include "key.h"
#include "task.h"
#include "led.h"
uint16_t adc_test1,adc_test2,adc_test3,adc_test4;
static uint8_t calibration_mode = 0;//校准模式标志 1：进入校准模式 0：未进入校准模式
static uint8_t HighThrottle_flg = 1;//开机油门标志 1：油门没有打到最底 0：油门打到底
uint8_t status = 0;
QueueHandle_t gimbalVal_Queue = NULL;
const uint16_t OutputCode[513] =      //Gimbal ADC value mapping table
{
    0,0,0,0,0,
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,
    61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
    81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
    101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
    121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,
    141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,
    161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,
    181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,
    201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,
    221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,
    241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,
    261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,
    281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,
    301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,
    321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,
    341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,
    361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,
    381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,
    401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,
    421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,
    441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,458,459,460,
    461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,
    481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,
    501,502,503,504,505,506,507,//508,509,510,511,512
};

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
		//限定AD值得采样范围
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
		if(OutputTemp > CHANNEL_OUTPUT_MID) 
		{
			OutputTemp = OutputCode[OutputTemp - CHANNEL_OUTPUT_MID] + CHANNEL_OUTPUT_MID;
		}
		else		            
		{
			OutputTemp = CHANNEL_OUTPUT_MID - OutputCode[CHANNEL_OUTPUT_MID - OutputTemp];
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

//void GimbalCalibrateProcess(void)
//{
//	static uint8_t MidValueGetSta = 0;
//	static uint8_t aveCalibrationValSta = 0;
//	status = GetSetupKeyClickTime();
//	switch(status)
//	{
//		case 1:                     //获取中值
//            Led_Blue(OFF);
//			calibration_mode = 1;   //进入校准模式
//			Led_Twinkle(2);
//			//Buzzer_Bee(Si,2);
//			osDelay(200);
//			//get_report_data(Gimbal_ADC_Value);
//			Sampling_MaxMinData[ELEVATOR][MIDDAT]  = getAdcValue(ELEVATOR);
//			Sampling_MaxMinData[AILERON][MIDDAT]   = getAdcValue(AILERON);
//			Sampling_MaxMinData[RUDDER][MIDDAT]    = getAdcValue(RUDDER);
//			Sampling_MaxMinData[THROTTLE][MIDDAT]  = getAdcValue(THROTTLE);
//			break;
//		case 2:						//保存中值并获取边界值
//			if(MidValueGetSta == 0x00)
//			{
//				//获取中值，方便边界值判断
//				Sampling_MaxMinData[THROTTLE][MAXDAT] = Sampling_MaxMinData[THROTTLE][MIDDAT];
//				Sampling_MaxMinData[THROTTLE][MINDAT] = Sampling_MaxMinData[THROTTLE][MIDDAT];
//				Sampling_MaxMinData[RUDDER][MAXDAT] = Sampling_MaxMinData[RUDDER][MIDDAT];
//				Sampling_MaxMinData[RUDDER][MINDAT] = Sampling_MaxMinData[RUDDER][MIDDAT];
//				Sampling_MaxMinData[ELEVATOR][MAXDAT] = Sampling_MaxMinData[ELEVATOR][MIDDAT];
//				Sampling_MaxMinData[ELEVATOR][MINDAT] = Sampling_MaxMinData[ELEVATOR][MIDDAT];
//				Sampling_MaxMinData[AILERON][MAXDAT] = Sampling_MaxMinData[AILERON][MIDDAT];
//				Sampling_MaxMinData[AILERON][MINDAT] = Sampling_MaxMinData[AILERON][MIDDAT];
//				MidValueGetSta = 0x01;          //设置已经保存标志位，防止重复保存读写Flash 容易损坏flash
//			}
//			Led_Twinkle(2);
//			Buzzer_Bee(Si,3);
//			Delay_ms(300);
//			//ADC_FilterWindow_Updata();
//			get_report_data(Gimbal_ADC_Value);
//			if(Gimbal_ADC_Value[THROTTLE] > Sampling_MaxMinData[THROTTLE][MAXDAT])    
//				Sampling_MaxMinData[THROTTLE][MAXDAT] = Gimbal_ADC_Value[THROTTLE];
//			if(Gimbal_ADC_Value[THROTTLE] < Sampling_MaxMinData[THROTTLE][MINDAT])    
//				Sampling_MaxMinData[THROTTLE][MINDAT] = Gimbal_ADC_Value[THROTTLE];

//			if(Gimbal_ADC_Value[RUDDER] > Sampling_MaxMinData[RUDDER][MAXDAT])    
//				Sampling_MaxMinData[RUDDER][MAXDAT] = Gimbal_ADC_Value[RUDDER];
//			if(Gimbal_ADC_Value[RUDDER] < Sampling_MaxMinData[RUDDER][MINDAT])    
//				Sampling_MaxMinData[RUDDER][MINDAT] = Gimbal_ADC_Value[RUDDER];

//			if(Gimbal_ADC_Value[ELEVATOR] > Sampling_MaxMinData[ELEVATOR][MAXDAT])    
//				Sampling_MaxMinData[ELEVATOR][MAXDAT] = Gimbal_ADC_Value[ELEVATOR];
//			if(Gimbal_ADC_Value[ELEVATOR] < Sampling_MaxMinData[ELEVATOR][MINDAT])    
//				Sampling_MaxMinData[ELEVATOR][MINDAT] = Gimbal_ADC_Value[ELEVATOR];

//			if(Gimbal_ADC_Value[AILERON] > Sampling_MaxMinData[AILERON][MAXDAT])    
//				Sampling_MaxMinData[AILERON][MAXDAT] = Gimbal_ADC_Value[AILERON];
//			if(Gimbal_ADC_Value[AILERON] < Sampling_MaxMinData[AILERON][MINDAT])    
//				Sampling_MaxMinData[AILERON][MINDAT] = Gimbal_ADC_Value[AILERON]; 
//			break;                  
//		case 3:
//			if(aveCalibrationValSta == 0x00)
//			{
//				Led_Twinkle(2);
//				//Buzzer_Bee(Si,2);
//				Buzzer_On(Si);
//				Delay_ms(1500);
//				Buzzer_Off();
//				SaveCalibrationValueToFlash();	
//				aveCalibrationValSta = 0x01;
//				Set_SetUpKeyStatus(0);
//			}
//			break;                  //保存边界值并退出校准模式
//		default: 
//			 MidValueGetSta = 0x00;
//			 aveCalibrationValSta = 0x00;
//			 calibration_mode = 0;
//			 break;
//	}
//}

void gimbalTask(void* param)
{
	BaseType_t xReturn = pdPASS;
	EventBits_t R_event = pdPASS;
    static uint16_t gimbal_buff[4] = {0};
	gimbalVal_Queue = xQueueCreate(20,sizeof(gimbal_buff));
	while(1)
	{
		vTaskDelay(5);
		gimbal_buff[AILERON] =  Get_GimbalValue(AILERON);
		gimbal_buff[ELEVATOR] = Get_GimbalValue(ELEVATOR);
		gimbal_buff[RUDDER] =   Get_GimbalValue(RUDDER);
		gimbal_buff[THROTTLE] = Get_GimbalValue(THROTTLE);
		R_event= xEventGroupWaitBits( KeyEventHandle,
		                              SETUP_SHORT_PRESS,
		                              pdTRUE,
	                                  pdTRUE,
		                              0);
		if((R_event & SETUP_SHORT_PRESS) == SETUP_SHORT_PRESS)
		{
			status +=1;
		}
		xReturn = xQueueSend(gimbalVal_Queue,gimbal_buff,0);
	//	GimbalCalibrateProcess();
	}
}

