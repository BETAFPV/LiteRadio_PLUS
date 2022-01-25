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

static uint32_t joystickDelayTime;
TaskHandle_t joystickTaskHandle;
/*累加和校验算法*/
static uint16_t checkSum;
uint16_t sendSpam;
extern crsfParameter_t externalRFprarmeter;
void joystickTask(void *param) 
{
    uint16_t hidReportData[8];
    uint16_t requestDataBuff[8];
    uint16_t mixValBuff[8];

    joystickDelayTime = Get_ProtocolDelayTime();
    while(1)
    {
        vTaskDelay(joystickDelayTime);
        xQueueReceive(mixesValQueue,mixValBuff,0);
     
//        hidReportData[0] = map(mixValBuff[0],988,2012,0,2047);
//        hidReportData[1] = map(mixValBuff[1],988,2012,0,2047);
//        hidReportData[2] = map(mixValBuff[2],988,2012,0,2047);
//        hidReportData[3] = map(mixValBuff[3],988,2012,0,2047);
//        hidReportData[4] = map(mixValBuff[4],988,2012,0,2047);
//        hidReportData[5] = map(mixValBuff[5],988,2012,0,2047);
//        hidReportData[6] = map(mixValBuff[6],988,2012,0,2047);
//        hidReportData[7] = map(mixValBuff[7],988,2012,0,2047);

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
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));
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
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));
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
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));

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
                    hidReportData[0] = EXTERNAL_CONFIGER_INFO_ID|(0x01 <<8);;
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
                    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));
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
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));

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
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));
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
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &hidReportData, 8*sizeof(uint16_t));
        }
        checkSum = 0;        

        
    }
}

