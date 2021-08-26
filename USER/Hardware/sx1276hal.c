#include "sx1276hal.h"
#include "spi.h"
#include "string.h"
#include "sx1276.h"
uint8_t SX1276_HalReadRegister(uint8_t reg)
{
    uint8_t halTxBuffer[2];
    uint8_t halRxBuffer[2];

    halTxBuffer[0] = reg | SPI_READ;

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
    
    HAL_SPI_TransmitReceive(&hspi2, halTxBuffer,halRxBuffer,2,1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

  return (halRxBuffer[1]);
}

void SX1276_HalWriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t halTxBuffer[2];

    halTxBuffer[0] = reg | SPI_WRITE;
    halTxBuffer[1] = data;

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
    
    HAL_SPI_Transmit(&hspi2,halTxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
    
}

uint8_t SX1276_HalSetRegValue(uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb)
{
  if ((msb > 7) || (lsb > 7) || (lsb > msb))
  {
    return (ERR_INVALID_BIT_RANGE);
  }

  uint8_t currentValue = SX1276_HalReadRegister(reg);
  uint8_t mask = ~((0xFF << (msb + 1)) | (0xFF >> (8 - lsb)));
  uint8_t newValue = (currentValue & ~mask) | (value & mask);
  SX1276_HalWriteRegister(reg, newValue);
  return (ERR_NONE);
}

uint8_t SX1276_HalGetRegValue(uint8_t reg, uint8_t msb, uint8_t lsb)
{
  if ((msb > 7) || (lsb > 7) || (lsb > msb))
  {
    return (ERR_INVALID_BIT_RANGE);
  }
  uint8_t rawValue = SX1276_HalReadRegister(reg);
  uint8_t maskedValue = rawValue & ((0xFF << lsb) & (0xFF >> (7 - msb)));
  return (maskedValue);
}


void SX1276_HalwriteRegisterBurst(uint8_t reg, uint8_t *data, uint8_t numBytes)
{
    uint8_t halTxBuffer[numBytes + 1];
    halTxBuffer[0] = reg | SPI_WRITE;
    memcpy(halTxBuffer + 1,  data, numBytes);

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
    
    SX1276_HalWriteRegister(reg, (numBytes + 1));
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
}

void SX1276_HalReadRegisterFIFO(volatile uint8_t *data, uint8_t numBytes)
{
    uint8_t halTxBuffer[numBytes + 1];
    uint8_t halRxBuffer[numBytes + 1];

    halTxBuffer[0] = SX127X_REG_FIFO | SPI_READ;

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&hspi2, halTxBuffer,halRxBuffer,numBytes + 1,1000);

    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

    for (int i = 0; i < numBytes; i++) // todo check if this is the right want to handle volatiles
    {
        data[i] = halRxBuffer[i + 1];
    }
}

void SX1276_HalWriteRegisterFIFO(volatile uint8_t *data, uint8_t numBytes)
{
    uint8_t halTxBuffer[numBytes + 1];
    halTxBuffer[0] = (SX127X_REG_FIFO | SPI_WRITE);

    for (int i = 0; i < numBytes; i++) // todo check if this is the right want to handle volatiles
    {
        halTxBuffer[i + 1] = data[i];
    }
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi2,halTxBuffer,(uint8_t)sizeof(halTxBuffer),1000);
    
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
}


void SX1276_HalRXenable()
{
    SX1276.InterruptAssignment = SX127x_INTERRUPT_RX_DONE;
    
    HAL_GPIO_WritePin(GPIOA, SX1280_TXRX_EN_Pin, GPIO_PIN_RESET);
}

void SX1276_HalTXenable()
{
    SX1276.InterruptAssignment = SX127x_INTERRUPT_TX_DONE;
    
    HAL_GPIO_WritePin(GPIOA, SX1280_TXRX_EN_Pin, GPIO_PIN_SET);
}
