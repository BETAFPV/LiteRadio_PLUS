#ifndef _RGB_H_
#define _RGB_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define TIMING_ZERO 2
#define TIMING_ONE 7

#define BREATH_DOWN 0
#define BREATH_UP 1

#define BRIGHTNESS_MAX 255

typedef enum
{
    RED = 0,
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
void RGB_Breath(void);
void RGB_Breath_Up(uint8_t colorIndex);
void RGB_Breath_Down(uint8_t colorIndex);
void rgbTask(void* param);
#endif

