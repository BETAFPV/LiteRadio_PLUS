#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f1xx_hal.h"
#define BIND_KEY_PORT           GPIOA
#define BIND_KEY_PIN            GPIO_PIN_8

#define CALIBRATE_KEY_PORT      GPIOB
#define CALIBRATE_KEY_PIN       GPIO_PIN_11

#define BIND_KEY_STATUS         HAL_GPIO_ReadPin(BIND_KEY_PORT,BIND_KEY_PIN)
#define CALIBRATE_KEY_STATUS    HAL_GPIO_ReadPin(CALIBRATE_KEY_PORT,CALIBRATE_KEY_PIN)

typedef enum{
    KEY_PRESS  = 0U,
    KEY_UP
} KeyState;

void Key_Init(void);
uint8_t Get_SetUpKeyStatus(void);
void SetUpKeyProcess(void);
uint8_t Get_SetUpKeyStatus(void);
void Set_SetUpKeyStatus(uint8_t x);
#endif
 