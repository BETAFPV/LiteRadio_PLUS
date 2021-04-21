#include "key.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

#define LONG_PRESS_COUNT 	500	/*判断为长按时间（ms）*/
EventGroupHandle_t KeyEventHandle = NULL;
#define PRESSED		0
#define RELEASED	1
static bool bind_pressed = false;
static bool setup_pressed = false;
static bool powerswitch_pressed = false;
static uint32_t bindKeypressedTime;
static uint32_t setupKeypressedTime;
static uint32_t powerswitchKeypressedTime;

static uint8_t bindKeyUpSta = 0x00;
static uint8_t setupKeyUpSta = 0x00;
static uint8_t powerswitchKeyUpSta = 0x00;

void keyTask(void* param)
{
	while(1)
	{
		vTaskDelay(10);
		if(bind_pressed==false && BIND_KEY_STATUS() == PRESSED)
		{
			bind_pressed = true;
			bindKeypressedTime = xTaskGetTickCount();
		}
		if(setup_pressed==false && SETUP_KEY_STATUS() == PRESSED)
		{
			setup_pressed = true;
			setupKeypressedTime = xTaskGetTickCount();
		}
		if(powerswitch_pressed==false && POWER_KEY_STATUS() == PRESSED)
		{
			powerswitch_pressed = true;
			powerswitchKeypressedTime = xTaskGetTickCount();
		}
		
		if(bind_pressed==true && bindKeyUpSta == 0x00)
		{
			if(BIND_KEY_STATUS() == RELEASED)
				bind_pressed = false;
			if((xTaskGetTickCount() - bindKeypressedTime) > LONG_PRESS_COUNT)
			{
				xEventGroupSetBits( KeyEventHandle, BIND_LONG_PRESS );
				bindKeyUpSta = 0x01;
			}
			else if(BIND_KEY_STATUS() == RELEASED)
				xEventGroupSetBits( KeyEventHandle, BIND_SHORT_PRESS );
		}
		if(BIND_KEY_STATUS()== RELEASED)
		{
			bindKeyUpSta = 0x00;
			bindKeypressedTime = xTaskGetTickCount();            //刷新pressedTime，防止再一次进入LONG_PRESS
		}
		
		
		
		
		if(setup_pressed==true && setupKeyUpSta == 0x00)
		{
			if(SETUP_KEY_STATUS() == RELEASED)
				setup_pressed = false;
			if((xTaskGetTickCount() - setupKeypressedTime) > LONG_PRESS_COUNT)
			{
				xEventGroupSetBits( KeyEventHandle, SETUP_LONG_PRESS );
				setupKeyUpSta = 0x01;
			}
			else if(SETUP_KEY_STATUS() == RELEASED)
				xEventGroupSetBits( KeyEventHandle, SETUP_SHORT_PRESS );
		}
		if(SETUP_KEY_STATUS()== RELEASED)
		{
			setupKeyUpSta = 0x00;
			setupKeypressedTime = xTaskGetTickCount();            //刷新pressedTime，防止再一次进入LONG_PRESS
		}
		
		
		
		if(powerswitch_pressed == true && powerswitchKeyUpSta == 0x00)
		{
			if(POWER_KEY_STATUS() == RELEASED)
				powerswitch_pressed = false;
			if((xTaskGetTickCount() - powerswitchKeypressedTime) > LONG_PRESS_COUNT)
			{
				xEventGroupSetBits( KeyEventHandle, POWERSWITCH_LONG_PRESS );
				powerswitchKeyUpSta = 0x01;
				
			}
			else if(POWER_KEY_STATUS()== RELEASED)
				xEventGroupSetBits( KeyEventHandle, POWERSWITCH_SHORT_PRESS );
		}
		if(POWER_KEY_STATUS()== RELEASED)
		{
			powerswitchKeyUpSta = 0x00;
			powerswitchKeypressedTime = xTaskGetTickCount();            //刷新pressedTime，防止再一次进入LONG_PRESS
		}
		

	}
}


//读取按键状态
//uint8_t getKeyState(void)
//{
//	uint8_t temp;
//	temp = keyState;
//	keyState = 0;//读取按键之后清零
//	return temp;
//}


