#include "sbus.h"



sbus_frame_t sbusFrame = {
    .start = 0x0FU,
    .statusReserve = 0,
    .statusFailSaveActivation = 0,
    .statusLostFrame = 0,
    .statusCh18 = 0,
    .statusCh17 = 0,
    .stop = 0x00U,
};



uint16_t sbusChannelBuf[16] = {
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
    SBUS_CHANNEL_DEFAULT_VALUE,
};
void sendSbusChannels(uint16_t* ch, uint8_t chNum){
    for(uint8_t i=0; i<16; i++){
        if(i<chNum){
            sbusChannelBuf[i] = ch[i];
        }
    }
    sbusFrame.ch1  = sbusChannelBuf[0];
    sbusFrame.ch2  = sbusChannelBuf[1];
    sbusFrame.ch3  = sbusChannelBuf[2];
    sbusFrame.ch4  = sbusChannelBuf[3];
    sbusFrame.ch5  = sbusChannelBuf[4];
    sbusFrame.ch6  = sbusChannelBuf[5];
    sbusFrame.ch7  = sbusChannelBuf[6];
    sbusFrame.ch8  = sbusChannelBuf[7];
    sbusFrame.ch9  = sbusChannelBuf[8];
    sbusFrame.ch10 = sbusChannelBuf[9];
    sbusFrame.ch11 = sbusChannelBuf[10];
    sbusFrame.ch12 = sbusChannelBuf[11];
    sbusFrame.ch13 = sbusChannelBuf[12];
    sbusFrame.ch14 = sbusChannelBuf[13];
    sbusFrame.ch15 = sbusChannelBuf[14];
    sbusFrame.ch16 = sbusChannelBuf[15];
}


