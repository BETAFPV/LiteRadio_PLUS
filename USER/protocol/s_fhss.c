#include "s_fhss.h"
#include "cc2500.h"
#include <stdlib.h>
#include "function.h"
#include "delay.h"
#include "spi.h"
#include "gimbal.h"
#include "switches.h"
#include "mixes.h"
#define SFHSS_COARSE	0

#define SFHSS_PACKET_LEN 13
#define SFHSS_TX_ID_LEN   2

uint32_t MProtocol_id;//tx id,
uint8_t	fhss_code=0; // 0-27
uint8_t  rx_tx_addr[5];
uint8_t  rf_ch_num;
uint8_t  calData[50];
uint16_t counter;
uint8_t  packet[40] ; 	
uint8_t  phase;
const uint8_t CH_AETR[]={AILERON, ELEVATOR, THROTTLE, RUDDER, CH5, CH6, CH7, CH8, CH9, CH10, CH11, CH12, CH13, CH14, CH15, CH16};
enum {
    SFHSS_START = 0x00,
    SFHSS_CAL   = 0x01,
    SFHSS_DATA1 = 0x02,
    SFHSS_DATA2 = 0x03,
    SFHSS_TUNE  = 0x04
};

#define SFHSS_FREQ0_VAL 0xC4
static uint16_t Channel_DataBuff[16];
// Some important initialization parameters, all others are either default,
// or not important in the context of transmitter
// IOCFG2   2F - GDO2_INV=0 GDO2_CFG=2F - HW0
// IOCFG1   2E - GDO1_INV=0 GDO1_CFG=2E - High Impedance
// IOCFG0   2F - GDO0 same as GDO2, TEMP_SENSOR_ENABLE=off
// FIFOTHR  07 - 33 decimal TX threshold
// SYNC1    D3
// SYNC0    91
// PKTLEN   0D - Packet length, 0D bytes
// PKTCTRL1 04 - APPEND_STATUS on, all other are receive parameters - irrelevant
// PKTCTRL0 0C - No whitening, use FIFO, CC2400 compatibility on, use CRC, fixed packet length
// ADDR     29
// CHANNR   10
// FSCTRL1  06 - IF 152343.75Hz, see page 65
// FSCTRL0  00 - zero freq offset
// FREQ2    5C - synthesizer frequency 2399999633Hz for 26MHz crystal, ibid
// FREQ1    4E
// FREQ0    C4
// MDMCFG4  7C - CHANBW_E - 01, CHANBW_M - 03, DRATE_E - 0C. Filter bandwidth = 232142Hz
// MDMCFG3  43 - DRATE_M - 43. Data rate = 128143bps
// MDMCFG2  83 - disable DC blocking, 2-FSK, no Manchester code, 15/16 sync bits detected (irrelevant for TX)
// MDMCFG1  23 - no FEC, 4 preamble bytes, CHANSPC_E - 03
// MDMCFG0  3B - CHANSPC_M - 3B. Channel spacing = 249938Hz (each 6th channel used, resulting in spacing of 1499628Hz)
// DEVIATN  44 - DEVIATION_E - 04, DEVIATION_M - 04. Deviation = 38085.9Hz
// MCSM2    07 - receive parameters, default, irrelevant
// MCSM1    0C - no CCA (transmit always), when packet received stay in RX, when sent go to IDLE
// MCSM0    08 - no autocalibration, PO_TIMEOUT - 64, no pin radio control, no forcing XTAL to stay in SLEEP
// FOCCFG   1D - not interesting, Frequency Offset Compensation
// FREND0   10 - PA_POWER = 0

const uint8_t SFHSS_init_values[] = {
  /* 00 */ 0x2F, 0x2E, 0x2F, 0x07, 0xD3, 0x91, 0x0D, 0x04,
  /* 08 */ 0x0C, 0x29, 0x10, 0x06, 0x00, 0x5C, 0x4E, SFHSS_FREQ0_VAL + SFHSS_COARSE,
  /* 10 */ 0x7C, 0x43, 0x83, 0x23, 0x3B, 0x44, 0x07, 0x0C,
  /* 18 */ 0x08, 0x1D, 0x1C, 0x43, 0x40, 0x91, 0x57, 0x6B,
  /* 20 */ 0xF8, 0xB6, 0x10, 0xEA, 0x0A, 0x11, 0x11
};

static void __attribute__((unused)) SFHSS_tune_chan()
{
	CC2500_Strobe(CC2500_SIDLE);
	CC2500_WriteReg(CC2500_0A_CHANNR, rf_ch_num*6+16);
	CC2500_Strobe(CC2500_SCAL);
}

static void __attribute__((unused)) SFHSS_tune_freq()
{
	
//		CC2500_WriteReg(CC2500_0C_FSCTRL0, 0x0A);
    CC2500_WriteReg(CC2500_0C_FSCTRL0, 0x0A);
		CC2500_WriteReg(CC2500_0F_FREQ0, SFHSS_FREQ0_VAL + SFHSS_COARSE);
	//	phase = SFHSS_START;	// Restart the tune process if option is changed to get good tuned values
}

static void __attribute__((unused)) SFHSS_tune_chan_fast()
{
	CC2500_Strobe(CC2500_SIDLE);
	CC2500_WriteReg(CC2500_0A_CHANNR, rf_ch_num*6+16);
	CC2500_WriteReg(CC2500_25_FSCAL1, calData[rf_ch_num]);
}

static void __attribute__((unused)) SFHSS_calc_next_chan()
{
    rf_ch_num += fhss_code + 2;
    if (rf_ch_num > 29)
	{
        if (rf_ch_num < 31)
			rf_ch_num += fhss_code + 2;
        rf_ch_num -= 31;
    }

}

// Generate internal id
static void __attribute__((unused)) SFHSS_get_tx_id()
{
	//  Some receivers (Orange) behaves better if they tuned to id that has
	//  no more than 6 consecutive zeros and ones
	uint32_t fixed_id;
	uint8_t run_count = 0;
	// add guard for bit count
	fixed_id = 1 ^ (MProtocol_id & 1);
	for (uint8_t i = 0; i < 16; ++i)
	{
		fixed_id = (fixed_id << 1) | (MProtocol_id & 1);
		MProtocol_id >>= 1;
		// If two LS bits are the same
		if ((fixed_id & 3) == 0 || (fixed_id & 3) == 3)
		{
			if (++run_count > 6)
			{
				fixed_id ^= 1;
				run_count = 0;
			}
		}
		else
			run_count = 0;
	}
	//    fixed_id = 0xBC11;
	rx_tx_addr[0] = fixed_id >> 8;
	rx_tx_addr[1] = fixed_id >> 0;
}

static void __attribute__((unused)) SFHSS_rf_init()
{
	CC2500_Strobe(CC2500_SIDLE);

	for (uint8_t i = 0; i < 39; ++i)
	{
		CC2500_WriteReg(i, SFHSS_init_values[i]);
	}
	//prev_option = option;
	Delay_US(500);
	CC2500_WriteReg(CC2500_0C_FSCTRL0, 0x0A);
	CC2500_SetTxRxMode(TX_EN);
	//CC2500_SetPower(RF_POWER);
}

void initSFHSS(uint8_t protocolIndex)
{
	//BIND_DONE;						 // Not a TX bind protocol
	//SPI2_Init();
	uint8_t CC2500_Error_flg = 0;
	MProtocol_id = GetUniqueID();
	SFHSS_get_tx_id();
	srand(SysTick->VAL);
	fhss_code = rand() % 28;  // Initialize it to random 0-27 inclusive
	CC2500_Reset(); 
	HAL_Delay(1);
	CC2500_Error_flg = CC2500_Init(protocolIndex); 
	if(CC2500_Error_flg == 1)
	{
		//Initialization failed
	}
	else
	{
		SFHSS_rf_init();
	}
	CC2500_SetPower(RF_POWER);
	phase = SFHSS_START;
	//return 10000;
}

// Channel value -125%<->125% is scaled to 16bit value with no limit
int16_t convert_channel_16b_nolimit(uint8_t num, int16_t min, int16_t max)
{
	int32_t val=Channel_DataBuff[num];				// 0<->2047
//	val=(val-CHANNEL_MIN_100)*(max-min)/(CHANNEL_MAX_100-CHANNEL_MIN_100)+min;
	return (uint16_t)val;
}

uint16_t ch[4];
static void __attribute__((unused)) SFHSS_build_data_packet(uint16_t* sfhssControlData)
{
	
	// command.bit0 is the packet number indicator: =0 -> SFHSS_DATA1, =1 -> SFHSS_DATA2
	// command.bit1 is unknown but seems to be linked to the payload[0].bit0 but more dumps are needed: payload[0]=0x82 -> =0, payload[0]=0x81 -> =1
	// command.bit2 is the failsafe transmission indicator: =0 -> normal data, =1->failsafe data
	// command.bit3 is the channels indicator: =0 -> CH1-4, =1 -> CH5-8
	
	//Coding below matches the Futaba T8J transmission scheme DATA1->CH1-4, DATA2->CH5-8, DATA1->CH5-8, DATA2->CH1-4,...
	// XK, T10J and TM-FH are different with a classic DATA1->CH1-4, DATA2->CH5-8,...
	//Failsafe is sent twice every couple of seconds (unknown but >5s) 
	uint8_t command= (phase == SFHSS_DATA1) ? 0 : 1;	// Building packet for Data1 or Data2
	counter+=command;
	
	command|=0x02;								        // Assuming packet[0] == 0x81
	counter&=0x3FF;										// Reset failsafe counter
	if(counter&1) command|=0x08;						// Transmit lower and upper channels twice in a row

	uint8_t ch_offset = (command&0x08) >> 1;			// CH1..CH4 or CH5..CH8

    Channel_DataBuff[0] = sfhssControlData[MIX_THROTTLE];
    Channel_DataBuff[1] = sfhssControlData[MIX_RUDDER];
    Channel_DataBuff[2] = sfhssControlData[MIX_ELEVATOR];
    Channel_DataBuff[3] = sfhssControlData[MIX_AILERON];

    Channel_DataBuff[4] = sfhssControlData[4];
    Channel_DataBuff[5] = sfhssControlData[5];
	Channel_DataBuff[6] = sfhssControlData[6];
    Channel_DataBuff[7] = sfhssControlData[7];
		{	//Normal data
			for(uint8_t i=0;i<4;i++)
				ch[i] = convert_channel_16b_nolimit(CH_AETR[ch_offset+i],2000,1000);
		}
	
	// XK		[0]=0x81 [3]=0x00 [4]=0x00
	// T8J		[0]=0x81 [3]=0x42 [4]=0x07
	// T10J		[0]=0x81 [3]=0x0F [4]=0x09
	// TM-FH	[0]=0x82 [3]=0x9A [4]=0x06
	packet[0]  = 0x81;	// can be 80 or 81 for Orange, only 81 for XK
	packet[1]  = rx_tx_addr[0];
	packet[2]  = rx_tx_addr[1];
	packet[3]  = 0x00;	// unknown but prevents some receivers to bind if not 0
	packet[4]  = 0x00;	// unknown but prevents some receivers to bind if not 0
	packet[5]  = (rf_ch_num << 3) | ((ch[0] >> 9) & 0x07);
	packet[6]  = (ch[0] >> 1);
	packet[7]  = (ch[0] << 7) | ((ch[1] >> 5) & 0x7F );
	packet[8]  = (ch[1] << 3) | ((ch[2] >> 9) & 0x07 );
	packet[9]  = (ch[2] >> 1);
	packet[10] = (ch[2] << 7) | ((ch[3] >> 5) & 0x7F );
	packet[11] = (ch[3] << 3) | ((fhss_code >> 2) & 0x07 );
	packet[12] = (fhss_code << 6) | command;

}

static void __attribute__((unused)) SFHSS_send_packet()
{
    CC2500_WriteData(packet, SFHSS_PACKET_LEN);
}


uint16_t ReadSFHSS(uint16_t* controlData)
{
    switch(phase)
    {
        case SFHSS_START:
            rf_ch_num = 0;
			SFHSS_tune_chan();
			phase = SFHSS_CAL;
			return 2000;
        //break;
		case SFHSS_CAL:
			calData[rf_ch_num]=CC2500_ReadReg(CC2500_25_FSCAL1);
			if (++rf_ch_num < 50)
				SFHSS_tune_chan();
			else
			{
				rf_ch_num = 0;
				counter = 0;
				phase = SFHSS_DATA1;
			}		
			return 2000;
            //break;
		/* Work cycle: 6.8ms */
    //#define SFHSS_PACKET_PERIOD	6798
    //#define SFHSS_DATA2_TIMING	1630//1647
    //#define SFHSS_TUNE_TIMING	2020
#define SFHSS_PACKET_PERIOD	6798
#define SFHSS_DATA2_TIMING	2020//1647
#define SFHSS_TUNE_TIMING	2020
			// Adjust this value between 1600 and 1650 if your RX(s) are not operating properly   //1647
		case SFHSS_DATA1:
			SFHSS_build_data_packet(controlData);
			SFHSS_send_packet();
			phase = SFHSS_DATA2;
			return SFHSS_DATA2_TIMING;								// original 1650
//		//break;
        case SFHSS_DATA2:
			SFHSS_build_data_packet(controlData);
			SFHSS_send_packet();
			SFHSS_calc_next_chan();
			phase = SFHSS_TUNE;
        return SFHSS_TUNE_TIMING;
        //return (SFHSS_PACKET_PERIOD-SFHSS_DATA2_TIMING-SFHSS_TUNE_TIMING);
			//return (SFHSS_PACKET_PERIOD -2000 -SFHSS_DATA2_TIMING);	// original 2000
//        return SFHSS_TUNE_TIMING;
//        //break;
		case SFHSS_TUNE:
			phase = SFHSS_DATA1;
			//sbus_checkrx();
			//SFHSS_tune_freq();
			SFHSS_tune_chan_fast();
			//CC2500_SetPower(CC2500_POWER_15);
			//return 1845;   //1845
        return (SFHSS_PACKET_PERIOD-SFHSS_DATA2_TIMING-SFHSS_TUNE_TIMING);
            //return (SFHSS_PACKET_PERIOD-SFHSS_DATA2_TIMING-SFHSS_TUNE_TIMING);
        //break;

	}
	return 0;
}

void SFHSS_SetBind(void)
{
    HAL_Delay(1);
}
