#include "radiolink.h"
#include "gimbal.h"
#include "switches.h"
#include "crsf.h"
#include "rgb.h"
#include "mixes.h"
#include "key.h"
#include "tim.h"
#include "delay.h"
#include "status.h"
#if defined(LiteRadio_Plus_CC2500) 
#include "frsky_d8.h"
#include "frsky_d16.h"
#include "s_fhss.h"
#include "cc2500.h"
uint16_t channelData[16];
#elif defined(LiteRadio_Plus_SX1280)
#include "sx1280.h"
#include "sx1280hal.h"
#include "common.h"
#include "stmflash.h"
uint16_t channelData[16];
#elif defined(LiteRadio_Plus_SX1276)
#include "sx1276.h"
#include "common.h"
uint16_t channelData[16];
#endif

TaskHandle_t radiolinkTaskHandle;
EventGroupHandle_t radioEventHandle;
static uint16_t rfcontrolData[8];
static uint8_t versionSelectFlg;
uint32_t radiolinkDelayTime ;
uint64_t radiolinkTick;
void (*RF_Init)(uint8_t protocolIndex);
void (*RF_Bind)(void);
uint16_t (*RF_Process)(uint16_t* controlData);
uint16_t (*RcDateSendToBuff)(uint16_t* controlData);

void Version_Init(uint16_t protocolIndex)
{
    versionSelectFlg = protocolIndex;
}
uint8_t Get_Protocol_Select(void)
{
    return versionSelectFlg;
}


void radiolinkTask(void* param)
{
    EventBits_t radioEvent;
    radiolinkDelayTime = Get_ProtocolDelayTime();
    switch(versionSelectFlg)
    {
#if defined(LiteRadio_Plus_CC2500)        
        case 0: RF_Init = FRSKYD16_Init;
                RF_Bind = SetBind;
                RcDateSendToBuff = CC2500_Process;
				RF_Process = ReadFRSKYD16;
				TIM1->ARR = 9000;		
                break;
        case 1: RF_Init = FRSKYD16_Init;
                RF_Bind = SetBind;
                RcDateSendToBuff = CC2500_Process;
				RF_Process = ReadFRSKYD16;
				TIM1->ARR = 9000;
                break;
        case 2: RF_Init = initFRSKYD8;
                RF_Bind = D8_SetBind;
                RcDateSendToBuff = CC2500_Process;
				RF_Process = ReadFRSKYD8;
				TIM1->ARR = 9000;
                break;
        case 3: RF_Init = initSFHSS;
                RF_Bind = SFHSS_SetBind;
				RcDateSendToBuff = CC2500_Process;
				RF_Process = ReadSFHSS;
				TIM1->ARR = 9000;
                break;
  
        case 4: RF_Init = CRSF_Init;
                RcDateSendToBuff = CRSF_Process;
                RF_Bind = CRSF_SetBind;               
                break;       
#elif defined(LiteRadio_Plus_SX1280)
        case 0: RF_Init = ExpressLRS_Init;
                RF_Process = SX1280_Process;
                RF_Bind = EnterBindingMode;               
                break;    
		case 1: RF_Init = CRSF_Init;
				RF_Process = CRSF_Process;
				RF_Bind = CRSF_SetBind;     
				break;
#elif defined(LiteRadio_Plus_SX1276)                
        case 0: RF_Init = ExpressLRS_Init;
                RF_Process = SX1276_Process;
                RF_Bind = EnterBindingMode;               
                break;                             
        case 1: RF_Init = CRSF_Init;
                RF_Process = CRSF_Process;
                RF_Bind = CRSF_SetBind;     
                break;
#endif
        default:
                break;
    }
    RF_Init(versionSelectFlg);
		
    while(1)
    {
        vTaskDelay(radiolinkDelayTime);
        xQueueReceive(mixesValQueue,rfcontrolData,0);
        radioEvent= xEventGroupWaitBits( radioEventHandle,
                                         RADIOLINK_BIND,
                                         pdTRUE,
                                         pdFALSE,
                                         0);
        if((radioEvent & RADIOLINK_BIND) == RADIOLINK_BIND)
        {
            RF_Bind();
        }  
#if defined(LiteRadio_Plus_CC2500)  
		RcDateSendToBuff(rfcontrolData);
#elif defined(LiteRadio_Plus_SX1280)		
		RF_Process(rfcontrolData);
#endif
        radiolinkTick = HAL_GetTick();
    }
}


