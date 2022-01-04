#include "sx1280.h"
#include "gpio.h"
#include "sx1280hal.h"
#include "fhss.h"
#include "common.h"

SX1280_t SX1280;
POWERMGNT_t POWERMGNT;

void SX1280_Init()
{
    SX1280.LastPacketRSSI = 0;
    SX1280.LastPacketSNR = 0;
    SX1280.currOpmode = SX1280_MODE_SLEEP;
    SX1280.currBW = SX1280_LORA_BW_0800;
    SX1280.currSF = SX1280_LORA_SF6;
    SX1280.currCR = SX1280_LORA_CR_4_7;
    SX1280.currFreq = GetInitialFreq(); //set frequency first or an error will occur!!!
    SX1280.IQinverted = 0;
    SX1280.InterruptAssignment = SX1280_INTERRUPT_NONE;
    
    SX1280_SetMode(SX1280_MODE_STDBY_RC);                                                                                                //step 1 put in STDBY_RC mode
    SX1280_HalWriteCommandVal(SX1280_RADIO_SET_PACKETTYPE, SX1280_PACKET_TYPE_LORA);                                                             //Step 2: set packet type to LoRa
    SX1280_ConfigLoRaModParams(SX1280.currBW, SX1280.currSF, SX1280.currCR);                                                                                      //Step 5: Configure Modulation Params
    SX1280_HalWriteCommandVal(SX1280_RADIO_SET_AUTOFS, 0x01);                                                                                    //enable auto FS
    SX1280_WriteRegister(0x0891, (SX1280_ReadRegister(0x0891) | 0xC0));                                                                       //default is low power mode, switch to high sensitivity instead
    SX1280_SetPacketParams(12, SX1280_LORA_PACKET_IMPLICIT, 8, SX1280_LORA_CRC_OFF, SX1280_LORA_IQ_NORMAL);                              //default params
    SX1280_SetFrequencyReg(SX1280.currFreq);                                                                                              //Step 3: Set Freq
    SX1280_SetFIFOaddr(0x00, 0x00);                                                                                                      //Step 4: Config FIFO addr
    SX1280_SetDioIrqParams(SX1280_IRQ_RADIO_ALL, SX1280_IRQ_TX_DONE | SX1280_IRQ_RX_DONE, SX1280_IRQ_RADIO_NONE, SX1280_IRQ_RADIO_NONE); //set IRQ to both RXdone/TXdone on DIO1
}



void SX1280_TXnb(volatile uint8_t *data, uint8_t length)
{
     if (SX1280.currOpmode == SX1280_MODE_TX) //catch TX timeout
    {
        SX1280_SetMode(SX1280_MODE_FS);
        SX1280_TXnbISR();
        return;
    }
    SX1280Hal_TXenable();                      // do first to allow PA stablise
    SX1280_HalWriteBuffer(0x00, data, length); //todo fix offset to equal fifo addr
    SX1280_SetMode(SX1280_MODE_TX);
}

void SX1280_RXnb()
{
    SX1280Hal_RXenable();
    SX1280_ClearIrqStatus(SX1280_IRQ_RADIO_ALL);
    SX1280_SetMode(SX1280_MODE_RX);
}

void SX1280_TXnbISR()
{
    SX1280.currOpmode = SX1280_MODE_FS; // radio goes to FS after TX
    TXdoneISR();
}

void SX1280_RXnbISR()
{
    SX1280.currOpmode = SX1280_MODE_FS;
    SX1280_ClearIrqStatus(SX1280_IRQ_RADIO_ALL);
    uint8_t FIFOaddr = SX1280_GetRxBufferAddr();
    SX1280_HalReadBuffer(FIFOaddr, SX1280.radioRXdataBuffer, TXRXBuffSize);
    SX1280_GetLastPacketStats();
 
    RXdoneISR();
}

void  SX1280_IsrCallback(void)
{
    uint16_t irqStatus = SX1280_GetIrqStatus();
    SX1280_ClearIrqStatus(SX1280_IRQ_RADIO_ALL);
    SX1280_TXnbISR();
}
void SX1280_Reset(void)
{
    HAL_GPIO_WritePin(SX1280_RST_GPIO_Port, SX1280_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(SX1280_RST_GPIO_Port, SX1280_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
}

void SX1280_SetMode(SX1280_RadioOperatingModes_t OPmode)
{

    if (OPmode == SX1280.currOpmode)
    {
       return;
    }
    uint8_t buf[3];
    uint32_t switchDelay = 0;

    switch (OPmode)
    {
        case SX1280_MODE_SLEEP:
            SX1280_HalWriteCommandVal(SX1280_RADIO_SET_SLEEP, 0x01);
            break;

        case SX1280_MODE_CALIBRATION:
            break;

        case SX1280_MODE_STDBY_RC:
            SX1280_HalWriteCommandVal(SX1280_RADIO_SET_STANDBY, SX1280_STDBY_RC);
            switchDelay = 1500;
            break;

        case SX1280_MODE_STDBY_XOSC:
            SX1280_HalWriteCommandVal(SX1280_RADIO_SET_STANDBY, SX1280_STDBY_XOSC);
            switchDelay = 50;
            break;

        case SX1280_MODE_FS:
            SX1280_HalWriteCommandVal(SX1280_RADIO_SET_FS, 0x00);
            switchDelay = 70;
            break;

        case SX1280_MODE_RX:
            buf[0] = 0x00; // periodBase = 1ms, page 71 datasheet, set to FF for cont RX
            buf[1] = 0xFF;
            buf[2] = 0xFF;
            SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_RX, buf, sizeof(buf));
            switchDelay = 100;
            break;

        case SX1280_MODE_TX:
            //uses timeout Time-out duration = periodBase * periodBaseCount
            buf[0] = 0x00; // periodBase = 1ms, page 71 datasheet
            buf[1] = 0xFF; // no timeout set for now
            buf[2] = 0xFF; // TODO dynamic timeout based on expected onairtime
            SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_TX, buf, sizeof(buf));
            switchDelay = 100;
            break;

        case SX1280_MODE_CAD:
            break;

        default:
            break;
    }
//    hal.BusyDelay(switchDelay);

    SX1280.currOpmode = OPmode;
}

void SX1280_ConfigLoRaModParams(SX1280_RadioLoRaBandwidths_t bw, SX1280_RadioLoRaSpreadingFactors_t sf, SX1280_RadioLoRaCodingRates_t cr)
{
    // Care must therefore be taken to ensure that modulation parameters are set using the command
    // SetModulationParam() only after defining the packet type SetPacketType() to be used
    uint8_t rfparams[3] = {0};

    rfparams[0] = (uint8_t)sf;
    rfparams[1] = (uint8_t)bw;
    rfparams[2] = (uint8_t)cr;

    SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_MODULATIONPARAMS, rfparams, sizeof(rfparams));

    switch (sf)
    {
    case SX1280_LORA_SF5:
    case SX1280_LORA_SF6:
        SX1280_WriteRegister(0x925, 0x1E); // for SF5 or SF6
        break;
    case SX1280_LORA_SF7:
    case SX1280_LORA_SF8:
        SX1280_WriteRegister(0x925, 0x37); // for SF7 or SF8
        break;
    default:
        SX1280_WriteRegister(0x925, 0x32); // for SF9, SF10, SF11, SF12
    }
}

void SX1280_SetPacketParams(uint8_t PreambleLength, SX1280_RadioLoRaPacketLengthsModes_t HeaderType, uint8_t PayloadLength, SX1280_RadioLoRaCrcModes_t crc, SX1280_RadioLoRaIQModes_t InvertIQ)
{
    uint8_t buf[7];

    buf[0] = PreambleLength;
    buf[1] = HeaderType;
    buf[2] = PayloadLength;
    buf[3] = crc;
    buf[4] = InvertIQ;
    buf[5] = 0x00;
    buf[6] = 0x00;

    SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_PACKETPARAMS, buf, sizeof(buf));
}

void SX1280_SetFrequencyReg(uint32_t freq)
{
    uint8_t buf[3] = {0};

    buf[0] = (uint8_t)((freq >> 16) & 0xFF);
    buf[1] = (uint8_t)((freq >> 8) & 0xFF);
    buf[2] = (uint8_t)(freq & 0xFF);

    SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_RFFREQUENCY, buf, sizeof(buf));
    SX1280.currFreq = freq;
}

void SX1280_SetFIFOaddr(uint8_t txBaseAddr, uint8_t rxBaseAddr)
{
    uint8_t buf[2];

    buf[0] = txBaseAddr;
    buf[1] = rxBaseAddr;
    SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_BUFFERBASEADDRESS, buf, sizeof(buf));
}

void SX1280_SetDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask)
{
    uint8_t buf[8];

    buf[0] = (uint8_t)((irqMask >> 8) & 0x00FF);
    buf[1] = (uint8_t)(irqMask & 0x00FF);
    buf[2] = (uint8_t)((dio1Mask >> 8) & 0x00FF);
    buf[3] = (uint8_t)(dio1Mask & 0x00FF);
    buf[4] = (uint8_t)((dio2Mask >> 8) & 0x00FF);
    buf[5] = (uint8_t)(dio2Mask & 0x00FF);
    buf[6] = (uint8_t)((dio3Mask >> 8) & 0x00FF);
    buf[7] = (uint8_t)(dio3Mask & 0x00FF);

    SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_DIOIRQPARAMS, buf, sizeof(buf));
}

uint16_t SX1280_GetFirmwareVersion(void)
{
    return( ( ( SX1280_ReadRegister( REG_LR_FIRMWARE_VERSION_MSB ) ) << 8 ) | ( SX1280_ReadRegister( REG_LR_FIRMWARE_VERSION_MSB + 1 ) ) );
}

uint16_t SX1280_GetIrqStatus(void)
{
    uint8_t status[2];
    SX1280_HalReadCommand(SX1280_RADIO_GET_IRQSTATUS, status, 2);
    return status[0] << 8 | status[1];
}

void SX1280_ClearIrqStatus(uint16_t irqMask)
{
    uint8_t buf[2];

    buf[0] = (uint8_t)(((uint16_t)irqMask >> 8) & 0x00FF);
    buf[1] = (uint8_t)((uint16_t)irqMask & 0x00FF);

    SX1280_HalWriteCommandBuffer(SX1280_RADIO_CLR_IRQSTATUS, buf, sizeof(buf));
}

uint8_t SX1280_GetRxBufferAddr()
{
    uint8_t status[2] = {0};
    SX1280_HalReadCommand(SX1280_RADIO_GET_RXBUFFERSTATUS, status, 2);
    return status[1];
}

void SX1280_GetLastPacketStats()
{
    uint8_t status[2];

    SX1280_HalReadCommand(SX1280_RADIO_GET_PACKETSTATUS, status, 2);
    SX1280.LastPacketRSSI = -(int8_t)(status[0] / 2);
    SX1280.LastPacketSNR = (int8_t)status[1] / 4;
}


uint8_t SX1280_ReadRegister(uint16_t address)
{
    uint8_t data;
    SX1280_HalReadRegisters( address, &data, 1 );
    return data;
}

void SX1280_WriteRegister(uint16_t address, uint8_t value)
{
    SX1280_HalWriteRegister(address, &value, 1);
}

void SX1280_Config(SX1280_RadioLoRaBandwidths_t bw, SX1280_RadioLoRaSpreadingFactors_t sf, SX1280_RadioLoRaCodingRates_t cr, uint32_t freq, uint8_t PreambleLength, uint8_t InvertIQ)
{
    SX1280.IQinverted = InvertIQ;
    SX1280_SetMode(SX1280_MODE_STDBY_XOSC);
    SX1280_ClearIrqStatus(SX1280_IRQ_RADIO_ALL);
    SX1280_ConfigLoRaModParams(bw, sf, cr);
    SX1280_SetPacketParams(PreambleLength, SX1280_LORA_PACKET_IMPLICIT, 8, SX1280_LORA_CRC_OFF, (SX1280_RadioLoRaIQModes_t)((uint8_t)!SX1280.IQinverted << 6)); // TODO don't make static etc. LORA_IQ_STD = 0x40, LORA_IQ_INVERTED = 0x00
    SX1280_SetFrequencyReg(freq);
}


PowerLevels_e SX1280_SetPower(PowerLevels_e Power)
{
    if (Power == POWERMGNT.currentPower)
        return POWERMGNT.currentPower;

    if (Power > MaxPower)
    {
        Power = (PowerLevels_e)MaxPower;
    }


#if defined(TARGET_TX_BETAFPV_900_V1)
    switch (Power)
    {
        case PWR_250mW:
            Radio.SetOutputPower(0b0011);
            break;
        case PWR_500mW:
            Radio.SetOutputPower(0b1000);
            break;
        case PWR_100mW:
        default:
            Power = PWR_100mW;
            Radio.SetOutputPower(0b0000);
            break;
    }
#elif defined(TARGET_TX_BETAFPV_2400_V1)
    switch (Power)
    {
        case PWR_25mW:
            SX1280_SetOutputPower(-5);
            break;
        case PWR_50mW:
            SX1280_SetOutputPower(-2);
            break;
        case PWR_100mW:    
        default:
            Power = PWR_100mW;
            SX1280_SetOutputPower(2);
            break;
    }
#endif
    
    POWERMGNT.currentPower = Power;
    //powerLedUpdate();
    return Power;
}

