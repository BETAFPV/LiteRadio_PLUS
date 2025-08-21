#ifndef __PPM_H
#define __PPM_H



#include <stdio.h>
#include <stdint.h>



#define PPM_CHANNEL_NUM         8

#define PPM_CHANNEL_MIN_VALUE   500
#define PPM_CHANNEL_MID_VALUE   1000
#define PPM_CHANNEL_MAX_VALUE   1500



typedef struct ppm_channel_s
{
    uint16_t*                   buf;
    uint16_t                    len;
}ppm_ch_t;



typedef void (*ppm_send_t)(ppm_ch_t* ch);

typedef struct ppm_interface_s
{
    ppm_send_t                  _send;
}ppm_if_t;



#endif /* __PPM_H */
