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
    uint16_t writeFlag = 0x00;
    uint16_t reportData[8];
    uint16_t mixValBuff[8];
		joystickDelayTime = Get_ProtocolDelayTime();
    while(1)
		{
		    vTaskDelay(joystickDelayTime);
		    uxTaskGetStackHighWaterMarkdebug_1 = uxTaskGetStackHighWaterMark(NULL);
        xQueueReceive(mixesValQueue,mixValBuff,0);

        // reportData[0] = mixValBuff[0];
        // reportData[1] = mixValBuff[1];
        // reportData[2] = mixValBuff[2];
        // reportData[3] = mixValBuff[3];
        // reportData[4] = mixValBuff[4];
        // reportData[5] = mixValBuff[5];
        // reportData[6] = mixValBuff[6];
        // reportData[7] = mixValBuff[7];
				reportData[0] = map(mixValBuff[0],988,2020,0,2047);
				reportData[1] = map(mixValBuff[1],988,2020,0,2047);
				reportData[2] = map(mixValBuff[2],988,2020,0,2047);
				reportData[3] = map(mixValBuff[3],988,2020,0,2047);
				reportData[4] = map(mixValBuff[4],988,2020,0,2047);
				reportData[5] = map(mixValBuff[5],988,2020,0,2047);
				reportData[6] = map(mixValBuff[6],988,2020,0,2047);
				reportData[7] = map(mixValBuff[7],988,2020,0,2047);
        STMFLASH_Read(CONFIGER_INFO_FLAG,&writeFlag,1);

        if(writeFlag != 0x01)
		{    
            if(writeFlag == 0x02)
            {
                reportData[0] = CONFIGER_INFO_ID;
                STMFLASH_Read(FLASH_ADDR,&reportData[1],1);
                STMFLASH_Read(CONFIGER_INFO_POWER,&reportData[2],5);
            }
            else if(0 < writeFlag && writeFlag <= 0x0A)
            {
                reportData[0] = CHANNEILS_INFO_ID;
                reportData[1] = writeFlag- 0x02;
                STMFLASH_Read(MIX_CHANNEL_1_INFO_ADDR + 8*(writeFlag - 0x03),&reportData[2],4);
            }
        }
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &reportData, 8*sizeof(uint16_t));
	}
}

