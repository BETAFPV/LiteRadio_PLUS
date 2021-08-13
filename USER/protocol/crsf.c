#include "crsf.h"
#include "usart.h"
#include "function.h"
#include "buzzer.h"
#include "mixes.h"
#include "status.h"
#include "common.h"

uint8_t crsfPacket[26] = {0x0F, 0x00, 0x34, 0x1F, 0xA8, 0x09, 0x08, 0x6A, 0x50, 0x03,0x10, 0x80, 0x00,
                             0x04, 0x20, 0x00, 0x01, 0x08, 0x07, 0x38, 0x00, 0x10, 0x80, 0x00, 0x04,0x00};
uint8_t sbusPacket[25] = {0x0F, 0x00, 0x34, 0x1F, 0xA8, 0x09, 0x08, 0x6A, 0x50, 0x03,0x10, 0x80, 0x00,
                             0x04, 0x20, 0x00, 0x01, 0x08, 0x07, 0x38, 0x00, 0x10, 0x80, 0x00, 0x04};
uint8_t outBuffer[LinkStatisticsFrameLength + 4] = {0};
static uint16_t channelDataBuff[16] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};
crsfData_t internalCRSFdata;
crsfData_t externalCRSFdata;
uint16_t controlDataBuff[8] = {0};
uint8_t crsfLinkCount= 0;
uint8_t dataToCRSF[8];
uint16_t requestCount;
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
    if(externalCRSFdata.configSetFlag)
    {
        if(externalCRSFdata.crsfParameter.rate != externalCRSFdata.lastCRSFparameter.rate)
        {
            switch (externalCRSFdata.regulatoryDomainIndex)
            {
                case FREQ_FCC_915:
                case FREQ_EU_868:
                {
                    switch (externalCRSFdata.crsfParameter.rate)
                    {
                        case FREQ_900_RATE_200HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_200HZ;
                            break;
                        case FREQ_900_RATE_100HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_100HZ;
                            break;
                        case FREQ_900_RATE_50HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_50HZ;
                            break;
                        case FREQ_900_RATE_25HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_25HZ;
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case FREQ_ISM_2400:
                {
                    switch (externalCRSFdata.crsfParameter.rate)
                    {
                        case FREQ_2400_RATE_500HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_500HZ;
                            break;
                        case FREQ_2400_RATE_250HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_250HZ;
                            break;
                        case FREQ_2400_RATE_150HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_150HZ;
                            break;
                        case FREQ_2400_RATE_50HZ:
                            externalCRSFdata.crsfParameter.rate = RATE_50HZ;
                            break;
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
                            
            }
            Send_CRSFParameterPackage(0x01,externalCRSFdata.crsfParameter.rate);
            externalCRSFdata.lastCRSFparameter.rate = externalCRSFdata.crsfParameter.rate;
        }
        if(externalCRSFdata.crsfParameter.TLM != externalCRSFdata.lastCRSFparameter.TLM)
        {
            Send_CRSFParameterPackage(0x02,externalCRSFdata.crsfParameter.TLM);
            externalCRSFdata.lastCRSFparameter.TLM = externalCRSFdata.crsfParameter.TLM;
        }
        if(externalCRSFdata.crsfParameter.power != externalCRSFdata.lastCRSFparameter.power)
        {
            Send_CRSFParameterPackage(0x03,externalCRSFdata.crsfParameter.power);
            externalCRSFdata.lastCRSFparameter.power = externalCRSFdata.crsfParameter.power;
        }
        if(externalCRSFdata.inBindingMode)
        {
            Send_CRSFParameterPackage(0xFF,1);
            externalCRSFdata.inBindingMode = 0x00;
        }        
        if(externalCRSFdata.webUpdateMode)
        {
//            Send_CRSFParameterPackage(0xFE,1);
            externalCRSFdata.webUpdateMode = 0x00;
        }                
        
        //Send_CRSFParameterPackage(externalCRSFdata.setDataType,externalCRSFdata.setDataParameter);
        externalCRSFdata.configSetFlag = 0;
        
    }
//    else if()
//    {
//        GetSbusPackage(sbusPacket,crsfcontrol_data);

//        HAL_UART_Transmit_DMA(&huart1,sbusPacket,25);
//    }
    else
    {
        Get_CRSFPackage(crsfPacket,crsfcontrol_data);
        HAL_UART_Transmit_DMA(&huart1,crsfPacket,26);
    }
    if(externalCRSFdata.RSSI<80 && crsfLinkCount>10)
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
                    externalCRSFdata.RSSI = crsfRXPacket[5];
//                }
                crsfLinkCount =255;              
            }
        }
        else if(crsfRXPacket[1] == 0x0E)
        {
            if(crsfRXPacket[2] == CRSF_FRAMETYPE_PARAMETER_WRITE)
            {
                externalCRSFdata.lastCRSFparameter.rate = crsfRXPacket[7];
                externalCRSFdata.lastCRSFparameter.TLM = crsfRXPacket[8];
                externalCRSFdata.lastCRSFparameter.power = crsfRXPacket[9];
                externalCRSFdata.regulatoryDomainIndex = crsfRXPacket[10];
                externalCRSFdata.crsfParameter = externalCRSFdata.lastCRSFparameter;
                connectTickLast = HAL_GetTick();
                if(externalCRSFdata.configUpdateFlag)
                {
                    requestType1 = 0x02;
                    requestType2 = 0x02;
                    externalCRSFdata.configUpdateFlag = 0x00;
                }
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
    connectTickNow = HAL_GetTick();
    if(connectTickNow - connectTickLast >= 1000)
    {
        externalCRSFdata.connectionState = 0x00;
    }
    else
    {
        externalCRSFdata.connectionState = 0x01;
    }
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

void Send_CRSFParameterPackage(uint8_t dataType,uint8_t dataParameter)
{
    dataToCRSF[0] = CRSF_ADDRESS_CRSF_TRANSMITTER; //MODULE ADDRESS: 0xEE
    dataToCRSF[1] = 0x06; // 1(ID) + 4 + 1(CRC)
    dataToCRSF[2] = CRSF_FRAMETYPE_PARAMETER_WRITE;
    dataToCRSF[3] = CRSF_ADDRESS_CRSF_TRANSMITTER;
    dataToCRSF[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    dataToCRSF[5] = dataType;
    dataToCRSF[6] = dataParameter;
    uint8_t * pointDataToCRSF = &dataToCRSF[2];
    dataToCRSF[7] = crc8(pointDataToCRSF, 5);  
    HAL_UART_Transmit_DMA(&huart1,dataToCRSF,8);    
}
