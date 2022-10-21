#include "frsky_d16.h"
#include "cc2500.h"
#include "delay.h"
#include "function.h"
#include <stdlib.h>
#include "adc.h"
#include "key.h"
#include "rgb.h"
#include "main.h"
#include "gimbal.h"
#include "switches.h"
#include "mixes.h"

TaskHandle_t frskyd16TaskHandle;

#ifdef LBT
	#define FRSKYD16_PACKET_LEN  33
#else
	#define FRSKYD16_PACKET_LEN  30
#endif
#define FRSKYD16_BINDCHANNEL 47 						//The 47th channel is fixed as a bound channel 

uint8_t  Bind_flg = 0 ; 
uint16_t FRSKYD16_BindCounts = 0; 						// 对码数据包发送个数

uint8_t	 FRSKYD16_ChannelShip = 0;       				// 跳频间隔(前后两次频段间隔)
uint8_t  FRSKYD16_ctr = 0 ; 							
uint8_t  FRSKYD16_CountsRst = 0; 						
uint8_t  FRSKYD16_HOPChannel[50] ; 						// 跳频列表(根据遥控器ID计算出47个跳频号(后三个频点无效))	
uint8_t  FRSKYD16_calData[50];							// 记录跳频通道频率值

uint8_t  FRSKYD16_Channel_Num = 0   ; 					// 跳频通道号

uint8_t highThrottleFlg = 1 ; 							//高油门标志位
static uint16_t TransmitterID ; 							    //遥控器唯一ID
uint8_t  SendPacket[40] ; 							    //发送数据包缓存 (1) 对码数据包14Byte   (2)发送遥控数据包 28Byte(8 + 16CH*2 = 40)



static uint8_t versionSelectFlg;

static uint16_t Channel_DataBuff[16] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};

typedef enum 
{
	FRSKYD16_BIND  		    = 0x00 , 
	//FRSKYD16_BIND_PASSBACK	= 0x01 , 
	FRSKYD16_DATA  		    = 0x02 ,	
    //FRSKYD16_TUNE  		    = 0x03 ,
}FRSKYD16PhaseTypeDef ;

FRSKYD16PhaseTypeDef FRSKYD16Phase = FRSKYD16_DATA ; 

//FRSKYD16 Channel order
const uint8_t  FRSKYD16_CH_Code[16] = {AILERON, ELEVATOR, THROTTLE, RUDDER, AUX1, AUX2, AUX3, AUX4, AUX5, AUX6, AUX7, AUX8, AUX9, AUX10, AUX11, AUX12};


//==============================================================================
//			FRSKYD16 初始化器件地址
//==============================================================================
static void __attribute__((unused)) FRSKYD16_InitDeviceAddr(uint8_t IsBindFlg)
{
	CC2500_WriteReg(CC2500_0C_FSCTRL0 , 0x00); 
	CC2500_WriteReg(CC2500_18_MCSM0,    0x08) ;	
	CC2500_WriteReg(CC2500_09_ADDR , IsBindFlg ? 0x03 : (TransmitterID & 0xFF));
	CC2500_WriteReg(CC2500_07_PKTCTRL1,0x05);
}

//==============================================================================
//			FRSKYD16 设置发送通道
//==============================================================================
static void __attribute__((unused)) FRSKYD16_TuneChannel(uint8_t Channel)
{
	CC2500_Strobe(CC2500_SIDLE);						//进入闲置状态
	CC2500_WriteReg(CC2500_25_FSCAL1, FRSKYD16_calData[Channel]);		//设置发送通道
	CC2500_WriteReg(CC2500_0A_CHANNR, FRSKYD16_HOPChannel[Channel]);	//设置发送通道
	//CC2500_Strobe(CC2500_SCAL);						//校准频率合成器并关闭
	Delay_US(20);
}

/*-------------------------------------------------------------
							CRC 
---------------------------------------------------------------*/
static const uint16_t CRC_Short[16] = 	//CRC
{
	0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
	0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7 
};

static uint16_t CRCTable(uint8_t val)
{
	uint16_t word ;
	word = CRC_Short[val&0x0F] ;
	val /= 16 ;
	return word ^ (0x1081 * val) ;
}

static uint16_t crc_x(uint8_t *data, uint8_t len)
{
	uint16_t crc = 0;
	for(uint8_t i=0; i < len; i++)
	{
		crc = (crc<<8) ^ CRCTable((uint8_t)(crc>>8) ^ *data++);
	}
	return crc;
}

//通道数据换算
static uint16_t  __attribute__((unused)) CalChannelData( uint8_t channel)
{	
  	if(channel > 15) channel = 15 ;
  	uint16_t DataTemp = Channel_DataBuff[channel]; 
	//DataTemp = (((DataTemp*15)>>4)+1320) - 1226;
     DataTemp =(uint16_t)(((DataTemp - 860) * 3) >> 1) + 64;
  	if(channel>7)
		DataTemp |= 2048;
	
	return DataTemp;		//mapped 0 ,2140(125%) range to 64,1984 ;
   
}


/*--------------------------------------------------------------
					frequency hopping 
---------------------------------------------------------------*/
static void __attribute__((unused)) FRSKYD16_tune_chan_fast(void)
{
	CC2500_Strobe(CC2500_SIDLE);
	CC2500_WriteReg(CC2500_25_FSCAL1, FRSKYD16_calData[FRSKYD16_Channel_Num]);
	CC2500_WriteReg(CC2500_0A_CHANNR, FRSKYD16_HOPChannel[FRSKYD16_Channel_Num]);
}


/*-------------------------------------------------------------------------------
	                      get next channel
--------------------------------------------------------------------------------*/
static void FRSKYD16_calc_next_chan(void)
{
    	FRSKYD16_Channel_Num = (FRSKYD16_Channel_Num + FRSKYD16_ChannelShip) % 47 ;
}

/*--------------------------------------------------------------------------------
						 build bind packet
---------------------------------------------------------------------------------*/
static void __attribute__((unused)) Frsky_D16_build_Bind_packet(void)
{
		//固定码
	if(versionSelectFlg == LBT)
	{
		SendPacket[0] = 0x20;
	}
	else
	{
		SendPacket[0] = 0x1D;
	}

	SendPacket[1] = 0x03;
	SendPacket[2] = 0x01;
	//遥控器ID
	SendPacket[3] = (TransmitterID >> 8) & 0xFF  ;
	SendPacket[4] = TransmitterID & 0xFF ;
	
	uint8_t  idx 	= (FRSKYD16_BindCounts % 10) * 5 ;
	SendPacket[5]   = idx;	
	SendPacket[6]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[7]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[8]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[9]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[10]  = FRSKYD16_HOPChannel[idx++];
	SendPacket[11]  = 0x02;
	SendPacket[12]  = 0x01; 
	
	//固定为0
	SendPacket[13] 	= 0x00;
	SendPacket[14] 	= 0x00;
	SendPacket[15] 	= 0x00;
	SendPacket[16] 	= 0x00;
	SendPacket[17] 	= 0x00;
	SendPacket[18] 	= 0x00;
	SendPacket[19] 	= 0x00;
	SendPacket[20] 	= 0x00;
	SendPacket[21] 	= 0x00;
	SendPacket[22] 	= 0x00;
	SendPacket[23] 	= 0x00;
	SendPacket[24] 	= 0x00;
	SendPacket[25] 	= 0x00;
	SendPacket[26] 	= 0x00;
	SendPacket[27] 	= 0x00;
	if(versionSelectFlg == LBT)
	{
		SendPacket[28] 	= 0x00;
		SendPacket[29] 	= 0x00;
		SendPacket[30] 	= 0x00;
		uint16_t lcrc = crc_x(&SendPacket[3], 28);
		SendPacket[31] 	= lcrc >> 8;;
		SendPacket[32] 	= lcrc;
	}
	else
	{
		//数据包校验和
		uint16_t lcrc = crc_x(&SendPacket[3], 25);
		
		SendPacket[28] = lcrc >> 8;
		SendPacket[29] = lcrc;
	}
}

/*---------------------------------------------------------------------
			build control data package					
----------------------------------------------------------------------*/
void  __attribute__((unused)) FRSKYD16_build_Data_packet(uint16_t* d16ControlData)
{
	static uint8_t lpass;
	uint16_t chan_0 ;
	uint16_t chan_1 ; 
	uint8_t startChan = 0;
	//sbus_checkrx();
	if(versionSelectFlg == LBT)
	{
		SendPacket[0] = 0x20;
	}
	else
	{
		SendPacket[0] = 0x1D;
	}
	//telemetry radio ID
	SendPacket[1]   = (TransmitterID >> 8) & 0xFF  ;
	SendPacket[2]   = TransmitterID & 0xFF ;           
	SendPacket[3] 	= 0x02;
	
	
	SendPacket[4] = (FRSKYD16_ctr<<6) + FRSKYD16_Channel_Num; 
	SendPacket[5] = FRSKYD16_CountsRst;
	SendPacket[6] = 0x01;
	
	
	if(FRSKYD16_Channel_Num == 0x21) 
	  FRSKYD16_Channel_Num = 0x21 ; 
	
	SendPacket[7] = 0;
	SendPacket[8] = 0;
	
	
	if (lpass & 1) 
	{
		startChan += 8;
	}
  
    Channel_DataBuff[0] = d16ControlData[MIX_AILERON];
    Channel_DataBuff[1] = d16ControlData[MIX_ELEVATOR];
    Channel_DataBuff[2] = d16ControlData[MIX_THROTTLE];
    Channel_DataBuff[3] = d16ControlData[MIX_RUDDER];

	Channel_DataBuff[4] = d16ControlData[4];
    Channel_DataBuff[5] = d16ControlData[5];
    Channel_DataBuff[6] = d16ControlData[6];
    Channel_DataBuff[7] = d16ControlData[7];

	//add Channel data
	for(uint8_t i = 0; i <12 ; i+=3)
	{
        
        chan_0 = CalChannelData(startChan);			
		startChan++ ;
	
		chan_1 = CalChannelData(startChan);				
		startChan++;
		
		SendPacket[9+i]   = (chan_0 & 0xFF); 
		SendPacket[9+i+1] = (((chan_0>>8) & 0x0F)|(chan_1 << 4));
		SendPacket[9+i+2] = chan_1 >> 4;
	}
 
	SendPacket[21] = 0x08 ; 
	//下一包数据 发送 后 8 通
	lpass += 1 ;
	if(versionSelectFlg == LBT)
	{
		for (uint8_t i=22;i<31;i++)
		{
			SendPacket[i]=0;
		}
		uint16_t lcrc = crc_x(&SendPacket[3], 28);
		SendPacket[31]=lcrc>>8;   //high byte
		SendPacket[32]=lcrc;      //low byte
	}
	else
	{
		for (uint8_t i=22;i<28;i++)
		{
			SendPacket[i]=0;
		}
		uint16_t lcrc = crc_x(&SendPacket[3], 25);
		SendPacket[28]=lcrc>>8;   //high byte
		SendPacket[29]=lcrc;      //low byte
	}
}


void Frsky_init_hop(void)
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
    FRSKYD16_HOPChannel[i]=i>46?0:val;
  }

}

//==============================================================================
//FRSKYD16 : 计算 FRSKYD16 通道(通过计算得到 47 个通道 。轮询时，在这47个通道间跳频)
//相邻两频段间隔在 5 以上
// 1  - 26  : 取 16 个频点
// 27 - 52  : 取 15 个频点
// 53 - 76  : 取 16 个频点
//==============================================================================
void Calc_FRSKYD16_Channel()
{
	uint8_t  idx = 0;
	uint16_t id_tmp = ~ TransmitterID; 					          
	
	while(idx < 47)
	{
		uint8_t i;
		uint8_t count_1_26 = 0, count_27_52 = 0, count_53_76 = 0;
		id_tmp = id_tmp * 0x0019660D + 0x3C6EF35F;			        // Randomization
		uint8_t next_ch = ((id_tmp >> 8) % 0x4B) + 1;			    // Use least-significant byte and must be larger than 1
		for (i = 0; i < idx; i++)
		{
			if(FRSKYD16_HOPChannel[i] == next_ch)    	
			{
				break;
			}
			if(FRSKYD16_HOPChannel[i] < 27) 		
			{
				count_1_26++;
			}
			else if(FRSKYD16_HOPChannel[i] < 53)  		
			{
				count_27_52++;
			}
			else 
			{				
				count_53_76++;
			}
		}
		if (i != idx)  
		{
			continue;				
		}
		
		if(idx)
		{
			uint8_t Temp = 0 ; 
			if(next_ch > FRSKYD16_HOPChannel[idx - 1]) 	
			{
				Temp = next_ch - FRSKYD16_HOPChannel[idx - 1] ;
			}				
			else 						
			{
				Temp = FRSKYD16_HOPChannel[idx - 1] - next_ch ;
			}				
		  	if(Temp < 5)
			{
				continue ;
			}				
		}
		
		if(next_ch == 0)  break; 
		
		//get frequency point
		if (((next_ch < 27) && (count_1_26 < 16)) || ((next_ch >= 27) && (next_ch < 53) && (count_27_52 < 15)) || ((next_ch >= 53) && (count_53_76 < 16)))
		{
			FRSKYD16_HOPChannel[idx++] = next_ch;
		}
	}
	
	FRSKYD16_HOPChannel[FRSKYD16_BINDCHANNEL] = 0 ;                  //Band band binding is 0
}


uint16_t ReadFRSKYD16(uint16_t* controlData)
{
	switch(FRSKYD16Phase)
	{
		//send bind data
		case FRSKYD16_BIND :
         //   Set_Bind_Flg();
		  	if(FRSKYD16_BindCounts < 600)
			{    
                ++FRSKYD16_BindCounts ;    
				FRSKYD16_TuneChannel(FRSKYD16_BINDCHANNEL) ; 
				CC2500_SetPower(RF_POWER);
				CC2500_Strobe(CC2500_SFRX);
				Frsky_D16_build_Bind_packet();
				Delay_US(19);
				CC2500_Strobe(CC2500_SIDLE);
				CC2500_WriteData(SendPacket, SendPacket[0] + 1);
			}  
			else
			{
			  	Bind_flg = 0 ; 
				FRSKYD16_BindCounts = 0 ; 
				FRSKYD16_Channel_Num = 0 ; 
				//FRSKYD16_InitDeviceAddr(Bind_flg) ;	
				CC2500_SetPower(RF_POWER);
				FRSKYD16Phase = FRSKYD16_DATA ; 
			}
		break;
		// Frsky D16 data
		case FRSKYD16_DATA :
         //   Reset_Bind_Flg();
		  	FRSKYD16_calc_next_chan();
			FRSKYD16_tune_chan_fast();
			FRSKYD16_build_Data_packet(controlData);
			CC2500_Strobe(CC2500_SIDLE);	
			CC2500_WriteData(SendPacket, FRSKYD16_PACKET_LEN);
		break;  
	  
	}
	return 9000 ; 
}

void SetBind(void)
{
  	FRSKYD16_BindCounts = 0 ; 
	FRSKYD16Phase = FRSKYD16_BIND ;
	FRSKYD16_TuneChannel(FRSKYD16_HOPChannel[FRSKYD16_BINDCHANNEL]) ; 
}


void FRSKYD16_Init(uint8_t protocolIndex)
{
    versionSelectFlg = protocolIndex;
  	uint8_t CC2500_Error_flg = 0;
    //get chip ID
  	TransmitterID = GetUniqueID();
	//Get the frequency hopping by chip ID
	//Calc_FRSKYD16_Channel();
	Frsky_init_hop();
    srand(SysTick->VAL);
	FRSKYD16_ChannelShip = rand() % 46 + 1;  // Initialize it to random 0-47 inclusive
	while((FRSKYD16_ChannelShip - FRSKYD16_ctr) % 4) 
	{
		FRSKYD16_ctr = (FRSKYD16_ctr + 1) % 4 ;
	}
	FRSKYD16_CountsRst = (FRSKYD16_ChannelShip - FRSKYD16_ctr) >> 2 ; 
	
	CC2500_Error_flg = CC2500_Init(protocolIndex) ; 
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
			CC2500_WriteReg(CC2500_0A_CHANNR , FRSKYD16_HOPChannel[i]);
			CC2500_Strobe(CC2500_SCAL);
			HAL_Delay(1);
			FRSKYD16_calData[i]  =  CC2500_ReadReg(CC2500_25_FSCAL1);
		}
		CC2500_SetPower(RF_POWER);	
		FRSKYD16Phase = FRSKYD16_DATA ;
		FRSKYD16_TuneChannel(FRSKYD16_HOPChannel[FRSKYD16_Channel_Num]) ; 
	}
}

