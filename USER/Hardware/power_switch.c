#include <stdbool.h>
#include "power_switch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "rgb.h"
#include "cmsis_os.h"
#include "buzzer.h"
#include "radiolink.h"

SemaphoreHandle_t powerSemaphore;
EventGroupHandle_t powerEventHandle = NULL;
TaskHandle_t powerTaskHandle;

static bool powerswitchStatus = false;

uint8_t key_status;
int i = 0;
void powerswitchTask(void* param)
{
	EventBits_t R_event;
	while(1)
	{
		vTaskDelay(100);
		R_event= xEventGroupWaitBits( powerEventHandle,
		                              POWER_ON|POWER_OFF,
		                              pdTRUE,
	                                  pdFALSE,
		                              0);
		if((R_event & POWER_ON) == POWER_ON)
		{
          //  Rgb_Set(BLUE,255);
            xEventGroupSetBits( buzzerEventHandle, POWER_ON_RING);
            osDelay(1200);
            POWER_PIN_HOLD_UP();
            Rgb_breath_up(BLUE);
            	taskENTER_CRITICAL();	/*Ω¯»Î¡ŸΩÁ*/
            vTaskSuspend(powerTaskHandle);

            	taskEXIT_CRITICAL();
		}
		if((R_event & POWER_OFF) == POWER_OFF)
		{
          //  Rgb_Set(BLACK,255);
            xEventGroupSetBits( buzzerEventHandle, POWER_OFF_RING);
            osDelay(1200);
            Rgb_breath_down(RED);
            POWER_PIN_HOLD_DOWN();
		}        
		
	}
}



