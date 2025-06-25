#include "joystick.h"
#include "gimbal.h"
#include "switches.h"
#include "usbd_customhid.h"
#include "mixes.h"
#include "stmflash.h"
#include "radiolink.h"
#include "function.h"
#include "status.h"
#include "crsf.h"
#include "common.h"
#include "xinput.h"
#include "phoenix.h"



static uint32_t joystickDelayTime;
TaskHandle_t joystickTaskHandle;
/*累加和校验算法*/
static uint16_t checkSum;
uint16_t sendSpam;
uint16_t liteRadioIndex = LITE_RADIO_UNKNOW;



extern crsfParameter_t externalRFprarmeter;



void joystickTask(void *param) 
{
    uint8_t             hidReportBuf[20] = { 0 };
    uint16_t*           hidReportData = (uint16_t*)hidReportBuf;
    xinput_gamepad_t*   xinGamepad = (xinput_gamepad_t*)hidReportBuf;
    phoenix_gamepad_t*  pnxGamepad = (phoenix_gamepad_t*)hidReportBuf;
    uint16_t            requestDataBuff[8];
    uint16_t            mixValBuff[8];
    
    //if(isXboxMode)xinGamepad->bPackageSize = 20;
    joystickDelayTime = Get_ProtocolDelayTime();
    
    while(1)
    {
        vTaskDelay(joystickDelayTime);
        xQueueReceive(mixesValQueue,mixValBuff,0);
        if (requestType1 == REQUEST_CHANNEL_INFO)
        {
            hidReportData[0] = CHANNEILS_INFO_ID|((requestType2- 0x01) << 8);
            hidReportData[1] = mixData[requestType2- 0x01].gimbalChannel|(mixData[requestType2- 0x01].reverse << 8);
            hidReportData[2] = mixData[requestType2- 0x01].weight|(mixData[requestType2- 0x01].offset << 8);
            for(int i=0;i<7;i++)
            {
                checkSum += hidReportData[i]&0x00FF;
            }
            hidReportData[7] = checkSum; 
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);
        }
        else if (requestType1 == REQUEST_CONIFG_INFO)
        {
            if(requestType2 == 0x00)/*lite_info*/
            {

                STMFLASH_Read(CONFIGER_INFO_ADDR,&requestDataBuff[0],3);
                hidReportData[0] = LITE_CONFIGER_INFO_ID|(VERSION_INDEX << 8);
                hidReportData[1] = requestDataBuff[0]|(requestDataBuff[1] << 8);
                hidReportData[2] = requestDataBuff[2];
                for(int i=0;i<7;i++)
                {
                    checkSum += hidReportData[i]&0x00FF;
                }
                hidReportData[7] = checkSum;
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);
            }
            else if(requestType2 == 0x01)/*internal_info*/
            {
                internalCRSFdata.configStatus = CONFIG_CRSF_ON;
#if defined(LiteRadio_Plus_SX1280)
                internalCRSFdata.crsfParameter.power = tx_config.power;
                internalCRSFdata.crsfParameter.rate = tx_config.rate;
                internalCRSFdata.crsfParameter.TLM = tx_config.tlm;
                hidReportData[0] = INTERNAL_CONFIGER_INFO_ID|(0x01 <<8);
                hidReportData[1] = internalCRSFdata.crsfParameter.power|(internalCRSFdata.crsfParameter.rate << 8);
                hidReportData[2] = internalCRSFdata.crsfParameter.TLM;
#endif
                for(int i=0;i<7;i++)
                {
                    checkSum += hidReportData[i] & 0x00FF;
                }
                hidReportData[7] = checkSum;
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);

            }
            else if(requestType2 == 0x02)/*external_info*/
            {
//                if(sendSpam>100)
//                {
//                    requestType1 = 0x00;
//                    requestType2 = 0x01;
//                    sendSpam = 0;
//                }
//                else
//                {
//                    sendSpam++;
//                }
                sendSpam++;
                if((externalCRSFdata.regulatoryDomainIndex!=0&&externalRFprarmeter.power!=0xff&&externalRFprarmeter.rate!=0xff&&externalRFprarmeter.TLM!=0xff)||(sendSpam>=10000))
                {
                    hidReportData[0] = EXTERNAL_CONFIGER_INFO_ID|(0x01 <<8);
                    uint8_t rateToConfigurator = 0,powerToConfigurator = 0;
                    switch (externalCRSFdata.regulatoryDomainIndex)
                    {
                        case NANO_TX_915Mhz:
                        {
                            switch (externalCRSFdata.crsfParameter.rate)
                            {
                                case FREQ_900_RATE_200HZ:
                                    rateToConfigurator = 3;
                                    break;
                                case FREQ_900_RATE_100HZ:
                                    rateToConfigurator = 2;
                                    break;
                                case FREQ_900_RATE_50HZ:
                                    rateToConfigurator = 1;
                                    break;
                                case FREQ_900_RATE_25HZ:
                                    rateToConfigurator = 0;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        }
                        case NANO_TX_2400Mhz:
                        {
                            switch (externalCRSFdata.crsfParameter.rate)
                            {
                                case FREQ_2400_RATE_500HZ:
                                    rateToConfigurator = 3;
                                    break;
                                case FREQ_2400_RATE_250HZ:
                                    rateToConfigurator = 2;
                                    break;
                                case FREQ_2400_RATE_150HZ:
                                    rateToConfigurator = 1;
                                    break;
                                case FREQ_2400_RATE_50HZ:
                                    rateToConfigurator = 0;
                                    break;
                                default:
                                    break;
                            }
//                            rateToConfigurator = externalCRSFdata.crsfParameter.rate;
                            break;
                        }
                        default:
                            break;
                                    
                    }
                    switch(externalCRSFdata.regulatoryDomainIndex)
                    {
                        case NANO_TX_915Mhz:
                            switch(externalCRSFdata.crsfParameter.power)
                            {
                                case power915Mhz100mw:
                                    powerToConfigurator = power100mw;
                                    break;
                                case power915Mhz250mw:
                                    powerToConfigurator = power250mw;
                                    break;
                                case power915Mhz500mw:
                                    powerToConfigurator = power500mw;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case NANO_TX_2400Mhz:
                            powerToConfigurator = externalCRSFdata.crsfParameter.power;
                            break;
                        
                    }
                    hidReportData[1] = powerToConfigurator|(rateToConfigurator<< 8);
                    hidReportData[2] = externalCRSFdata.crsfParameter.TLM|(externalCRSFdata.regulatoryDomainIndex << 8);
                    for(int i=0;i<7;i++)
                    {
                        checkSum += hidReportData[i]&0x00FF;
                    }
                    hidReportData[7] = checkSum;
                    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);
                }
                else//旧代码上位机配置外置高频头相关的业务已不可用，强制发送一些固定信息，无实际效果
                {
                    uint8_t* buf = (uint8_t*)hidReportData;
                    buf[0] = EXTERNAL_CONFIGER_INFO_ID;
                    buf[1] = 0x01;
                    buf[2] = power50mw;
                    buf[3] = ELRS_PkgRate2400_250HZ;
                    buf[4] = TLM_1_64;
                    buf[5] = NANO_TX_2400Mhz;
                    for(int i=0;i<7;i++)
                    {
                        checkSum += hidReportData[i]&0x00FF;
                    }
                    hidReportData[7] = checkSum;
                    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);
                }
            }
        }
        else if(requestType1 == REQUEST_DEVICE_INFO)
        {
            sendSpam = 0;
            externalCRSFdata.regulatoryDomainIndex = 0;
            externalRFprarmeter.power = 0xff;
            externalRFprarmeter.rate = 0xff;
            externalRFprarmeter.TLM =0xff;
            
            uint16_t device_info_buff[10] = {0};
            STMFLASH_Read(LITE_RADIO_HARDWARE_TYPE_ADDR,&device_info_buff[0],1);
            STMFLASH_Read(INTERNAL_RADIO_TYPE_ADDR,&device_info_buff[1],1);
            STMFLASH_Read(THROTTLE_ROCKER_POSITION_ADDR,&device_info_buff[2],1);
            STMFLASH_Read(HARDWARE_MAJOR_VERSION_ADDR,&device_info_buff[3],1);
            STMFLASH_Read(HARDWARE_MINOR_VERSION_ADDR,&device_info_buff[4],1);
            STMFLASH_Read(HARDWARE_PATCH_VERSION_ADDR,&device_info_buff[5],1);
            device_info_buff[6] = FIRMWARE_MAJOR_VERSION;
            device_info_buff[7] = FIRMWARE_MINOR_VERSION;
            device_info_buff[8] = FIRMWARE_PITCH_VERSION;
            STMFLASH_Read(FIRST_FLASH_MARK_ADDR,&device_info_buff[9],1);//0xa55a
            hidReportData[0] = DEVICE_INFO_ID;
            hidReportData[1] = (device_info_buff[0]&0x00ff)|((device_info_buff[1]&0x00FF)<<8);
            hidReportData[2] = (device_info_buff[2]&0x00ff)|((device_info_buff[3]&0x00FF)<<8);
            hidReportData[3] = (device_info_buff[4]&0x00ff)|((device_info_buff[5]&0x00FF)<<8);
            hidReportData[4] = (device_info_buff[6]&0x00ff)|((device_info_buff[7]&0x00FF)<<8);
            hidReportData[5] = (device_info_buff[8]&0x00ff);
            hidReportData[6] = device_info_buff[9];
            for(int i=0;i<7;i++)
            {
                checkSum += hidReportData[i]&0x00FF;
            }
            hidReportData[7] = checkSum;
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);

        }
        else if(requestType1 == REQUEST_EXTRA_CONFIG_INFO)
        {
            uint16_t JoystickDeadZonePercent,BuzzerSwitch;
            STMFLASH_Read(JoystickDeadZonePercent_ADDR,&JoystickDeadZonePercent,1);
            STMFLASH_Read(BuzzerSwitch_ADDR,&BuzzerSwitch,1);
            hidReportData[0] = EXTRA_CUSTOM_CONFIG_ID;
            hidReportData[1] = (JoystickDeadZonePercent&0x00ff)|((BuzzerSwitch&0x00ff)<<8);

            for(int i=0;i<7;i++)
            {
                checkSum += hidReportData[i]&0x00FF;
            }
            hidReportData[7] = checkSum;
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);
        }
        else 
        {
            if(isXboxMode)
            {
                xinGamepad->sThumbRX = xinputMap(mixValBuff[0], 988,2012, INT16_MIN, INT16_MAX);
                xinGamepad->sThumbRY = xinputMap(mixValBuff[1], 988,2012, INT16_MIN, INT16_MAX);
                xinGamepad->sThumbLY = xinputMap(mixValBuff[2], 988,2012, INT16_MIN, INT16_MAX);
                xinGamepad->sThumbLX = xinputMap(mixValBuff[3], 988,2012, INT16_MIN, INT16_MAX);
                xinGamepad->bReverse = 0;
                xinGamepad->bPackageSize = 20;
                xinGamepad->bHat = 0;
                xinGamepad->bButtons = 0;
                //SA
                if      (mixValBuff[4] < 1200){
                    xinGamepad->bLeftTrigger = 0;
                }else if(mixValBuff[4] < 1800){
                    xinGamepad->bLeftTrigger = 0;
                }else                         {
                    xinGamepad->bLeftTrigger = UINT8_MAX;
                }
                //SB
                if      (mixValBuff[5] < 1200){
                    xinGamepad->bButtons |= XINPUT_GAMEPAD_BUTTON_6;
                }else if(mixValBuff[5] < 1800){
                    
                }else                         {
                    xinGamepad->bButtons |= XINPUT_GAMEPAD_BUTTON_5;
                }
                //SC
                if      (mixValBuff[6] < 1200){
                    xinGamepad->bHat |= XINPUT_GAMEPAD_HAT_DOWN;
                }else if(mixValBuff[6] < 1800){
                }else                         {
                    xinGamepad->bHat |= XINPUT_GAMEPAD_HAT_UP;
                }
                //SD
                if      (mixValBuff[7] < 1200){
                    xinGamepad->bRightTrigger = 0;
                }else if(mixValBuff[7] < 1800){
                    xinGamepad->bRightTrigger = 0;
                }else                         {
                    xinGamepad->bRightTrigger = UINT8_MAX;
                }
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 20);
            }
            else if(isPhoenixMode)
            {
                pnxGamepad->ch1 = phoenixCrsfToByte(mixValBuff[0]);
                pnxGamepad->ch2 = phoenixRCHash;
                pnxGamepad->ch3 = phoenixCrsfToByte(mixValBuff[1]);
                pnxGamepad->ch4 = phoenixCrsfToByte(mixValBuff[2]);
                pnxGamepad->ch5 = phoenixCrsfToByte(mixValBuff[3]);
                pnxGamepad->ch6 = phoenixCrsfToByte(mixValBuff[4]);
                pnxGamepad->ch7 = phoenixCrsfToByte(mixValBuff[5]);
                pnxGamepad->ch8 = 0x00;
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, PHOENIX_HID_REPORT_SIZE);
            }
            else
            {
                hidReportData[0] = map(mixValBuff[0],988,2012,0,2047);
                hidReportData[1] = map(mixValBuff[1],988,2012,0,2047);
                hidReportData[2] = map(mixValBuff[2],988,2012,0,2047);
                hidReportData[3] = map(mixValBuff[3],988,2012,0,2047);
                hidReportData[4] = map(mixValBuff[4],988,2012,0,2047);
                hidReportData[5] = map(mixValBuff[5],988,2012,0,2047);
                hidReportData[6] = map(mixValBuff[6],988,2012,0,2047);
                hidReportData[7] = map(mixValBuff[7],988,2012,0,2047);
                hidReportBuf[16] = 0;
                //模式切换映射
                if      (mixValBuff[5] < 1200)  hidReportBuf[16] |= 0x80;
                else if (mixValBuff[5] < 1800)  hidReportBuf[16] |= 0x40;
                //自定义按键5
                if      (mixValBuff[6] > 1800)  hidReportBuf[16] |= 0x08;
                //自定义按键6
                if      (mixValBuff[7] > 1800)  hidReportBuf[16] |= 0x02;
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)hidReportData, 18);
            }
        }
        checkSum = 0;        

        
    }
}

