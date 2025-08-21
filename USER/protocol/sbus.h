#ifndef __sbus_H
#define __sbus_H



#include <stdint.h>
#include <stdio.h>



#define SBUS_CHANNEL_DEFAULT_VALUE  0x0000U



typedef struct sbus_frame_s{
    uint8_t             start;  // = 0x0F
    uint16_t            ch1:11;
    uint16_t            ch2:11;
    uint16_t            ch3:11;
    uint16_t            ch4:11;
    uint16_t            ch5:11;
    uint16_t            ch6:11;
    uint16_t            ch7:11;
    uint16_t            ch8:11;
    uint16_t            ch9:11;
    uint16_t            ch10:11;
    uint16_t            ch11:11;
    uint16_t            ch12:11;
    uint16_t            ch13:11;
    uint16_t            ch14:11;
    uint16_t            ch15:11;
    uint16_t            ch16:11;
    uint8_t             statusReserve:4;
    uint8_t             statusFailSaveActivation:1;
    uint8_t             statusLostFrame:1;
    uint8_t             statusCh18:1;
    uint8_t             statusCh17:1;
    uint8_t             stop;   // = 0x00
}__attribute__((__packed__)) sbus_frame_t;



#endif /* __sbus_H */
