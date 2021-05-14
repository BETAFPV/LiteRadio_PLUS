#include "radiolink.h"
#include "gimbal.h"
#include "switches.h"
#include "frsky_d8.h"
#include "frsky_d16.h"
#include "s_fhss.h"
#include "cc2500.h"
#include "led.h"
#include "mixes.h"
#include "key.h"
TaskHandle_t radiolinkTaskHandle;
//uint16_t gimbal_val_buff[4];
//uint16_t switches_val_buff[4];
uint16_t control_data[8];
static uint8_t Version_select_flag = 3;
uint32_t delay_time = 0;
void (*RF_Init)(uint8_t protocol_index);
void (*RF_Bind)(void);
uint16_t (*RF_Process)(uint16_t* control_data);

void radiolinkTask(void* param)
{

//    initFRSKYD16();
    EventBits_t R_event = pdPASS;
    switch(Version_select_flag)
	{
		case 0: RF_Init = initFRSKYD16;
                RF_Bind = SetBind;
				RF_Process = ReadFRSKYD16;
                delay_time = 9;
				break;
		case 1: RF_Init = initFRSKYD16;
                RF_Bind = SetBind;
				RF_Process = ReadFRSKYD16;
                delay_time = 9;
				break;
		case 2: RF_Init = initFRSKYD8;
                RF_Bind = D8_SetBind;
				RF_Process = ReadFRSKYD8;
                delay_time = 9;
				break;
        case 3: RF_Init = initSFHSS;
				RF_Process = ReadSFHSS;
                //Reset_Bind_Flg();
                delay_time = 2;
				break;
        case 4: RF_Init = initFRSKYD16;
				RF_Process = ReadFRSKYD16;
				break;
		default:
				break;
	}
    RF_Init(Version_select_flag);
    while(1)
    {
        osDelay(delay_time);
        xQueueReceive(mixesdataVal_Queue,control_data,0);
        xEventGroupGetBits(KeyEventHandle);

        R_event= xEventGroupWaitBits( KeyEventHandle,
		                              BIND_SHORT_PRESS,
		                              pdTRUE,
	                                  pdTRUE,
		                              0);
		if((R_event & BIND_SHORT_PRESS) == BIND_SHORT_PRESS)
		{
            RF_Bind();
		}

        RF_Process(control_data);
    }
}

