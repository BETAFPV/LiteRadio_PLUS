#ifndef __SX1276HAL_H_
#define __SX1276HAL_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "sx1276reg.h"

//#include "targets.h"

typedef enum
{
    SX127x_INTERRUPT_NONE,
    SX127x_INTERRUPT_RX_DONE,
    SX127x_INTERRUPT_TX_DONE,
    SX127x_INTERRUPT_CAD
} SX1276_InterruptAssignment;

uint8_t SX1276_HalReadRegister(uint8_t reg);
void SX1276_HalWriteRegister(uint8_t reg, uint8_t data);

uint8_t SX1276_HalSetRegValue(uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb);    
uint8_t SX1276_HalGetRegValue(uint8_t reg, uint8_t msb, uint8_t lsb);

void SX1276_HalwriteRegisterBurst(uint8_t reg, uint8_t *data, uint8_t numBytes);

void SX1276_HalReadRegisterFIFO(volatile uint8_t *data, uint8_t numBytes);
void SX1276_HalWriteRegisterFIFO(volatile uint8_t *data, uint8_t numBytes);
    


#endif
