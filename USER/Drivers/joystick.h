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

typedef enum{
    LITE_RADIO_UNKNOW = 0U,
    LITE_RADIO_2_SE = 1U,
    LITE_RADIO_2_SE_V2_SX1280 = 2U,
    LITE_RADIO_2_SE_V2_CC2500 = 3U,
    LITE_RADIO_3_SX1280 = 4U,
    LITE_RADIO_3_CC2500 = 5U,
    LITE_RADIO_1_CC2500 = 6U,
    LITE_RADIO_4_SE_SX1280 = 7U,
    LITE_RADIO_2_SIM = 8U,
}lite_radio_index_e;

extern uint16_t sendSpam;   
extern uint16_t liteRadioIndex;
                                                                
void joystickTask(void *param);
extern TaskHandle_t joystickTaskHandle;
#endif
