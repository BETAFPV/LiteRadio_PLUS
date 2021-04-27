#include "process.h"
#include "gimbal.h"
#include "switches.h"
//uint16_t gimbal_val_buff[4];
//uint16_t switches_val_buff[4];

void radiolinkDataProcessTask(void *param) 
{
//	BaseType_t xReturn = pdPASS;
	while(1)
	{
		vTaskDelay(2);
//		xReturn = xQueueReceive(gimbalVal_Queue,gimbal_val_buff,0);
//		
//		xReturn = xQueueReceive(switchesVal_Queue,switches_val_buff,0);
	}
}

