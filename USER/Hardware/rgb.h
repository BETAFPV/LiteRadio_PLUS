#ifndef _RGB_H_
#define _RGB_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define TIMING_ZERO 30
#define TIMING_ONE 60

#define BREATH_DOWN 0
#define BREATH_UP 1

#define BRIGHTNESS_MIN 0
#define BRIGHTNESS_MAX 255

#define BIND_RGB	              (0x01 << 0)	
#define SETUP_RGB           	  (0x01 << 1)	
#define DATA_RGB	              (0x01 << 2)	
#define SHUTDOWN_RGB	          (0x01 << 3)
#define CHRG_AND_JOYSTICK_RGB     (0x01 << 4)
#define POWER_ON_RGB	          (0x01 << 5)	
#define POWER_OFF_RGB	          (0x01 << 6)	

typedef enum
{
    RGB_INIT = 0,
    RGB_SHUTDOWN ,
    RGB_DATA,
    RGB_CHRG_AND_JOYSTICK,
    RGB_BIND,
}rgbStatus_e;


typedef enum
{
    RED = 0,
    GREEN,
    BLUE,
    YELLOW,
    BLACK,
    WHITE,
    PURPLE,
}color_e;

extern EventGroupHandle_t rgbEventHandle;

void Color_Set(uint8_t colorIndex,uint8_t brightness);
void RGB_Set(uint8_t colorIndex,uint8_t brightness);
void RGB_TwinkleForInit(uint8_t num,uint16_t twinkleDelayTime);
void RGB_SetupTwinkle(void);
void RGB_BindTwinkle(void);
void RGB_Breath(uint8_t colorIndex);
void RGB_Breath_Up(uint8_t colorIndex);
void RGB_Breath_Down(uint8_t colorIndex);
void rgbTask(void* param);
#endif

