#ifndef _LED_H_
#define _LED_H_
#include "stm32f1xx_hal.h"

#define LED_BLUE_PORT       GPIOB
#define LED_RED_PORT        GPIOB

#define LED_BLUE_PIN        GPIO_PIN_0
#define LED_RED_PIN         GPIO_PIN_1

#define LED_BLUE_ON 		HAL_GPIO_WritePin(LED_BLUE_PORT,LED_BLUE_PIN,GPIO_PIN_SET)
#define LED_BLUE_OFF 		HAL_GPIO_WritePin(LED_BLUE_PORT,LED_BLUE_PIN,GPIO_PIN_RESET)
#define LED_RED_ON 		    HAL_GPIO_WritePin(LED_RED_PORT,LED_RED_PIN,GPIO_PIN_SET) 
#define LED_RED_OFF 	    HAL_GPIO_WritePin(LED_RED_PORT,LED_RED_PIN,GPIO_PIN_RESET) 

#define ON  GPIO_PIN_SET
#define OFF GPIO_PIN_RESET
 

void Led_Init(void);/* LED初始化 */
void Led_Red(GPIO_PinState status);
void Led_Blue(GPIO_PinState status);
void Led_On_Off(uint8_t status);
void Led_Twinkle(uint8_t num);
#endif

