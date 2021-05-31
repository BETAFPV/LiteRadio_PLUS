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


void Color_Set(uint8_t colorIndex,uint8_t brightness);
void RGB_Set(uint8_t colorIndex,uint8_t brightness);
void RGB_Toggle(uint8_t status);
void RGB_TwinkleForInit(uint8_t num,uint16_t twinkleDelayTime);
void RGB_Twinkle(uint8_t num,uint16_t twinkleDelayTime);
void Rgb_Breath(void);
void Rgb_Breath_Up(uint8_t colorIndex);
void Rgb_Breath_Down(uint8_t colorIndex);
void WS2812_send(uint8_t *rgb, uint16_t len);
void rgbTask(void* param);
#endif

