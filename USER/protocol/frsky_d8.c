#include "frsky_d8.h"
#include "cc2500.h"
#include "delay.h"
#include "function.h"
#include <stdlib.h>
#include "rgb.h"
#include "main.h"
#include "key.h"
#include "mixes.h"
#define FRSKYD8_PACKET_LEN  18
#define FRSKYD8_BINDCHANNEL 47 						//The 47th channel is fixed as a bound channel 

uint8_t  D8_Bind_flg = 0 ; 
uint16_t FRSKYD8_BindCounts = 0; 						
uint8_t  FRSKYD8_Counts = 0;
uint8_t	 FRSKYD8_ChannelShip = 0;       				
uint8_t  FRSKYD8_ctr = 0 ; 							
uint8_t  FRSKYD8_CountsRst = 0; 						
uint8_t  FRSKYD8_HOPChannel[50] ; 						
uint8_t  FRSKYD8_calData[50];							
uint8_t  FRSKYD8_Channel_Num = 0   ; 					
uint8_t  D8_SendPacket[18] ; 							    

static uint16_t TransmitterID ; 							    //遥控器唯一ID
static uint8_t RF_POWER = 0xFF;

static uint16_t Channel_DataBuff[8] = {1500,1500,1500,1500,1500,1500,1500,1500};
typedef enum 
{
	FRSKYD8_BIND  		    = 0x00 , 
	//FRSKYD16_BIND_PASSBACK	= 0x01 , 
	FRSKYD8_DATA  		    = 0x02 ,	
    //FRSKYD16_TUNE  		    = 0x03 ,
}FRSKYD8PhaseTypeDef ;

FRSKYD8PhaseTypeDef FRSKYD8Phase = FRSKYD8_DATA ; 

//FRSKYD16 Channel order
const uint8_t  FRSKYD8_CH_Code[8] = {AILERON, ELEVATOR, THROTTLE, RUDDER, AUX1, AUX2, AUX3, AUX4};


//==============================================================================
//			FRSKYD8 初始化器件地址
//==============================================================================
static void __attribute__((unused)) FRSKYD8_InitDeviceAddr(uint8_t IsBindFlg)
{
	CC2500_WriteReg(CC2500_18_MCSM0,    0x08) ;	
	CC2500_WriteReg(CC2500_09_ADDR , IsBindFlg ? 0x03 : (TransmitterID >> 8)&0xFF);
	CC2500_WriteReg(CC2500_07_PKTCTRL1,0x05);
}

//==============================================================================
//			FRSKYD8 设置发送通道
//==============================================================================
static void __attribute__((unused)) FRSKYD8_TuneChannel(uint8_t Channel)
{
	CC2500_Strobe(CC2500_SIDLE);						                //进入闲置状态
	CC2500_WriteReg(CC2500_25_FSCAL1, FRSKYD8_calData[Channel]);		//设置发送通道
	CC2500_WriteReg(CC2500_0A_CHANNR, FRSKYD8_HOPChannel[Channel]);	    //设置发送通道
	CC2500_Strobe(CC2500_SCAL);						                    //校准频率合成器并关闭
}

uint16_t convert_channel_frsky(uint8_t num)
{
	uint16_t val=Channel_DataBuff[num];
	//return ((val*15)>>4)+1321;
     val =(uint16_t)(((val - 860) * 3) >> 1) + 64+1226;
    return val;
}
/*--------------------------------------------------------------
					frequency hopping 
---------------------------------------------------------------*/
static void __attribute__((unused)) FRSKYD8_tune_chan_fast(void)
{
	CC2500_Strobe(CC2500_SIDLE);
	CC2500_WriteReg(CC2500_0D_FREQ2 , Fre_Carrier_H);
	CC2500_WriteReg(CC2500_0E_FREQ1 , Fre_Carrier_M);
	CC2500_WriteReg(CC2500_0F_FREQ0 , Fre_Carrier_L);
	Delay_US(2);
	CC2500_Strobe(CC2500_SIDLE);
	CC2500_WriteReg(CC2500_0D_FREQ2 , Fre_Carrier_H);
	CC2500_WriteReg(CC2500_0E_FREQ1 , Fre_Carrier_M);
	CC2500_WriteReg(CC2500_0F_FREQ0 , Fre_Carrier_L);
	CC2500_WriteReg(CC2500_25_FSCAL1, FRSKYD8_calData[FRSKYD8_Counts%47]);
	CC2500_WriteReg(CC2500_0A_CHANNR, FRSKYD8_HOPChannel[FRSKYD8_Counts%47]);
}

/*--------------------------------------------------------------------------------
						 build bind packet
---------------------------------------------------------------------------------*/
static void __attribute__((unused)) Frsky_D8_build_Bind_packet(void)
{
		//固定码

	D8_SendPacket[0] = 0x11;
	D8_SendPacket[1] = 0x03;
	D8_SendPacket[2] = 0x01;
	//遥控器ID
	D8_SendPacket[3] = (TransmitterID >> 8) & 0xFF  ;
	D8_SendPacket[4] = TransmitterID & 0xFF ;
	
	uint8_t  idx 	= (FRSKYD8_BindCounts % 10) * 5 ;
	D8_SendPacket[5]   = idx;	
	D8_SendPacket[6]   = FRSKYD8_HOPChannel[idx++];
	D8_SendPacket[7]   = FRSKYD8_HOPChannel[idx++];
	D8_SendPacket[8]   = FRSKYD8_HOPChannel[idx++];
	D8_SendPacket[9]   = FRSKYD8_HOPChannel[idx++];
	D8_SendPacket[10]  = FRSKYD8_HOPChannel[idx++];
	D8_SendPacket[11]  = 0x00;
	D8_SendPacket[12]  = 0x00; 
	D8_SendPacket[13] 	= 0x00;
	D8_SendPacket[14] 	= 0x00;
	D8_SendPacket[15] 	= 0x00;
	D8_SendPacket[16] 	= 0x00;
	D8_SendPacket[17] 	= 0x00;	
}

/*---------------------------------------------------------------------
			build control data package					
----------------------------------------------------------------------*/
void  __attribute__((unused)) FRSKYD8_build_Data_packet(uint16_t* d8ControlData)
{
	//固定码 + 遥控ID
	D8_SendPacket[0] = 0x11;

	//telemetry radio ID
	D8_SendPacket[1] = (TransmitterID >> 8) & 0xFF  ;
	D8_SendPacket[2] = TransmitterID & 0xFF ;           
	D8_SendPacket[3] = FRSKYD8_Counts;
	D8_SendPacket[4] = 0x00;
	D8_SendPacket[5] = 0x01;
	
	D8_SendPacket[10] = 0;
	D8_SendPacket[11] = 0;
	D8_SendPacket[16] = 0;
	D8_SendPacket[17] = 0;
	

    Channel_DataBuff[0] = d8ControlData[MIX_AILERON];
    Channel_DataBuff[1] = d8ControlData[MIX_ELEVATOR];
    Channel_DataBuff[2] = d8ControlData[MIX_THROTTLE];
    Channel_DataBuff[3] = d8ControlData[MIX_RUDDER];

	Channel_DataBuff[4] = d8ControlData[4];
    Channel_DataBuff[5] = d8ControlData[5];
    Channel_DataBuff[6] = d8ControlData[6];
    Channel_DataBuff[7] = d8ControlData[7];
    
    
	for(uint8_t i=0; i < 8; i++)
	{
		//uint16_t value = Channel_DataBuff[i];// + (uint16_t)((float)Channel_DataBuff[i]/2+0.5f);
		uint16_t value;
		value = convert_channel_frsky(i);
		if(i < 4)
		{
			D8_SendPacket[6+i] = value & 0xff;
			D8_SendPacket[10+(i>>1)] |= ((value >> 8) & 0x0f) << (4 *(i & 0x01));
		}
		else
		{
			D8_SendPacket[8+i] = value & 0xff;
			D8_SendPacket[16+((i-4)>>1)] |= ((value >> 8) & 0x0f) << (4 * ((i-4) & 0x01));
		}
	}
}

//==============================================================================
//FRSKYD16 : 计算 FRSKYD16 通道(通过计算得到 47 个通道 。轮询时，在这47个通道间跳频)
//相邻两频段间隔在 5 以上
// 1  - 26  : 取 16 个频点
// 27 - 52  : 取 15 个频点
// 53 - 76  : 取 16 个频点
//==============================================================================
void Calc_FRSKYD8_Channel()
{
  uint8_t val;
  uint8_t channel = TransmitterID&0x07;
  uint8_t channel_spacing = TransmitterID&0xFF;
  //Filter bad tables
  if(channel_spacing<0x02) channel_spacing+=0x02;
  if(channel_spacing>0xE9) channel_spacing-=0xE7;
  if(channel_spacing%0x2F==0) channel_spacing++;
    
  for(uint8_t i=0;i<50;i++)
  {
    channel=(channel+channel_spacing) % 0xEB;
    val=channel;
    if((val==0x00) || (val==0x5A) || (val==0xDC))
      val++;
    FRSKYD8_HOPChannel[i]=i>46?0:val;
  }
//	uint8_t  idx = 0;
//	uint16_t id_tmp = ~ TransmitterID; 					          
//	
//	while(idx < 47)
//	{
//		uint8_t i;
//		uint8_t count_1_26 = 0, count_27_52 = 0, count_53_76 = 0;
//		id_tmp = id_tmp * 0x0019660D + 0x3C6EF35F;			        // Randomization
//		uint8_t next_ch = ((id_tmp >> 8) % 0x4B) + 1;			    // Use least-significant byte and must be larger than 1
//		for (i = 0; i < idx; i++)
//		{
//			if(FRSKYD8_HOPChannel[i] == next_ch)    	
//			{
//				break;
//			}
//			if(FRSKYD8_HOPChannel[i] < 27) 		
//			{
//				count_1_26++;
//			}
//			else if(FRSKYD8_HOPChannel[i] < 53)  		
//			{
//				count_27_52++;
//			}
//			else 
//			{				
//				count_53_76++;
//			}
//		}
//		if (i != idx)  
//		{
//			continue;				
//		}
//		
//		if(idx)
//		{
//			uint8_t Temp = 0 ; 
//			if(next_ch > FRSKYD8_HOPChannel[idx - 1]) 	
//			{
//				Temp = next_ch - FRSKYD8_HOPChannel[idx - 1] ;
//			}				
//			else 						
//			{
//				Temp = FRSKYD8_HOPChannel[idx - 1] - next_ch ;
//			}				
//		  	if(Temp < 5)
//			{
//				continue ;
//			}				
//		}
//		
//		if(next_ch == 0)  break; 
//		
//		//get frequency point
//		if (((next_ch < 27) && (count_1_26 < 16)) || ((next_ch >= 27) && (next_ch < 53) && (count_27_52 < 15)) || ((next_ch >= 53) && (count_53_76 < 16)))
//		{
//			FRSKYD8_HOPChannel[idx++] = next_ch;
//		}
//	}
//	
//	FRSKYD8_HOPChannel[FRSKYD8_BindCounts] = 0 ;                  //Band band binding is 0
    
}




uint16_t ReadFRSKYD8(uint16_t* controlData)
{
	switch(FRSKYD8Phase)
	{
		//send bind data
		case FRSKYD8_BIND : 
            if(FRSKYD8_BindCounts < 600)
			{
				FRSKYD8_TuneChannel(FRSKYD8_BINDCHANNEL) ; 
				CC2500_SetPower(RF_POWER);
				CC2500_Strobe(CC2500_SFRX);
				Frsky_D8_build_Bind_packet();
				CC2500_Strobe(CC2500_SIDLE);
				CC2500_Strobe(CC2500_SFTX);
				CC2500_WriteData(D8_SendPacket, FRSKYD8_PACKET_LEN);
				++FRSKYD8_BindCounts ; 
				
			}  
			else
			{
			  	D8_Bind_flg = 0 ; 
				FRSKYD8_BindCounts = 0 ; 
				FRSKYD8_Counts = 0 ; 
				CC2500_SetPower(RF_POWER);           //设置发送功率
				FRSKYD8_InitDeviceAddr(D8_Bind_flg) ;	
				FRSKYD8Phase = FRSKYD8_DATA ; 	
			}
			return 8830 ;
		// Frsky D16 data
		case FRSKYD8_DATA :
			FRSKYD8_Counts = (FRSKYD8_Counts + 1) % 188;
			FRSKYD8_tune_chan_fast();
			FRSKYD8_build_Data_packet(controlData);
			CC2500_Strobe(CC2500_SIDLE);
			CC2500_WriteData(D8_SendPacket, FRSKYD8_PACKET_LEN);	
			return 8830 ;  
	  
	}
	return 9000 ; 
}

void D8_SetBind(void)
{
  	FRSKYD8_BindCounts = 0 ; 
	FRSKYD8Phase = FRSKYD8_BIND ;
	FRSKYD8_TuneChannel(FRSKYD8_HOPChannel[FRSKYD8_BINDCHANNEL]) ; 
}


void initFRSKYD8(uint8_t protocolIndex)
{
    uint8_t CC2500_Error_flg = 0;
    
  	//get chip ID
  	TransmitterID = GetUniqueID();
	//Get the frequency hopping by chip ID
	Calc_FRSKYD8_Channel();
	
    srand(SysTick->VAL);
	FRSKYD8_ChannelShip = rand() % 46 + 1;  // Initialize it to random 0-47 inclusive
	while((FRSKYD8_ChannelShip - FRSKYD8_ctr) % 4) 
	{
		FRSKYD8_ctr = (FRSKYD8_ctr + 1) % 4 ;
	}
	FRSKYD8_CountsRst = (FRSKYD8_ChannelShip - FRSKYD8_ctr) >> 2 ; 
	
	CC2500_Error_flg = CC2500_Init(protocolIndex); 
	if(CC2500_Error_flg == 1)
	{
		//Initialization failed
	}
	else
	{
		// calibration frequency
		for (uint8_t i = 0 ; i < 48 ; i++)
		{
			CC2500_Strobe(CC2500_SIDLE);
			CC2500_WriteReg(CC2500_0A_CHANNR , FRSKYD8_HOPChannel[i]);
			CC2500_Strobe(CC2500_SCAL);
			HAL_Delay(1);
			FRSKYD8_calData[i]  =  CC2500_ReadReg(CC2500_25_FSCAL1);
		}
		FRSKYD8_InitDeviceAddr(D8_Bind_flg);
		CC2500_SetPower(RF_POWER);		
		FRSKYD8Phase = FRSKYD8_DATA ;
		FRSKYD8_TuneChannel(FRSKYD8_HOPChannel[0]); 
	}
}



