/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "tim.h"
#include "crsf.h"
#include "radiolink.h"
#include "s_fhss.h"
#include "frsky_d16.h"
#include "common.h"
#if defined(LiteRadio_Plus_SX1280)
#include "sx1280.h"
#include "status.h"
#elif defined(LiteRadio_Plus_SX1276)
#include "sx1276.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t crsfRXPacket[64] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_FS;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;
extern uint16_t (*RF_Process)(uint16_t* controlData);
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel4 global interrupt.
  */
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel5 global interrupt.
  */
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */
//  Get_LinkStatis(crsfRXPacket);
  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim2_ch2_ch4);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
  * @brief This function handles USB low priority or CAN RX0 interrupts.
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */
  //  如果没有数据收到ESOF中断，则关闭如下中断 
  if (__HAL_PCD_GET_FLAG (&hpcd_USB_FS, USB_ISTR_ESOF))
  {
    /* clear ESOF flag in ISTR */
        USB->CNTR &= 0xe2ff;

        /* Force low-power mode in the macrocell */
        USB->CNTR |= USB_CNTR_FSUSP;
        USB->CNTR |= USB_CNTR_LP_MODE;
        
        USB->ISTR &= ~USB_ISTR_RESET;
        USB->ISTR &= ~USB_ISTR_SUSP;
        USB->ISTR &= ~USB_ISTR_WKUP;
        USB->ISTR &= ~USB_ISTR_ESOF;
  }
  /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt.
  */
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */
#if defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)
    if(Get_ProtocolIndex()==0)
        SendRCdataToRF(channelData);
#elif defined(LiteRadio_Plus_CC2500)
  TIM1->ARR = RF_Process(channelData); 
#endif        
  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
#if defined(LiteRadio_Plus_SX1280)
    if(Get_ProtocolIndex()==SX1280_ELRS)
            SX1280_IsrCallback();
#elif defined(LiteRadio_Plus_SX1276)
    if (SX1276.InterruptAssignment == SX127x_INTERRUPT_RX_DONE)
    {
        SX1276_RXnbISR();
    }
    else if (SX1276.InterruptAssignment == SX127x_INTERRUPT_TX_DONE)
    {
        SX1276_TXnbISR();
    }
#endif

    
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim ==&htim2)
    HAL_TIM_OC_Stop_DMA(&htim2,TIM_CHANNEL_4);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
#if defined(LiteRadio_Plus_SX1280)||(LiteRadio_Plus_SX1276)
    if(Get_ProtocolIndex()!=SX1280_CRSF)
        return;
#elif defined(LiteRadio_Plus_CC2500)
  if(Get_ProtocolIndex()!=CC2500_CRSF)
        return;
#endif
    
    if(externalCRSFdata.regulatoryDomainIndex!=0&&externalRFprarmeter.power!=0xff&&externalRFprarmeter.rate!=0xff&&externalRFprarmeter.TLM!=0xff)
        return;
    
    HAL_UART_Receive_DMA(&huart1,crsfRXPacket,maxPackSize);
    
    if (crsfRXPacket[0]!= RADIO_ADDRESS)
        return;
    
    if(maxPackSize == 64)
    {
        switch(crsfRXPacket[5])
        {
            case rate:
                if(externalCRSFdata.regulatoryDomainIndex == NANO_TX_2400Mhz)
                {
                    if(crsfRXPacket[6]==0 && crsfRXPacket[14]==41)
                    {
                        externalCRSFdata.crsfParameter.rate = crsfRXPacket[16];
                        externalCRSFdata.lastCRSFparameter.rate = crsfRXPacket[16];
                        externalRFprarmeter.rate = crsfRXPacket[16];
                    }
                    else if(crsfRXPacket[6]==1||crsfRXPacket[6]==2)
                    {
                        maxPackSize = 32;
                    }
                }
                else if(externalCRSFdata.regulatoryDomainIndex == NANO_TX_915Mhz)
                {
                    if(crsfRXPacket[6]==0 && crsfRXPacket[13]==41)
                    {
                        externalCRSFdata.crsfParameter.rate = crsfRXPacket[15];
                        externalCRSFdata.lastCRSFparameter.rate = crsfRXPacket[15];
                        externalRFprarmeter.rate = crsfRXPacket[15];
                    }
                    else if(crsfRXPacket[6]==1||crsfRXPacket[6]==2)
                    {
                        maxPackSize = 32;
                    }
                }
                
                break;
                
            case tlm:
                if(crsfRXPacket[6]==0 && crsfRXPacket[56]==50)
                {
                    externalCRSFdata.crsfParameter.TLM = crsfRXPacket[58];
                    externalCRSFdata.lastCRSFparameter.TLM = crsfRXPacket[58];
                    externalRFprarmeter.TLM =  crsfRXPacket[58];
                }
                else if(crsfRXPacket[6]==1||crsfRXPacket[6]==2)
                {
                    maxPackSize = 32;
                }
                break;
                
            case power:
                if(externalCRSFdata.regulatoryDomainIndex == NANO_TX_2400Mhz)
                {
                    if(crsfRXPacket[6]==0 && crsfRXPacket[38]==48)
                    {
                        externalCRSFdata.crsfParameter.power = crsfRXPacket[40];
                        externalCRSFdata.lastCRSFparameter.power = crsfRXPacket[40];
                        externalRFprarmeter.power = crsfRXPacket[40];
                    }
                    else if(crsfRXPacket[6]==1||crsfRXPacket[6]==2)
                    {
                        maxPackSize = 32;
                    }
                }
                else if(externalCRSFdata.regulatoryDomainIndex == NANO_TX_915Mhz)
                {
                    if(crsfRXPacket[6]==0 && crsfRXPacket[29]==48)
                    {
                        externalCRSFdata.crsfParameter.power = crsfRXPacket[31];
                        externalCRSFdata.lastCRSFparameter.power = crsfRXPacket[31];
                        externalRFprarmeter.power = crsfRXPacket[31];
                    }
                    else if(crsfRXPacket[6]==1||crsfRXPacket[6]==2)
                    {
                        maxPackSize = 32;
                    }
                }
                
                break;
                
            case regulatoryDomain:
                if(crsfRXPacket[13]=='2' &&(crsfRXPacket[14]=='4'||crsfRXPacket[15]=='4') )
                {
                    externalCRSFdata.regulatoryDomainIndex = NANO_TX_2400Mhz;
                }
                else if(crsfRXPacket[13]=='9' && crsfRXPacket[15]=='0')
                {
                    externalCRSFdata.regulatoryDomainIndex = NANO_TX_915Mhz;
                }
                break;
                
            default:
                break;       
        }
    }
    else if(maxPackSize == 32)
    {
         switch(crsfRXPacket[5])
        {
            case rate:
                if(crsfRXPacket[6]==0 && crsfRXPacket[22]==41)
                {
                    externalCRSFdata.crsfParameter.rate = crsfRXPacket[24];
                    externalCRSFdata.lastCRSFparameter.rate = crsfRXPacket[24];
                    externalRFprarmeter.rate = crsfRXPacket[24];
                }
                break;
            case tlm:
                if(crsfRXPacket[6]==0 && crsfRXPacket[8]==50)
                {
                    externalCRSFdata.crsfParameter.TLM = crsfRXPacket[10];
                    externalCRSFdata.lastCRSFparameter.TLM = crsfRXPacket[10];
                    externalRFprarmeter.TLM =  crsfRXPacket[10];
                }
                break;
            case power:
                if(crsfRXPacket[6]==0 && crsfRXPacket[14]==48)
                {
                    externalCRSFdata.crsfParameter.power = crsfRXPacket[16];
                    externalCRSFdata.lastCRSFparameter.power = crsfRXPacket[16];
                    externalRFprarmeter.power = crsfRXPacket[16];
                }
                break;
            case regulatoryDomain:
                if(crsfRXPacket[13]=='2' && crsfRXPacket[15]=='4')
                        externalCRSFdata.regulatoryDomainIndex = NANO_TX_2400Mhz;
                else if(crsfRXPacket[13]=='9' && crsfRXPacket[15]=='0')
                    externalCRSFdata.regulatoryDomainIndex = NANO_TX_915Mhz;
                break;
            default:
                break;       
        }
    }
    
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
