#include "key.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

#define LONG_PRESS_COUNT 	500	/*判断为长按时间（ms）*/

#define PRESSED		0
#define RELEASED	1

static bool bind_pressed;
static bool setup_pressed;
static bool powerswitch_pressed;
static uint8_t keyState;
static uint32_t pressedTime;

void keyTask(void* param)
{
	while(1)
	{
		vTaskDelay(1);
		
		if(bind_pressed==false && BIND_KEY_STATUS() == PRESSED)
		{
			bind_pressed = true;
			pressedTime = xTaskGetTickCount();
		}
		if(setup_pressed==false && SETUP_KEY_STATUS() == PRESSED)
		{
			setup_pressed = true;
			pressedTime = xTaskGetTickCount();
		}
		if(powerswitch_pressed==false && POWER_KEY_STATUS() == PRESSED)
		{
			powerswitch_pressed = true;
			pressedTime = xTaskGetTickCount();
		}
		
		if(bind_pressed==true)
		{
			if(BIND_KEY_STATUS() == RELEASED)
				bind_pressed = false;
			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
				keyState = BIND_LONG_PRESS;
			else if(BIND_KEY_STATUS() == RELEASED)
				keyState = BIND_SHORT_PRESS;
		}
		if(setup_pressed==true)
		{
			if(SETUP_KEY_STATUS() == RELEASED)
				setup_pressed = false;
			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
				keyState = SETUP_LONG_PRESS;
			else if(SETUP_KEY_STATUS() == RELEASED)
				keyState = SETUP_SHORT_PRESS;
		}
		if(powerswitch_pressed == true)
		{
			if(POWER_KEY_STATUS() == RELEASED)
				powerswitch_pressed = false;
			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
				keyState = POWERSWITCH_LONG_PRESS;
			else if(POWER_KEY_STATUS()== RELEASED)
				keyState = POWERSWITCH_SHORT_PRESS;
		}

	}
}


//读取按键状态
uint8_t getKeyState(void)
{
	uint8_t temp;
	temp = keyState;
	keyState = 0;//读取按键之后清零
	return temp;
}


