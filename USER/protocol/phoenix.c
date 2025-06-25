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
uint8_t phoenixRCHash = 0;
uint8_t phoenixRCRepCount = 0;



/* 处理hid回传报表，里面有hash更新信息，放在custom hid if文件的outEvent接口函数中 */
void HandleSetReport(){
    uint8_t     i;
    uint8_t     type;
    uint32_t    sum;
    uint8_t*    b;
    
    phoenixRCHash = 0;
    USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceFS.pClassData;
    b = hhid->Report_buf;
    
    // calculate phoenixrc hash
    type = b[2];
    if(type > 0)type--;
    type >>= 5;
    
    // 1. calculate sum
    switch (type) {
    case 0: sum = b[6]; break;
    case 1: sum = b[3]; break;
    case 2: sum = b[4] + b[5]; break;
    case 3: sum = b[1] + b[6]; break;
    case 4: sum = b[0] + b[7]; break;
    case 5: sum = b[5] + b[6]; break;
    case 6: sum = b[0] + b[1] + b[3] + b[4]; break;
    case 7:
        sum = 0;
        for(i = 0; i < 8; i++)
        if (i != 2)
            sum += b[i];
        break;
    }
    // 2. calc hash
    type &= 3;
    switch (type){
    case 0: phoenixRCHash = (uint8_t)(((sum * 8) & 0x7F) + 0x0D); break;
    case 1: phoenixRCHash = (uint8_t)(((sum * 2) & 0x3F) + 0x12); break;
    case 2: phoenixRCHash = (uint8_t)((((sum + 8) / 2) & 0x1F) + 0x10); break;
    case 3: phoenixRCHash = (uint8_t)((((sum + 4) / 8) & 0xF) + 0x18); break;
    }
    
    phoenixRCRepCount = 250;
}



/* 更新phoenix hash，看源代码好像正常传数据时没用到此函数 */
void UpdatePhoenixHash(){
    if(phoenixRCRepCount > 0){
        phoenixRCRepCount--;
        if(phoenixRCRepCount == 0)phoenixRCHash = 0;
    }
}



/**
* @brief    CRSF 范围 1500+-512 = 988~2012
*/
uint8_t phoenixCrsfToByte(uint16_t channelData){
    int val = channelData - 988;
	
    if(val < 0)val = 0;
	val = (val * 255) >> 10;
	if(val > 255)val = 255;
    
	return (uint8_t)val;
}



/** 
* @biref    下面这两个函数用来修改stm32 usb库的描述符为凤凰模拟器相关信息，需要分别放到指定位置
void phoenixSetUsbDevDesc(void){
    
}

void USBD_CUSTOM_HID_Update_desc_len(void){
    
}

*/



void phoenixUsbInit(){
    
}


