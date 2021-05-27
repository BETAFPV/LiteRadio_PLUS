#ifndef _GIMBAL_H_
#define _GIMBAL_H_
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

#define FLASH_ADDR 0x08007820

extern QueueHandle_t gimbalVal_Queue;
#define GIMBAL_VAL_QUEUE_SIZE           (4*sizeof(uint16_t))
#define MODE2 //摇杆操控模式选择 --> #define MODE2：美国手 / 注释掉#define MODE2：日本手
//摇杆数据隐射

#define ELEVATOR_MAXVALUE_ADDR 0x08007800
#define ELEVATOR_MIDVALUE_ADDR 0x08007802
#define ELEVATOR_MINVALUE_ADDR 0x08007804

#define AILERON_MAXVALUE_ADDR 0x08007806
#define AILERON_MIDVALUE_ADDR 0x08007808
#define AILERON_MINVALUE_ADDR 0x0800780A

#define RUDDER_MAXVALUE_ADDR 0x0800780C
#define RUDDER_MIDVALUE_ADDR 0x0800780E
#define RUDDER_MINVALUE_ADDR 0x08007810

#define THROTTLE_MAXVALUE_ADDR 0x08007812
#define THROTTLE_MIDVALUE_ADDR 0x08007814
#define THROTTLE_MINVALUE_ADDR 0x08007816

//按电位器采取顺序排列
typedef enum
{
  	RUDDER   = 0 ,       //yaw
	THROTTLE = 1 ,       //throttle
	AILERON  = 2 ,       //roll
	ELEVATOR = 3 ,       //pitch
}GimbalChannelTypeDef;

typedef struct{
    uint8_t THROTTLE:1;       
	uint8_t AILERON:1 ;      
	uint8_t RUDDER:1;       
	uint8_t ELEVATOR:1;  
}GimbalReverseTypeDef;

#define CHANNEL_OUTPUT_MAX 2010
#define CHANNEL_OUTPUT_MID 1500
#define CHANNEL_OUTPUT_MIN 988

#define THR_OUTPUT_MAX 2010
#define THR_OUTPUT_MID 1500
#define THR_OUTPUT_MIN 988
#define HIGH_THROTTLE_THRESHOLD 1050  

#define ADC_INPUT_MAX 4094
#define ADC_INPUT_MID 2047
#define ADC_INPUT_MIN 0

#define MAX_VALUE_Min  3095
#define MIN_VALUE_Max  1000

#define MAXDAT 0 
#define MIDDAT 1
#define MINDAT 2 

#define AD_MIDVALUE_MIN (ADC_INPUT_MID - 350)
#define AD_MIDVALUE_MAX	(ADC_INPUT_MID + 350)


void gimbalTask(void* param);

uint16_t Get_GimbalValue(GimbalChannelTypeDef channel);
void SaveCalibrationValueToFlash(void);
void ReadCalibrationValueForFlash(void);
void HighThrottleCheck(void);
uint8_t Get_HighThrottle_flg(void);
void GimbalCalibrateProcess(void);
uint8_t get_calibration_mode(void);

void get_report_data(uint16_t *data);

#endif
