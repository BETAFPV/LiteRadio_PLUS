#ifndef __xinput_H
#define __xinput_H



#include "stdio.h"
#include "stdint.h"



typedef enum
{
    XINPUT_GAMEPAD_HAT_NONE         = 0x00U,
    XINPUT_GAMEPAD_HAT_UP           = 0x01U,
    XINPUT_GAMEPAD_HAT_DOWN         = 0x02U,
    XINPUT_GAMEPAD_HAT_LEFT         = 0x04U,
    XINPUT_GAMEPAD_HAT_RIGHT        = 0x08U,
    XINPUT_GAMEPAD_HAT_BUTTON_8     = 0x10U,
    XINPUT_GAMEPAD_HAT_BUTTON_7     = 0x20U,
    XINPUT_GAMEPAD_HAT_BUTTON_9     = 0x40U,
    XINPUT_GAMEPAD_HAT_BUTTON_10    = 0x80U,
}xinput_gamepad_hat_e;

typedef enum
{
    XINPUT_GAMEPAD_BUTTON_NONE	    = 0x00U,
    XINPUT_GAMEPAD_BUTTON_5	        = 0x01U,
    XINPUT_GAMEPAD_BUTTON_6	        = 0x02U,
    XINPUT_GAMEPAD_BUTTON_START     = 0x04U,
    XINPUT_GAMEPAD_BUTTON_BACK      = 0x08U,
    XINPUT_GAMEPAD_BUTTON_1	        = 0x10U,
    XINPUT_GAMEPAD_BUTTON_2	        = 0x20U,
    XINPUT_GAMEPAD_BUTTON_3         = 0x40U,
    XINPUT_GAMEPAD_BUTTON_4         = 0x80U,
}xinput_gamepad_button_e;

typedef struct xinput_gamepad_s
{
    uint8_t     bReverse;
    uint8_t     bPackageSize;
    uint8_t     bHat;               //xinput_gamepad_hat_e
    uint8_t     bButtons;           //xinput_gamepad_button_e
    uint8_t     bLeftTrigger;
    uint8_t     bRightTrigger;
    int16_t     sThumbLX;
    int16_t     sThumbLY;
    int16_t     sThumbRX;
    int16_t     sThumbRY;
    uint8_t     bReverseArray[6];
}xinput_gamepad_t;



extern uint8_t isXboxMode;



int32_t xinputMap(int32_t pos, int32_t i1, int32_t i2, int32_t o1, int32_t o2);



#endif /* __xinput_H */
