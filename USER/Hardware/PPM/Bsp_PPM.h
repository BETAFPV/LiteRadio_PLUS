#ifndef __Bsp_PPM_H
#define __Bsp_PPM_H



#include "PPM.h"



typedef enum
{
    BSP_PPM_STATUS_IDLE = 0x00U,
    BSP_PPM_STATUS_BUSY = 0x01U,
}bsp_ppm_status_e;

typedef struct bsp_ppm_s
{
    ppm_if_t                    interface;
    ppm_ch_t                    ch;
    uint8_t                     status;     //bsp_ppm_status_e
    uint8_t                     step;
    uint16_t                    time;       //The time remaining for each transmission, unit = ms
}bsp_ppm_t;



ppm_if_t* bspPpm_Init(void);
void bspPpm_IRQHandler(void);



#endif /* __Bsp_PPM_H */
