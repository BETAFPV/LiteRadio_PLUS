#include "joystick.h"
#include "gimbal.h"
#include "switches.h"
#include "usbd_hid.h"
#include "mixes.h"
#include "stmflash.h"
#include "radiolink.h"
#include "function.h"
#include "status.h"
#include "crsf.h"
#include "common.h"
#include "usbd_cdc_if.h"

static uint32_t joystickDelayTime;
TaskHandle_t joystickTaskHandle;
/*累加和校验算法*/

extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t device;

void joystickTask(void *param) 
{
    uint16_t hidReportData[8];
    uint16_t mixValBuff[8];

    joystickDelayTime = Get_ProtocolDelayTime();
    while(1)
    {
        vTaskDelay(joystickDelayTime);
        xQueueReceive(mixesValQueue,mixValBuff,0);
     
        hidReportData[0] = map(mixValBuff[0],988,2012,0,2047);
        hidReportData[1] = map(mixValBuff[1],988,2012,0,2047);
        hidReportData[2] = map(mixValBuff[2],988,2012,0,2047);
        hidReportData[3] = map(mixValBuff[3],988,2012,0,2047);
        hidReportData[4] = map(mixValBuff[4],988,2012,0,2047);
        hidReportData[5] = map(mixValBuff[5],988,2012,0,2047);
        hidReportData[6] = map(mixValBuff[6],988,2012,0,2047);
        hidReportData[7] = map(mixValBuff[7],988,2012,0,2047);

        //HID send data
        if(device==0)
        {
            USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));
        }
        
        //CDC send data
        //CDC_Transmit_FS((uint8_t*) &hidReportData, 8*sizeof(uint16_t));
    }
}

