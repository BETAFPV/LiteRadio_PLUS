#include "joystick.h"
#include "gimbal.h"
#include "switches.h"
#include "usbd_customhid.h"
#include "mixes.h"
#include "stmflash.h"
#include "radiolink.h"
#include "function.h"
#include "status.h"
static uint32_t joystickDelayTime;
TaskHandle_t joystickTaskHandle;
void joystickTask(void *param) 
{
    uint16_t reportData[8];
    uint16_t mixValBuff[8];
		joystickDelayTime = Get_ProtocolDelayTime();
    while(1)
	{
	    vTaskDelay(joystickDelayTime);
	    uxTaskGetStackHighWaterMarkdebug_1 = uxTaskGetStackHighWaterMark(NULL);
        xQueueReceive(mixesValQueue,mixValBuff,0);
     
        reportData[0] = map(mixValBuff[0],1000,2000,0,2047);
        reportData[1] = map(mixValBuff[1],1000,2000,0,2047);
        reportData[2] = map(mixValBuff[2],1000,2000,0,2047);
        reportData[3] = map(mixValBuff[3],1000,2000,0,2047);
        reportData[4] = map(mixValBuff[4],1000,2000,0,2047);
        reportData[5] = map(mixValBuff[5],1000,2000,0,2047);
        reportData[6] = map(mixValBuff[6],1000,2000,0,2047);
        reportData[7] = map(mixValBuff[7],1000,2000,0,2047);

        if(configerRequest != 0x01)
        {    
            if(configerRequest == 0x02)
            {
                reportData[0] = INTERNAL_CONFIGER_INFO_ID;
                STMFLASH_Read(CONFIGER_INFO_ADDR,&reportData[1],3);
                reportData[7] = 0xFFFE;
            }
            else if(0 < configerRequest && configerRequest <= 0x0A)
            {
                reportData[0] = CHANNEILS_INFO_ID;
                reportData[1] = configerRequest- 0x03;
                STMFLASH_Read(MIX_CHANNEL_1_INFO_ADDR + 8*(configerRequest - 0x03),&reportData[2],4);
                reportData[7] = 0xFFFE;
            }
                reportData[7] = 0xFFFE;
            }
        }
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &reportData, 8*sizeof(uint16_t));
    }
}

