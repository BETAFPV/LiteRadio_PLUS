#include "joystick.h"
#include "gimbal.h"
#include "switches.h"
#include "usbd_customhid.h"

TaskHandle_t joystickTaskHandle;

void joystickTask(void *param) 
{
    uint16_t report_data[8];
    uint16_t gimbal_val_buff[4];
    uint16_t switches_val_buff[4];
    BaseType_t xReturn = pdPASS;
	while(1)
	{
        
		vTaskDelay(10);
        xReturn = xQueueReceive(gimbalVal_Queue,gimbal_val_buff,0);
		xReturn = xQueueReceive(switchesVal_Queue,switches_val_buff,0);
        
        //RUDDER   = 0 ,       //yaw
        //THROTTLE = 1 ,       //throttle
        //AILERON  = 2 ,       //roll
        //ELEVATOR = 3 ,       //pitch
        
        //x轴
		report_data[3] = gimbal_val_buff[RUDDER];
        //y轴
		report_data[4] = gimbal_val_buff[THROTTLE];
        //z轴
        report_data[5] = gimbal_val_buff[ELEVATOR];

		//x旋转		
		report_data[0] = gimbal_val_buff[AILERON];
        //y旋转
		report_data[1] = switches_val_buff[0];
		//z旋转
        report_data[2] = switches_val_buff[1];
        
        //滑块1
		report_data[6] = switches_val_buff[2];
        //滑块2
		report_data[7] = switches_val_buff[3];

        
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &report_data, 8*sizeof(uint16_t));
	}
}
