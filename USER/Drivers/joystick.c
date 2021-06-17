#include "joystick.h"
#include "gimbal.h"
#include "switches.h"
#include "usbd_customhid.h"
#include "mixes.h"
#include "stmflash.h"
#include "radiolink.h"
TaskHandle_t joystickTaskHandle;

void joystickTask(void *param) 
{
    uint8_t writeCount = 0;
    uint16_t writeFlag = 0x00;
    uint16_t reportData[8];
    uint16_t mixValBuff[8];
  //  uint16_t gimbalValBuff[4];
   // uint16_t switchValBuff[4];
	while(1)
	{
		vTaskDelay(5);
        
       // xQueueReceive(gimbalValQueue,gimbalValBuff,0);
		//xQueueReceive(switchesValQueue,switchValBuff,0);
            xQueueReceive(mixesValQueue,mixValBuff,0);
        //RUDDER   = 0 ,       //yaw
        //THROTTLE = 1 ,       //throttle
        //AILERON  = 2 ,       //roll
        //ELEVATOR = 3 ,       //pitch
        
//        //x轴
//		reportData[3] = gimbalValBuff[RUDDER];
//        //y轴
//		reportData[4] = gimbalValBuff[THROTTLE];
//        //z轴
//        reportData[5] = gimbalValBuff[ELEVATOR];

//		//x旋转		
//		reportData[0] = gimbalValBuff[AILERON];
//        //y旋转
//		reportData[1] = switchValBuff[0];
//		//z旋转
//        reportData[2] = switchValBuff[1];
//        
//        //滑块1
//		reportData[6] = switchValBuff[2];
//        //滑块2
//		reportData[7] = switchValBuff[3];

        //x轴
		reportData[3] = mixValBuff[RUDDER];
        //y轴
		reportData[4] = mixValBuff[THROTTLE];
        //z轴
        reportData[5] = mixValBuff[ELEVATOR];

		//x旋转		
		reportData[0] = mixValBuff[AILERON];
        //y旋转
		reportData[1] = mixValBuff[4];
		//z旋转
        reportData[2] = mixValBuff[5];
        
        //滑块1
		reportData[6] = mixValBuff[6];
        //滑块2
		reportData[7] = mixValBuff[7];

        STMFLASH_Read(CONFIGER_INFO_FLAG,&writeFlag,1);

        if(writeFlag == 0x02)
		{    
            reportData[0] = CONFIGER_INFO_ID;
            STMFLASH_Read(CONFIGER_INFO_POWER,&reportData[2],3);
            reportData[5] = 0xffff;
        }
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &reportData, 8*sizeof(uint16_t));
	}
}

