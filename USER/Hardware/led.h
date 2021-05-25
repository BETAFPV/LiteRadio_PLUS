#ifndef _LED_H_
#define _LED_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define TIMING_ZERO 2
#define TIMING_ONE 7

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
void WS2812_send(uint8_t *rgb, uint16_t len);
#endif

