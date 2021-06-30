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

	while(1)
	{
            xQueueReceive(mixesValQueue,mixValBuff,0);
        //RUDDER   = 0 ,       //yaw
        //THROTTLE = 1 ,       //throttle
        //AILERON  = 2 ,       //roll
        //ELEVATOR = 3 ,       //pitch

//        //x轴
//		reportData[3] = mixValBuff[RUDDER];
//        //y轴
//		reportData[4] = mixValBuff[THROTTLE];
//        //z轴
//        reportData[5] = mixValBuff[ELEVATOR];

//		//x旋转		
//		reportData[0] = mixValBuff[AILERON];
//        //y旋转
//		reportData[1] = mixValBuff[4];
//		//z旋转
//        reportData[2] = mixValBuff[5];
//        
//        //滑块1
//		reportData[6] = mixValBuff[6];
//        //滑块2
//		reportData[7] = mixValBuff[7];

        reportData[0] = mixValBuff[AILERON];
        reportData[1] = mixValBuff[ELEVATOR];
        reportData[2] = mixValBuff[THROTTLE];
        reportData[3] = mixValBuff[RUDDER];
        reportData[4] = mixValBuff[4];
        reportData[5] = mixValBuff[5];
        reportData[6] = mixValBuff[6];
        reportData[7] = mixValBuff[7];
        

		    vTaskDelay(joystickDelayTime);
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

