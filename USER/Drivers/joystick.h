#ifndef __JOYSTICK_H_
#define __JOYSTICK_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "usb_device.h"

#define THROTTLE_ENABLED				1					// 1 for enabled, 0 for disabled
#define RUDDER_ENABLED					1					// 1 for enabled, 0 for disabled
#define JOYSTICK_AXIS_NUM				3
#define SECONDARY_AXIS_NUM				3

#define AXIS_NUM								((THROTTLE_ENABLED) + (JOYSTICK_AXIS_NUM) \
																+ (SECONDARY_AXIS_NUM) + (RUDDER_ENABLED))

     
typedef enum
{
    REQUEST_COMMOND          = 0x00,
    REQUEST_CHANNEL_INFO     = 0x01,   
    REQUEST_CONIFG_INFO      = 0x02,
    REQUEST_DEVICE_INFO      = 0x03,
    REQUEST_EXTRA_CONFIG_INFO      = 0x04,
}requestProtocol_e;


extern uint16_t sendSpam;                                                                
                                                                
void joystickTask(void *param);
extern TaskHandle_t joystickTaskHandle;
#endif
