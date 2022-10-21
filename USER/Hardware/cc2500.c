#include "cc2500.h"
#include "gpio.h"
#include "spi.h"
#include "delay.h"
#include "stmflash.h"
#include "radiolink.h"

uint8_t RF_POWER;
static uint8_t cc2500_conf_D8[CC2500_CONFIG_CNTS][2]=
{
	{ CC2500_02_IOCFG0   , 0x06 },	
	{ CC2500_00_IOCFG2   , 0x06 },	
	{ CC2500_17_MCSM1    , 0x0c },  
	{ CC2500_18_MCSM0    , 0x18 },  
	{ CC2500_06_PKTLEN   , 0x19 },  
	{ CC2500_07_PKTCTRL1 , 0x05 },  
	{ CC2500_08_PKTCTRL0 , 0x05 },  
	{ CC2500_3E_PATABLE  , 0xff }, 
	{ CC2500_0B_FSCTRL1  , 0x08 }, 
	{ CC2500_0C_FSCTRL0  , 0x00 },
	{ CC2500_0D_FREQ2    , 0x5c },	       
	{ CC2500_0E_FREQ1    , 0x76 },
	{ CC2500_0F_FREQ0    , 0x27 },
	{ CC2500_10_MDMCFG4  , 0xAA },
	{ CC2500_11_MDMCFG3  , 0x39 },  
	{ CC2500_12_MDMCFG2  , 0x11 },  
	{ CC2500_13_MDMCFG1  , 0x23 },  
	{ CC2500_14_MDMCFG0  , 0x7a }, 
	{ CC2500_15_DEVIATN  , 0x42 }, 
	{ CC2500_1B_AGCCTRL2 , 0x03 },	
	{ CC2500_19_FOCCFG   , 0x16 }, 
	{ CC2500_1A_BSCFG    , 0x6c },	
	{ CC2500_1C_AGCCTRL1 , 0x40 }, 
	{ CC2500_1D_AGCCTRL0 , 0x91 },  
	{ CC2500_21_FREND1   , 0x56 },
	{ CC2500_22_FREND0   , 0x10 },
	{ CC2500_23_FSCAL3   , 0xa9 },
	{ CC2500_24_FSCAL2   , 0x0A },
	{ CC2500_25_FSCAL1   , 0x00 },
	{ CC2500_26_FSCAL0   , 0x11 },
	{ CC2500_29_FSTEST   , 0x59 },
	{ CC2500_2C_TEST2    , 0x88 },
	{ CC2500_2D_TEST1    , 0x31 },
	{ CC2500_2E_TEST0    , 0x0B },
	{ CC2500_03_FIFOTHR  , 0x07 },
	{ CC2500_09_ADDR     , 0x00 }
};

static uint8_t cc2500_conf_EU[CC2500_CONFIG_CNTS][2]=
{
	{ CC2500_02_IOCFG0   , 0x06 },	
	{ CC2500_00_IOCFG2   , 0x06 },	
	{ CC2500_17_MCSM1    , 0x0E },
	{ CC2500_18_MCSM0    , 0x18 },
	{ CC2500_06_PKTLEN   , 0x23 },
	{ CC2500_07_PKTCTRL1 , 0x04 },  
	{ CC2500_08_PKTCTRL0 , 0x01 },  
	{ CC2500_3E_PATABLE  , 0xff }, 
	{ CC2500_0B_FSCTRL1   , 0x08 },
	{ CC2500_0C_FSCTRL0  , 0x00 },	
	{ CC2500_0D_FREQ2    , 0x5c },	
	{ CC2500_0E_FREQ1    , 0x80 },
	{ CC2500_0F_FREQ0    , 0x00 },	
	{ CC2500_10_MDMCFG4  , 0x7B },
	{ CC2500_11_MDMCFG3  , 0xF8 },
	{ CC2500_12_MDMCFG2  , 0x03 },
	{ CC2500_13_MDMCFG1  , 0x23 },  
	{ CC2500_14_MDMCFG0  , 0x7a },
	{ CC2500_15_DEVIATN  , 0x53 },
	{ CC2500_19_FOCCFG   , 0x16 }, 
	{ CC2500_1A_BSCFG    , 0x6c },	
	{ CC2500_1B_AGCCTRL2 , 0x43 },	
	{ CC2500_1C_AGCCTRL1 , 0x40 },    
	{ CC2500_1D_AGCCTRL0 , 0x91 },  
	{ CC2500_21_FREND1   , 0x56 },
	{ CC2500_22_FREND0   , 0x10 },
	{ CC2500_23_FSCAL3   , 0xa9 },
	{ CC2500_24_FSCAL2   , 0x0A },
	{ CC2500_25_FSCAL1   , 0x00 },
	{ CC2500_26_FSCAL0   , 0x11 },
	{ CC2500_29_FSTEST   , 0x59 },
	{ CC2500_2C_TEST2    , 0x88 },
	{ CC2500_2D_TEST1    , 0x31 },
	{ CC2500_2E_TEST0    , 0x0B },
	{ CC2500_03_FIFOTHR  , 0x07 },
	{ CC2500_09_ADDR     , 0x00 }
	
};

static uint8_t cc2500_conf_FCC[CC2500_CONFIG_CNTS][2]=
{
	{ CC2500_02_IOCFG0   , 0x06 },	
	{ CC2500_00_IOCFG2   , 0x06 },	
	{ CC2500_17_MCSM1    , 0x0c },	
	{ CC2500_18_MCSM0    , 0x18 },
	{ CC2500_06_PKTLEN   , 0x1E }, 
	{ CC2500_07_PKTCTRL1 , 0x04 },  
	{ CC2500_08_PKTCTRL0 , 0x01 },  
	{ CC2500_3E_PATABLE  , 0xff }, 
	{ CC2500_0B_FSCTRL1  , 0x0A },   
	{ CC2500_0C_FSCTRL0  , 0x00 },	
	{ CC2500_0D_FREQ2    , 0x5c },	
	{ CC2500_0E_FREQ1    , 0x76 },  
	{ CC2500_0F_FREQ0    , 0x27 }, 	
	{ CC2500_10_MDMCFG4  , 0x7B },
	{ CC2500_11_MDMCFG3  , 0x61 }, 
	{ CC2500_12_MDMCFG2  , 0x13 },  
	{ CC2500_13_MDMCFG1  , 0x23 },  
	{ CC2500_14_MDMCFG0  , 0x7a },
	{ CC2500_15_DEVIATN  , 0x51 },  
	{ CC2500_19_FOCCFG   , 0x16 }, 
	{ CC2500_1A_BSCFG    , 0x6c },	
	{ CC2500_1B_AGCCTRL2 , 0x43 },	
	{ CC2500_1C_AGCCTRL1 , 0x40 },    
	{ CC2500_1D_AGCCTRL0 , 0x91 },  
	{ CC2500_21_FREND1   , 0x56 },
	{ CC2500_22_FREND0   , 0x10 },
	{ CC2500_23_FSCAL3   , 0xa9 },
	{ CC2500_24_FSCAL2   , 0x0A },
	{ CC2500_25_FSCAL1   , 0x00 },
	{ CC2500_26_FSCAL0   , 0x11 },
	{ CC2500_29_FSTEST   , 0x59 },
	{ CC2500_2C_TEST2    , 0x88 },
	{ CC2500_2D_TEST1    , 0x31 },
	{ CC2500_2E_TEST0    , 0x0B },
	{ CC2500_03_FIFOTHR  , 0x07 },
	{ CC2500_09_ADDR     , 0x00 }
	
};

 uint8_t CC2500_Init(uint8_t versionSelectFlg)
{
    uint8_t (*cc2500_config)[2];
    CC2500_PA_TXEN();              //ENABLE PA TX
    uint16_t internalPower;
    //STMFLASH_Read(INTERNAL_CONFIGER_INFO_POWER_ADDR,&internalPower,1);
	internalPower = PWR_100mW;
	switch(internalPower)
	{
		case PWR_25mW: 
            RF_POWER = CC2500_POWER_12;
            break;
		case PWR_50mW:
            RF_POWER = CC2500_POWER_14;
            break;
		case PWR_100mW: 
            RF_POWER = CC2500_POWER_17;
			break;
		default:
			break;
	}

    uint8_t CC2500RestError_flag = 0;
	HAL_Delay(100);
	if(!CC2500_Reset())
	{
		CC2500RestError_flag = 1;	
	}
	HAL_Delay(1);
	switch(versionSelectFlg)
	{
		case 0: cc2500_config = cc2500_conf_FCC;
				break;
		case 1: cc2500_config = cc2500_conf_EU;
				break;
		case 2: cc2500_config = cc2500_conf_D8;
				break;
		case 3: 
				break;
		default:
				break;
	}
	if(!CC2500RestError_flag)
	{
		for(uint8_t i=0 ;i < CC2500_CONFIG_CNTS ; ++i)
		{
			
			CC2500_WriteReg(cc2500_config[i][0],cc2500_config[i][1]);  //FCC
			Delay_US(20);
		}
		//CC2500_Strobe(CC2500_SIDLE);
		Delay_US(10);
		CC2500_SetTxRxMode(TX_EN);
		//CC2500_SetPower(RF_POWER);
		CC2500_Strobe(CC2500_SIDLE);
		Delay_US(10);
	}
	return CC2500RestError_flag;
}

uint8_t CC2500_Reset()
{
	CC2500_Strobe(CC2500_SRES);
    HAL_Delay(1);
	CC2500_SetTxRxMode(TXRX_OFF);
	return (CC2500_ReadReg(CC2500_0E_FREQ1) == 0xC4);//check if reset
}

void CC2500_SetPower(uint8_t power)
{
	static uint8_t prev_power = CC2500_POWER_1;
	if(prev_power != power)
	{
		prev_power = power;
		CC2500_WriteReg(CC2500_3E_PATABLE, power);
	}
}

void CC2500_WriteReglistrMulti(uint8_t address,uint8_t* data, uint16_t length)
{
	CC2500_NSS_LOW;
    uint8_t pdata = (CC2500_WRITE_BURST | address);
    HAL_SPI_Transmit(&hspi2, &pdata, 1,1);   
    HAL_SPI_Transmit(&hspi2, data, length,1);
    
	CC2500_NSS_HIGH;
}

void CC2500_WriteData(uint8_t *dpbuffer, uint16_t len)
{
	CC2500_Strobe(CC2500_SFTX);
	CC2500_WriteReglistrMulti(CC2500_3F_TXFIFO,dpbuffer,len);
	CC2500_Strobe(CC2500_STX);
}

void CC2500_Strobe(uint8_t state)
{
    CC2500_NSS_LOW;    
    HAL_SPI_Transmit(&hspi2, &state, 1,1);
	CC2500_NSS_HIGH;
}

uint8_t CC2500_ReadReg(uint8_t address)
{
    uint8_t pdata = CC2500_READ_SINGLE | address;
	uint8_t result;
	CC2500_NSS_LOW;          
    HAL_SPI_Transmit(&hspi2, &pdata, 1,1);
	HAL_SPI_Receive(&hspi2, &result, 1,1);
	CC2500_NSS_HIGH;
	return result;
}

void CC2500_WriteReg(uint8_t address, uint8_t data)
{
    uint8_t pData[2];
    pData[0] = address;
    pData[1] = data;
	CC2500_NSS_LOW;       	//delay_us(1);
    HAL_SPI_Transmit(&hspi2, pData, 2,1);
	CC2500_NSS_HIGH;
}


void CC2500_SetTxRxMode(uint8_t mode)
{
	if(mode == TX_EN)
	{//from deviation firmware
		CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F);
		CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F | 0x40);
	}
	else
		if (mode == RX_EN)
		{
			CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F);
			CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F | 0x40);
		}
		else
		{
			CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F);
			CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F);
		}
}

uint16_t CC2500_Process(uint16_t* controlDataBuff)
{
    channelData[0] = controlDataBuff[0];
    channelData[1] = controlDataBuff[1];
    channelData[2] = controlDataBuff[2];
    channelData[3] = controlDataBuff[3];
    channelData[4] = controlDataBuff[4];
    channelData[5] = controlDataBuff[5];
    channelData[6] = controlDataBuff[6];
    channelData[7] = controlDataBuff[7];
	return 0;
}

void CC2500_PA_TXEN()
{
    HAL_GPIO_WritePin(GPIOA, PA_TXRX_EN_Pin, GPIO_PIN_SET);
}


void CC2500_PA_RXEN()
{
    HAL_GPIO_WritePin(GPIOA, PA_TXRX_EN_Pin, GPIO_PIN_RESET);
}
