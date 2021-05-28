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
TaskHandle_t radiolinkTaskHandle;
EventGroupHandle_t radioEventHandle;
uint16_t control_data[8];
uint16_t send_Buf[24] = {0};
static uint8_t Version_select_flag = 2;
uint32_t delay_time = 0;
void (*RF_Init)(uint8_t protocol_index);
void (*RF_Bind)(void);
uint16_t (*RF_Process)(uint16_t* control_data);

void Version_Init(uint16_t protocol_Index)
{
    Version_select_flag = protocol_Index;
}

void radiolinkTask(void* param)
{
    
    EventBits_t radioEvent;
    switch(Version_select_flag)
	{
		case 0: RF_Init = FRSKYD16_Init;
                RF_Bind = SetBind;
				RF_Process = ReadFRSKYD16;
                delay_time = D16_INTERVAL;
				break;
		case 1: RF_Init = FRSKYD16_Init;
                RF_Bind = SetBind;
				RF_Process = ReadFRSKYD16;
                delay_time = D16_INTERVAL;
				break;
		case 2: RF_Init = initFRSKYD8;
                RF_Bind = D8_SetBind;
				RF_Process = ReadFRSKYD8;
                delay_time = D8_INTERVAL;
				break;
        case 3: RF_Init = initSFHSS;
				RF_Process = ReadSFHSS;
                RF_Bind = SFHSS_SetBind;
                delay_time = SFHSS_INTERVAL;
				break;
        case 4: RF_Init = CRSF_Init;
				RF_Process = CRSF_Process;
                RF_Bind = CRSF_SetBind;
                delay_time = CRSF_INTERVAL;
				break;
		default:
				break;
	}
    RF_Init(Version_select_flag);
    while(1)
    {
        vTaskDelay(delay_time);
        xQueueReceive(mixesdataVal_Queue,control_data,0);
        radioEvent= xEventGroupWaitBits( radioEventHandle,
		                              RADIOLINK_BIND,
		                              pdTRUE,
	                                  pdTRUE,
		                              0);
		if((radioEvent & RADIOLINK_BIND) == RADIOLINK_BIND)
		{
            RF_Bind();
		}

        RF_Process(control_data);
    }
}

