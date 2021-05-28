#include "crsf.h"
#include "usart.h"
#include "function.h"

uint8_t CRSF_Packet[26] = {0x0F, 0x00, 0x34, 0x1F, 0xA8, 0x09, 0x08, 0x6A, 0x50, 0x03,0x10, 0x80, 0x00,
                             0x04, 0x20, 0x00, 0x01, 0x08, 0x07, 0x38, 0x00, 0x10, 0x80, 0x00, 0x04,0x00};

static uint16_t ChannelDataBuff[16] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};
static GimbalReverseTypeDef gimbalReverseFlg;//摇杆输出反向标志 0：不反向 1：反向
uint16_t ControlDataBuff[8] = {0};
void CRSF_SetBind()
{
    HAL_Delay(1);
}

void CRSF_Init(uint8_t protocol_Index)
{
    HAL_Delay(1);
}

uint16_t CRSF_Process(uint16_t* control_data)
{
    
    Get_CRSFPackage(CRSF_Packet,control_data);
    DMA1_Channel4->CCR &= ~( 1 << 0 );
    DMA1_Channel4->CNDTR = 26 ;//send DataNumber
    DMA1_Channel4->CCR |= 1 << 0 ;// ENABLERC_DMA_TxCh
    HAL_UART_Transmit_DMA(&huart1,CRSF_Packet,26);//启动传输 
	return 0; 
}


void Get_CRSFPackage(uint8_t* ChannelToCRSF,uint16_t* ControlDataBuff)
{
    uint16_t SWA_Temp;
	uint16_t SWB_Temp;
	uint16_t SWC_Temp;
	uint16_t SWD_Temp;
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


    ChannelDataBuff[0] =(gimbalReverseFlg.ELEVATOR == 1)?(2*CHANNEL_OUTPUT_MID - ControlDataBuff[ELEVATOR]):ControlDataBuff[ELEVATOR];
    ChannelDataBuff[1] =(gimbalReverseFlg.AILERON  == 1)?(2*CHANNEL_OUTPUT_MID - ControlDataBuff[AILERON]) :ControlDataBuff[AILERON];
    ChannelDataBuff[2] =(gimbalReverseFlg.THROTTLE == 1)?(2*CHANNEL_OUTPUT_MID - ControlDataBuff[THROTTLE]):ControlDataBuff[THROTTLE];
    ChannelDataBuff[3] =(gimbalReverseFlg.RUDDER   == 1)?(2*CHANNEL_OUTPUT_MID - ControlDataBuff[RUDDER])  :ControlDataBuff[RUDDER];

	ChannelDataBuff[4] = ControlDataBuff[4];
    ChannelDataBuff[5] = ControlDataBuff[5];
    ChannelDataBuff[6] = ControlDataBuff[6];
    ChannelDataBuff[7] = ControlDataBuff[7];
    
	SWA_Temp = map(ChannelDataBuff[4],993,2000,163,1811);
	SWB_Temp = map(ChannelDataBuff[5],993,2000,163,1811);
	SWC_Temp = map(ChannelDataBuff[6],993,2000,163,1811);
	SWD_Temp = map(ChannelDataBuff[7],993,2000,163,1811);  
    
  uint32_t data_channel[16];
  data_channel[0] = ChannelDataBuff[AILERON] ;
  data_channel[1] = ChannelDataBuff[ELEVATOR];
  data_channel[2] = ChannelDataBuff[THROTTLE];
  data_channel[3] = ChannelDataBuff[RUDDER];
  data_channel[4] = SWA_Temp;
  data_channel[5] = SWB_Temp;
  data_channel[6] = SWC_Temp;
  data_channel[7] = SWD_Temp;
  data_channel[8] = DEFAULT_VALUE;
  data_channel[9] = DEFAULT_VALUE;
  data_channel[10] = DEFAULT_VALUE;    
  data_channel[11] = DEFAULT_VALUE;  
  data_channel[12] = DEFAULT_VALUE;
  data_channel[13] = DEFAULT_VALUE;
  data_channel[14] = DEFAULT_VALUE;
  data_channel[15] = DEFAULT_VALUE;    
  
  uint8_t *buf = ChannelToCRSF;
  *buf++ = 0xEE; //MODULE ADDRESS: 0xEE
  *buf++ = 24; // 1(ID) + 22 + 1(CRC)
  uint8_t * crc_start = buf;
  *buf++ = 0x16;//CHANNELS_ID: 0x16
  uint32_t bits = 0;
  uint8_t bitsavailable = 0;
    for (int i=0; i<16; i++)
    {
        uint32_t val = data_channel[i];
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
