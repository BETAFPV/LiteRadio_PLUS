#include "key.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.h"

EventGroupHandle_t KeyEventHandle = NULL;

static bool bindPressed = false;
static bool setupPressed = false;
static bool powerPressed = false;
static uint32_t bindKeyPressedTime;
static uint32_t setupKeypressedTime;
static uint32_t powerKeyPressedTime;

static uint8_t bindKeyUpSta = 0x00;
static uint8_t setupKeyUpSta = 0x00;
static uint8_t powerswitchKeyUpSta = 0x00;

void keyTask(void* param)
{
	while(1)
	{
		vTaskDelay(10);
		if(bindPressed==false && BIND_KEY_STATUS() == PRESSED)
		{
			bindPressed = true;
			bindKeyPressedTime = xTaskGetTickCount();
		}
		if(setupPressed==false && SETUP_KEY_STATUS() == PRESSED)
		{
			setupPressed = true;
			setupKeypressedTime = xTaskGetTickCount();
		}
		if(powerPressed==false && POWER_KEY_STATUS() == PRESSED)
		{
			powerPressed = true;
			powerKeyPressedTime = xTaskGetTickCount();
		}
		
		if(bindPressed==true && bindKeyUpSta == 0x00)
		{
			if(BIND_KEY_STATUS() == RELEASED)
				bindPressed = false;
			if((xTaskGetTickCount() - bindKeyPressedTime) > LONG_PRESS_COUNT)
			{
				xEventGroupSetBits( KeyEventHandle, BIND_LONG_PRESS );
				bindKeyUpSta = 0x01;
			}
			else if(BIND_KEY_STATUS() == RELEASED)
            {
				xEventGroupSetBits( KeyEventHandle, BIND_SHORT_PRESS );
            }
		}
		if(BIND_KEY_STATUS()== RELEASED)
		{
			bindKeyUpSta = 0x00;
			bindKeyPressedTime = xTaskGetTickCount();            //刷新pressedTime，防止再一次进入LONG_PRESS
		}
		
		
		
		
		if(setupPressed==true && setupKeyUpSta == 0x00)
		{
			if(SETUP_KEY_STATUS() == RELEASED)
				setupPressed = false;
			if((xTaskGetTickCount() - setupKeypressedTime) > LONG_PRESS_COUNT)
			{
				xEventGroupSetBits( KeyEventHandle, SETUP_LONG_PRESS );
				setupKeyUpSta = 0x01;
			}
			else if(SETUP_KEY_STATUS() == RELEASED)
            {
                xEventGroupSetBits( KeyEventHandle, SETUP_SHORT_PRESS );
            }
		}
		if(SETUP_KEY_STATUS()== RELEASED)
		{
			setupKeyUpSta = 0x00;
			setupKeypressedTime = xTaskGetTickCount();            //刷新pressedTime，防止再一次进入LONG_PRESS
		}
		
		
		
		if(powerPressed == true && powerswitchKeyUpSta == 0x00)
		{
			if(POWER_KEY_STATUS() == RELEASED)
				powerPressed = false;
			if((xTaskGetTickCount() - powerKeyPressedTime) > LONG_PRESS_COUNT)
			{
				xEventGroupSetBits( KeyEventHandle, POWERSWITCH_LONG_PRESS );
				powerswitchKeyUpSta = 0x01;
			}
			else if(POWER_KEY_STATUS()== RELEASED)
            {
				xEventGroupSetBits( KeyEventHandle, POWERSWITCH_SHORT_PRESS );
            }
		}
		if(POWER_KEY_STATUS()== RELEASED)
		{
			powerswitchKeyUpSta = 0x00;
			powerKeyPressedTime = xTaskGetTickCount();            //刷新pressedTime，防止再一次进入LONG_PRESS
		}
		
	}
}



