#include "crsf.h"
#include "usart.h"
#include "function.h"

uint8_t CRSF_Packet[26] = {0x0F, 0x00, 0x34, 0x1F, 0xA8, 0x09, 0x08, 0x6A, 0x50, 0x03,0x10, 0x80, 0x00,
                             0x04, 0x20, 0x00, 0x01, 0x08, 0x07, 0x38, 0x00, 0x10, 0x80, 0x00, 0x04,0x00};
uint8_t outBuffer[LinkStatisticsFrameLength + 4] = {0};
static uint16_t channelDataBuff[16] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};
uint8_t crsfRSSI;
static GimbalReverseTypeDef gimbalReverseFlg;//摇杆输出反向标志 0：不反向 1：反向
uint16_t controlDataBuff[8] = {0};

uint8_t crsfRxBuff[10] = {0};
static uint16_t crsfRxCount = 0;
void CRSF_SetBind()
{
    HAL_Delay(1);
}

void CRSF_Init(uint8_t protocolIndex)
{
    HAL_Delay(1);
}

uint16_t CRSF_Process(uint16_t* control_data)
{
    Get_CRSFPackage(CRSF_Packet,control_data);
	HAL_HalfDuplex_EnableTransmitter(&huart1);
    if(HAL_UART_Transmit_DMA(&huart1,CRSF_Packet,26)!= HAL_OK) //判断是否发送正常，如果出现异常则进入异常中断函数
    {
        Error_Handler();
    }
    if(crsfRxCount>999)
    {
        HAL_HalfDuplex_EnableReceiver(&huart1);
        HAL_UART_Receive(&huart1, crsfRxBuff, 2, 200);
        crsfRxCount = 0;
    }
    else
    {
        crsfRxCount++;        
    }
	return 0; 
}

void Get_LinkStatis()
{

    if(outBuffer[0] == CRSF_ADDRESS_RADIO_TRANSMITTER)
    {
        if(outBuffer[1] == (LinkStatisticsFrameLength + 2))
        {
            if(outBuffer[2] == CRSF_FRAMETYPE_LINK_STATISTICS)
            {
                
                uint8_t crc = crc8(&outBuffer[2], LinkStatisticsFrameLength + 1);
                if(crc == outBuffer[LinkStatisticsFrameLength + 3])
                {
                    crsfRSSI = outBuffer[6];
                }                    
            }
        }
    }
    

    
}

void Get_CRSFPackage(uint8_t* channelToCRSF,uint16_t* controlDataBuff)
{
    uint16_t SWA_Temp;
	uint16_t SWB_Temp;
	uint16_t SWC_Temp;
	uint16_t SWD_Temp;
    uint32_t dataVal[16];
      
    
#ifdef MODE2        
    gimbalReverseFlg.AILERON  = 0;
    gimbalReverseFlg.ELEVATOR = 1;
    gimbalReverseFlg.RUDDER   = 1;
    gimbalReverseFlg.THROTTLE = 0;
#else
    gimbalReverseFlg.AILERON  = 0;
    gimbalReverseFlg.ELEVATOR = 0;
    gimbalReverseFlg.RUDDER   = 1;
    gimbalReverseFlg.THROTTLE = 1;
#endif 	


    channelDataBuff[0] =(gimbalReverseFlg.ELEVATOR == 1)?(2*CHANNEL_OUTPUT_MID - controlDataBuff[ELEVATOR]):controlDataBuff[ELEVATOR];
    channelDataBuff[1] =(gimbalReverseFlg.AILERON  == 1)?(2*CHANNEL_OUTPUT_MID - controlDataBuff[AILERON]) :controlDataBuff[AILERON];
    channelDataBuff[2] =(gimbalReverseFlg.THROTTLE == 1)?(2*CHANNEL_OUTPUT_MID - controlDataBuff[THROTTLE]):controlDataBuff[THROTTLE];
    channelDataBuff[3] =(gimbalReverseFlg.RUDDER   == 1)?(2*CHANNEL_OUTPUT_MID - controlDataBuff[RUDDER])  :controlDataBuff[RUDDER];

	channelDataBuff[4] = controlDataBuff[4];
    channelDataBuff[5] = controlDataBuff[5];
    channelDataBuff[6] = controlDataBuff[6];
    channelDataBuff[7] = controlDataBuff[7];
    
	SWA_Temp = map(channelDataBuff[4],993,2000,163,1811);
	SWB_Temp = map(channelDataBuff[5],993,2000,163,1811);
	SWC_Temp = map(channelDataBuff[6],993,2000,163,1811);
	SWD_Temp = map(channelDataBuff[7],993,2000,163,1811);  
    

    dataVal[0] = channelDataBuff[AILERON] ;
    dataVal[1] = channelDataBuff[ELEVATOR];
    dataVal[2] = channelDataBuff[THROTTLE];
    dataVal[3] = channelDataBuff[RUDDER];
    dataVal[4] = SWA_Temp;
    dataVal[5] = SWB_Temp;
    dataVal[6] = SWC_Temp;
    dataVal[7] = SWD_Temp;
    dataVal[8] = DEFAULT_VALUE;
    dataVal[9] = DEFAULT_VALUE;
    dataVal[10] = DEFAULT_VALUE;    
    dataVal[11] = DEFAULT_VALUE;  
    dataVal[12] = DEFAULT_VALUE;
    dataVal[13] = DEFAULT_VALUE;
    dataVal[14] = DEFAULT_VALUE;
    dataVal[15] = DEFAULT_VALUE;    

    uint8_t *buf = channelToCRSF;
    *buf++ = CRSF_ADDRESS_CRSF_TRANSMITTER; //MODULE ADDRESS: 0xEE
    *buf++ = 24; // 1(ID) + 22 + 1(CRC)
    uint8_t * crc_start = buf;
    *buf++ = CRSF_FRAMETYPE_RC_CHANNELS_PACKED;//CHANNELS_ID: 0x16
    uint32_t bits = 0;
    uint8_t bitsavailable = 0;
    for (int i=0; i<16; i++)
    {
        uint32_t val = dataVal[i];
        bits |= val << bitsavailable;
        bitsavailable += 11;
        while(bitsavailable >= 8) 
        {
            *buf++ = bits;
            bits >>= 8;
            bitsavailable -= 8;
        }
    }
    *buf++ = crc8(crc_start, 23);
}
