#ifndef __phoenix_H
#define __phoenix_H



#include <stdint.h>
#include <stdio.h>



/* 凤凰模拟器HID发送缓冲区长度（字节） */
#define PHOENIX_HID_REPORT_SIZE     8



typedef struct phoenix_gamepad_s{
    uint8_t         ch1;
    uint8_t         ch2;
    uint8_t         ch3;
    uint8_t         ch4;
    uint8_t         ch5;
    uint8_t         ch6;
    uint8_t         ch7;
    uint8_t         ch8;
}phoenix_gamepad_t;



extern uint8_t isPhoenixMode;
extern uint8_t phoenixRCHash;
extern uint8_t phoenixRCRepCount;



void    phoenixUsbInit(void);
void    HandleSetReport(void);
void    UpdatePhoenixHash(void);
uint8_t phoenixCrsfToByte(uint16_t channelData);



/* 需要在外部c文件实现的接口函数 */
void phoenixSetDescLen(void);   // 修改hid配置描述符长度



/**
* @brief    这个宏函数要加到遥控器USB初始化函数最前面，
*           用于切换初始化内容为凤凰模拟器的描述符相关数据
*/
#define __PHOENIX_HID_INIT()    do{ \
                                    phoenixUsbInit(); \
                                    return; \
                                }while(0) \



#endif /*  */
