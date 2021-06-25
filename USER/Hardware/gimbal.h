#ifndef _GIMBAL_H_
#define _GIMBAL_H_
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

#define GIMBAL_CALIBRATE_IN	    (0x01 << 0)	
#define GIMBAL_CALIBRATE_END	(0x02 << 0)	
#define FLASH_ADDR 0x08007820

extern QueueHandle_t gimbalValQueue;
extern EventGroupHandle_t gimbalEventHandle;
#define GIMBAL_VAL_QUEUE_SIZE           (4*sizeof(uint16_t))

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
	ELEVATOR = 2 ,      //pitch
	AILERON  = 3 ,      //roll
  	RUDDER   = 1 ,      //yaw
	THROTTLE = 0 ,      //throttle
}gimbalChannelTypeDef;


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

#define MAX_VALUE_MIN  3095
#define MIN_VALUE_MAX  1000

#define MAXDAT 0 
#define MIDDAT 1
#define MINDAT 2 

#define AD_MIDVALUE_MIN (ADC_INPUT_MID - 350)
#define AD_MIDVALUE_MAX	(ADC_INPUT_MID + 350)

void Gimbal_Init(void);
void gimbalTask(void* param);

uint16_t Get_GimbalValue(gimbalChannelTypeDef channel);
void SaveCalibrationValueToFlash(void);
void ReadCalibrationValueForFlash(void);
void Check_HighThrottle(void);
uint8_t Get_highThrottleFlg(void);
void GimbalCalibrateProcess(void);
uint8_t get_calibrationMode(void);

void get_reportData(uint16_t *data);

#endif
