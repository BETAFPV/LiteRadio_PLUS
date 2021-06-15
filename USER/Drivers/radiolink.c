#include "radiolink.h"
#include "gimbal.h"
#include "switches.h"
#include "frsky_d8.h"
#include "frsky_d16.h"
#include "s_fhss.h"
#include "crsf.h"
#include "cc2500.h"
#include "rgb.h"
#include "mixes.h"
#include "key.h"
#include "tim.h"
#include "delay.h"

TaskHandle_t radiolinkTaskHandle;
EventGroupHandle_t radioEventHandle;
static uint16_t controlData[8];
static uint8_t versionSelectFlg = 3;
static uint32_t delayTime = 0;
static uint32_t delayTimeUs = 0;
void (*RF_Init)(uint8_t protocolIndex);
void (*RF_Bind)(void);
uint16_t (*RF_Process)(uint16_t* controlData);
void Version_Init(uint16_t protocolIndex)
{
    versionSelectFlg = protocolIndex;
}

void radiolinkTask(void* param)
{
    //TickType_t xLastWakeTime;   
    EventBits_t radioEvent;
    switch(versionSelectFlg)
	{
		case 0: RF_Init = FRSKYD16_Init;
                RF_Bind = SetBind;
				RF_Process = ReadFRSKYD16;
                delayTime = D16_INTERVAL;
                HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);        
				break;
		case 1: RF_Init = FRSKYD16_Init;
                RF_Bind = SetBind;
				RF_Process = ReadFRSKYD16;
                delayTime = D16_INTERVAL;
                HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);
				break;
		case 2: RF_Init = initFRSKYD8;
                RF_Bind = D8_SetBind;
				RF_Process = ReadFRSKYD8;
                delayTime = D8_INTERVAL;
                HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);
				break;
        case 3: RF_Init = initSFHSS;
				RF_Process = ReadSFHSS;
                RF_Bind = SFHSS_SetBind;
                delayTime = SFHSS_INTERVAL;
                HAL_GPIO_WritePin(GPIOB,INTERNAL_RF_EN_Pin,GPIO_PIN_SET);
				break;
        case 4: RF_Init = CRSF_Init;
				RF_Process = CRSF_Process;
                RF_Bind = CRSF_SetBind;
                delayTime = CRSF_INTERVAL;
                HAL_GPIO_WritePin(EXTERNAL_RF_EN_GPIO_Port, EXTERNAL_RF_EN_Pin, GPIO_PIN_SET);                
				break;
		default:
				break;
	}
    RF_Init(versionSelectFlg);
    while(1)
    {
     //   xLastWakeTime = xTaskGetTickCount();
        vTaskDelay(delayTime);
        xQueueReceive(mixesValQueue,controlData,0);
        radioEvent= xEventGroupWaitBits( radioEventHandle,
                                         RADIOLINK_BIND,
		                                 pdTRUE,
	                                     pdFALSE,
		                                 0);
		if((radioEvent & RADIOLINK_BIND) == RADIOLINK_BIND)
		{
            RF_Bind();
		}  
        
        RF_Process(controlData);
    }
}

