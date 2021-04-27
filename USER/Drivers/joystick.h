#ifndef __JOYSTICK_H_
#define __JOYSTICK_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "usbd_hid.h"
#include "usb_device.h"

#define THROTTLE_ENABLED				1					// 1 for enabled, 0 for disabled
#define RUDDER_ENABLED					1					// 1 for enabled, 0 for disabled
#define JOYSTICK_AXIS_NUM				3
#define SECONDARY_AXIS_NUM				3

#define AXIS_NUM								((THROTTLE_ENABLED) + (JOYSTICK_AXIS_NUM) \
																+ (SECONDARY_AXIS_NUM) + (RUDDER_ENABLED))
								

void joystickTask(void *param);

#endif