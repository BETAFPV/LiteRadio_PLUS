#include "phoenix.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"



extern USBD_HandleTypeDef hUsbDeviceFS;



void USBD_FS_Set_controller_type(void);
void USBD_CUSTOM_HID_Update_desc_len(void);



uint8_t isPhoenixMode = 0;



/** 
* @biref    下面这两个函数用来修改stm32 usb库的描述符为凤凰模拟器相关信息，需要分别放到指定位置
*               void USBD_FS_Set_controller_type(void);
*               放到
*               void USBD_CUSTOM_HID_Update_desc_len(void);
*               放到
*/



void phoenixUsbInit(){
    
}


