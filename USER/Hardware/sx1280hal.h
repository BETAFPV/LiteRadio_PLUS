#ifndef __SX1280HAL_H_
#define __SX1280HAL_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "sx1280reg.h"

void SX1280_HalReadCommand(SX1280_RadioCommands_t command, uint8_t *buffer, uint8_t size);
void SX1280_HalWriteCommandVal(SX1280_RadioCommands_t command, uint8_t val);
void SX1280_HalWriteCommandBuffer(SX1280_RadioCommands_t command, uint8_t *buffer, uint16_t size);
void SX1280_HalReadRegisters(uint16_t address, uint8_t *buffer, uint16_t size);
void SX1280_HalWriteRegister(uint16_t address, uint8_t *buffer, uint8_t size);
void SX1280_HalWriteBuffer(uint8_t offset, volatile uint8_t *buffer, uint8_t size);
void SX1280_HalReadBuffer(uint8_t offset, volatile uint8_t *buffer, uint8_t size);

void SX1280_HalWaitOnBusy(void);

/*PA使能*/
void SX1280Hal_TXenable(void);
void SX1280Hal_RXenable(void);

/*Power*/
void SX1280_SetOutputPower(int8_t power);

#endif
