#include "Bsp_PPM.h"
#include "gpio.h"
#include "tim.h"



#define PPM_OUTPUT_GPIO_Port    GPIOA
#define PPM_OUTPUT_Pin          GPIO_PIN_11



/* ======================================== GPIO init ======================================== */
void BspPpm_GpioInit(void){
    /* 设置PA11为推挽输出，注意不要启动USB初始化 */
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin     = PPM_OUTPUT_Pin;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull    = GPIO_NOPULL;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PPM_OUTPUT_GPIO_Port, &GPIO_InitStruct);
}



/* ======================================== Timer init ======================================== */
void BspPpm_TimerInit(void){
    /* 重新初始化tim1 */
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 72-1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000-1;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  // 不要开启自动重装
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK){
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK){
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK){
        Error_Handler();
    }
}



/* ======================================== bsp ppm init ======================================== */
uint16_t bspPpmBuf[PPM_CHANNEL_NUM] = {
    PPM_CHANNEL_MID_VALUE, PPM_CHANNEL_MID_VALUE, 
    PPM_CHANNEL_MID_VALUE, PPM_CHANNEL_MID_VALUE,
    PPM_CHANNEL_MID_VALUE, PPM_CHANNEL_MID_VALUE, 
    PPM_CHANNEL_MID_VALUE, PPM_CHANNEL_MID_VALUE,
};

bsp_ppm_t bspPpm = { 0 };



static void bspPpm_SendChannel(ppm_ch_t* ch){
    if(bspPpm.status == BSP_PPM_STATUS_BUSY)return;

    bspPpm.status = BSP_PPM_STATUS_BUSY;
    // 装填数据
    for(uint8_t i=0; i<ch->len; i++){
        if(ch->buf[i] < PPM_CHANNEL_MIN_VALUE){
            bspPpm.ch.buf[i] = PPM_CHANNEL_MIN_VALUE;
        }else if(ch->buf[i] > PPM_CHANNEL_MAX_VALUE){
            bspPpm.ch.buf[i] = PPM_CHANNEL_MAX_VALUE;
        }else{
            bspPpm.ch.buf[i] = ch->buf[i];
        }
    }
    // 启动PPM
    bspPpm.step = 0;
    bspPpm.time = 20000;                // 2000us * 10
    __HAL_TIM_SET_COUNTER(&htim1, 0);   // 复位计数器的值
    bspPpm_IRQHandler();
}


ppm_if_t* bspPpm_Init(void){
    // Hardware init
    BspPpm_GpioInit();
    BspPpm_TimerInit();         // 初始化us定时器
    // struct init
    bspPpm.interface._send = bspPpm_SendChannel;
    bspPpm.ch.buf = bspPpmBuf;
    bspPpm.ch.len = PPM_CHANNEL_NUM;
    bspPpm.status = BSP_PPM_STATUS_IDLE;
    bspPpm.step = 0;
    bspPpm.time = 0;

    return &bspPpm.interface;
}



/* 放在中断服务中调用 */
void bspPpm_IRQHandler(void){
    if(bspPpm.status == BSP_PPM_STATUS_IDLE)return;

    if(bspPpm.step <= 16){
        if((bspPpm.step&1) != 0){
            HAL_GPIO_WritePin(PPM_OUTPUT_GPIO_Port, PPM_OUTPUT_Pin, GPIO_PIN_SET);
            __HAL_TIM_SET_AUTORELOAD(&htim1, bspPpm.ch.buf[bspPpm.step/2]-1);
            bspPpm.time -= bspPpm.ch.buf[bspPpm.step/2];
            HAL_TIM_Base_Start_IT(&htim1);
        }else{
            HAL_GPIO_WritePin(PPM_OUTPUT_GPIO_Port, PPM_OUTPUT_Pin, GPIO_PIN_RESET);
            __HAL_TIM_SET_AUTORELOAD(&htim1, 500-1);
            bspPpm.time -= 500;
            HAL_TIM_Base_Start_IT(&htim1);
        }
    }else if(bspPpm.step == 17){
        HAL_GPIO_WritePin(PPM_OUTPUT_GPIO_Port, PPM_OUTPUT_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_AUTORELOAD(&htim1, bspPpm.time-1);
        HAL_TIM_Base_Start_IT(&htim1);
    }else{
        bspPpm.step = 0;
        bspPpm.time = 0;
        bspPpm.status = BSP_PPM_STATUS_IDLE;
        return;
    }

    bspPpm.step++;
}


