#include "crsf.h"
#include "usart.h"
#include "function.h"
#include "buzzer.h"
#include "mixes.h"
uint8_t crsfPacket[26] = {0x0F, 0x00, 0x34, 0x1F, 0xA8, 0x09, 0x08, 0x6A, 0x50, 0x03,0x10, 0x80, 0x00,
                             0x04, 0x20, 0x00, 0x01, 0x08, 0x07, 0x38, 0x00, 0x10, 0x80, 0x00, 0x04,0x00};
uint8_t outBuffer[LinkStatisticsFrameLength + 4] = {0};
static uint16_t channelDataBuff[16] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};
uint8_t crsfRSSI;

uint16_t controlDataBuff[8] = {0};
uint8_t crsfLinkCount= 0;
void CRSF_SetBind()
{
    HAL_Delay(1);
}

void CRSF_Init(uint8_t protocolIndex)
{
    HAL_Delay(1);
}

uint16_t CRSF_Process(uint16_t* crsfcontrol_data)
{

    Get_CRSFPackage(crsfPacket,crsfcontrol_data);
    HAL_UART_Transmit_DMA(&huart1,crsfPacket,26);
    if(crsfRSSI<98 && crsfLinkCount>10)
    {
        xEventGroupSetBits(buzzerEventHandle,RISS_WARNING_RING);
    }
	return 0; 
}

void Get_LinkStatis(uint8_t* crsfRXPacket)
{
    
    if(crsfRXPacket[0] == CRSF_ADDRESS_RADIO_TRANSMITTER)
    {
        if(crsfRXPacket[1] == (LinkStatisticsFrameLength + 2))
        {
            if(crsfRXPacket[2] == CRSF_FRAMETYPE_LINK_STATISTICS)
            {
//                uint8_t crc = crc8(&CRSF_RXPacket[2], LinkStatisticsFrameLength + 1);
//                if(crc == CRSF_RXPacket[LinkStatisticsFrameLength + 3])
//                {
                    crsfRSSI = crsfRXPacket[5];
//                }
                crsfLinkCount =255;              
            }
        }
 
    }
    else
    {
        if(crsfLinkCount > 0)
        {
            crsfLinkCount --;
        }
    }
}

uint8_t Get_RSSI(void)
{
    return crsfRSSI;
}

void Get_CRSFPackage(uint8_t* channelToCRSF,uint16_t* controlDataBuff)
{
    uint16_t SWA_Temp;
	uint16_t SWB_Temp;
	uint16_t SWC_Temp;
	uint16_t SWD_Temp;
    uint32_t dataVal[16];
      
    channelDataBuff[MIX_ELEVATOR] = map(controlDataBuff[MIX_ELEVATOR],1000,2000,192,1792);
    channelDataBuff[MIX_AILERON] = map(controlDataBuff[MIX_AILERON],1000,2000,192,1792);
    channelDataBuff[MIX_THROTTLE] = map(controlDataBuff[MIX_THROTTLE],1000,2000,192,1792);
    channelDataBuff[MIX_RUDDER] = map(controlDataBuff[MIX_RUDDER],1000,2000,192,1792);
	channelDataBuff[4] = controlDataBuff[4];
    channelDataBuff[5] = controlDataBuff[5];
    channelDataBuff[6] = controlDataBuff[6];
    channelDataBuff[7] = controlDataBuff[7];
    
	SWA_Temp = map(channelDataBuff[4],1000,2000,192,1792);
	SWB_Temp = map(channelDataBuff[5],1000,2000,192,1792);
	SWC_Temp = map(channelDataBuff[6],1000,2000,192,1792);
	SWD_Temp = map(channelDataBuff[7],1000,2000,192,1792);  


    dataVal[0] = channelDataBuff[MIX_AILERON] ;
    dataVal[1] = channelDataBuff[MIX_ELEVATOR];
    dataVal[2] = channelDataBuff[MIX_THROTTLE];
    dataVal[3] = channelDataBuff[MIX_RUDDER];
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
