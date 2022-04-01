#include "sx1280hal.h"
#include "sx1280.h"
#include "string.h"
#include "spi.h"
#include "gpio.h"

void SX1280_HalReadRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    uint8_t halTxBuffer[size + 4];
    uint8_t halRxBuffer[size + 4];
    uint16_t halSize = 4 + size;
    halTxBuffer[0] = SX1280_RADIO_READ_REGISTER;
    halTxBuffer[1] = ( address & 0xFF00 ) >> 8;
    halTxBuffer[2] = address & 0x00FF;
    halTxBuffer[3] = 0x00;
    for( uint16_t index = 0; index < size; index++ )
    {
        halTxBuffer[4+index] = 0x00;
    }
    
    SX1280_HalWaitOnBusy();

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&hspi2, halTxBuffer,halRxBuffer,halSize,1000);
    
    memcpy(buffer, halRxBuffer + 4, size);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
    
    //SX1280HalWaitOnBusy();
}

void SX1280_HalWriteRegister(uint16_t address, uint8_t *buffer, uint8_t size)
{
    uint8_t halTxBuffer[size + 3];

    halTxBuffer[0] = (SX1280_RADIO_WRITE_REGISTER);
    halTxBuffer[1] = ((address & 0xFF00) >> 8);
    halTxBuffer[2] = (address & 0x00FF);

    memcpy(halTxBuffer + 3, buffer, size);

    SX1280_HalWaitOnBusy();

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
    
    HAL_SPI_Transmit(&hspi2,halTxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

   // BusyDelay(12);
}

void SX1280_HalWaitOnBusy(void)
{
    while( HAL_GPIO_ReadPin(GPIOB, SX1280_BUSY_Pin ) == 1 );
}

void SX1280_HalWriteCommandVal(SX1280_RadioCommands_t command, uint8_t val)
{
    uint8_t halTxBuffer[2];
    
    halTxBuffer[0] = command;
    halTxBuffer[1] = val;
    
    SX1280_HalWaitOnBusy();
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi2,halTxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

    if(command != SX1280_RADIO_SET_SLEEP)
    {
        SX1280_HalWaitOnBusy();
    }
}

void SX1280_HalWriteCommandBuffer(SX1280_RadioCommands_t command, uint8_t *buffer, uint16_t size)
{
    uint8_t halTxBuffer[size + 1];
    
    halTxBuffer[0] = command;
    memcpy(halTxBuffer + 1, buffer, size); 
    
    SX1280_HalWaitOnBusy();

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi2,halTxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

    if(command != SX1280_RADIO_SET_SLEEP)
    {
        SX1280_HalWaitOnBusy();
    }
}


void SX1280_HalReadCommand(SX1280_RadioCommands_t command, uint8_t *buffer, uint8_t size)
{
    uint8_t halTxBuffer[size + 2];
    #define RADIO_GET_STATUS_BUF_SIZEOF 3 // special case for command == SX1280_RADIO_GET_STATUS, fixed 3 bytes packet size

    SX1280_HalWaitOnBusy();

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);

    if (command == SX1280_RADIO_GET_STATUS)
    {
        halTxBuffer[0] = (uint8_t)command;
        halTxBuffer[1] = 0x00;
        halTxBuffer[2] = 0x00;

        HAL_SPI_Transmit(&hspi2,halTxBuffer,RADIO_GET_STATUS_BUF_SIZEOF,1000);
        buffer[0] = halTxBuffer[0];
    }
    else
    {
        halTxBuffer[0] = (uint8_t)command;
        halTxBuffer[1] = 0x00;
        memcpy(halTxBuffer + 2, buffer, size);
        HAL_SPI_Transmit(&hspi2,halTxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
        
        memcpy(buffer, halTxBuffer + 2, size);
    }
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
}


void SX1280_HalWriteBuffer(uint8_t offset, volatile uint8_t *buffer, uint8_t size)
{
    uint8_t localbuf[size];

    for (int i = 0; i < size; i++) // todo check if this is the right want to handle volatiles
    {
        localbuf[i] = buffer[i];
    }

    uint8_t halTxBuffer[size + 2];

    halTxBuffer[0] = SX1280_RADIO_WRITE_BUFFER;
    halTxBuffer[1] = offset;

    memcpy(halTxBuffer + 2, localbuf, size);

    SX1280_HalWaitOnBusy();

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
    
    HAL_SPI_Transmit(&hspi2,halTxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

  //  BusyDelay(12);
}
uint8_t halRRxBuffer[12];
void SX1280_HalReadBuffer(uint8_t offset, volatile uint8_t *buffer, uint8_t size)
{
    uint8_t halTxBuffer[size + 3];
    uint8_t localbuf[size];

    halTxBuffer[0] = SX1280_RADIO_READ_BUFFER;
    halTxBuffer[1] = offset;
    halTxBuffer[2] = 0x00;

    SX1280_HalWaitOnBusy();

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&hspi2,halTxBuffer,halRRxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

    memcpy(localbuf, halRRxBuffer + 3, size);

    for (int i = 0; i < size; i++) // todo check if this is the right wany to handle volatiles
    {
        buffer[i] = localbuf[i];
    }
}

void SX1280Hal_TXenable()
{
    SX1280.InterruptAssignment = SX1280_INTERRUPT_TX_DONE;
    HAL_GPIO_WritePin(GPIOA, PA_TXRX_EN_Pin, GPIO_PIN_SET);
}


void SX1280Hal_RXenable()
{
    SX1280.InterruptAssignment = SX1280_INTERRUPT_RX_DONE;
    HAL_GPIO_WritePin(GPIOA, PA_TXRX_EN_Pin, GPIO_PIN_RESET);
}


void SX1280_SetOutputPower(int8_t power)
{
    if (power < -18) power = -18;
    else if (13 < power) power = 13;
    uint8_t buf[2] = {(uint8_t)(power + 18), (uint8_t)SX1280_RADIO_RAMP_04_US};
    SX1280_HalWriteCommandBuffer(SX1280_RADIO_SET_TXPARAMS, buf, sizeof(buf));
    return;
}
