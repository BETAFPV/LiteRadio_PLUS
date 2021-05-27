#ifndef _RGB_H_
#define _RGB_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define TIMING_ZERO 2
#define TIMING_ONE 7

#define BREATH_DOWN 0
#define BREATH_UP 1


typedef enum
{
	RED = 1,
	GREEN,
	BLUE,
	YELLOW,
    BLACK,
}color_e;


void color_write(uint8_t color_set,uint8_t brightness);
void Rgb_Set(uint8_t color_set,uint8_t brightness);
void Led_On_Off(uint8_t status);
void Led_Twinkle(uint8_t num);
void Led_Twinkle_Init(uint8_t num);
void Rgb_breath(void);
void Rgb_breath_up(uint8_t color_set);
void Rgb_breath_down(uint8_t color_set);
void WS2812_send(uint8_t *rgb, uint16_t len);
void rgbTask(void* param);
#endif

