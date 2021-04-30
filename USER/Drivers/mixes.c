#include "mixes.h"
#include "switches.h"
#include "buzzer.h"
//mixchannel_t mixchannel;
QueueHandle_t mixesdataVal_Queue = NULL;


void mixes_init(mixdata_t* mixdata)
{
    mixdata[0].GimbalChannel=mix_THROTTLE;
    mixdata[1].GimbalChannel=mix_AILERON;
    mixdata[2].GimbalChannel=mix_RUDDER;
    mixdata[3].GimbalChannel=mix_ELEVATOR;
    mixdata[4].GimbalChannel=mix_SWA;
    mixdata[5].GimbalChannel=mix_SWB;
    mixdata[6].GimbalChannel=mix_SWC;
    mixdata[7].GimbalChannel=mix_SWD;
    
    mixdata[0].mix_inverse = 0;
    mixdata[1].mix_inverse = 0;
    mixdata[2].mix_inverse = 0;
    mixdata[3].mix_inverse = 0;
    mixdata[4].mix_inverse = 0;
    mixdata[5].mix_inverse = 0;
    mixdata[6].mix_inverse = 0;
    mixdata[7].mix_inverse = 0;
}

uint16_t mixes_inverse(uint8_t inverse, uint16_t gimbal_val_curr)
{
    if(inverse)
    {
        if(gimbal_val_curr > CHANNEL_OUTPUT_MID) 
        {
            gimbal_val_curr = CHANNEL_OUTPUT_MID - (gimbal_val_curr - CHANNEL_OUTPUT_MID);

        }
        else		            
        {
            gimbal_val_curr = CHANNEL_OUTPUT_MID + (CHANNEL_OUTPUT_MID - gimbal_val_curr);            
        }
    }
    return gimbal_val_curr;
}

void mixesTask(void* param)
{
    mixdata_t mixdata[8];
    static uint8_t mixes_data[8];
    uint16_t report_data[8];
    uint16_t gimbal_val_buff[4];
    uint16_t switches_val_buff[4];
    mixesdataVal_Queue = xQueueCreate(20,sizeof(mixes_data));
    mixes_init(mixdata);
    while(1)
	{
		vTaskDelay(10);  
//        xQueueReceive(gimbalVal_Queue,gimbal_val_buff,0);
//        xQueueReceive(switchesVal_Queue,switches_val_buff,0);
          	//RUDDER   = 0 ,       //yaw
            //THROTTLE = 1 ,       //throttle
            //AILERON  = 2 ,       //roll
            //ELEVATOR = 3 ,       //pitch
        report_data[0] = gimbal_val_buff[0];
		report_data[1] = gimbal_val_buff[1];
		report_data[2] = gimbal_val_buff[2];
		report_data[3] = gimbal_val_buff[3];
		
		report_data[4] = switches_val_buff[0];
		report_data[5] = switches_val_buff[1];
		report_data[6] = switches_val_buff[2];
		report_data[7] = switches_val_buff[3];        
        uint8_t mix_index = 0;
        for(mix_index = 0;mix_index < 8;mix_index++)
        {
            if(mixdata[mix_index].GimbalChannel < 4)
            {
                mixdata[mix_index].mix_output_data =  report_data[mixdata[mix_index].GimbalChannel];
                mixdata[mix_index].mix_output_data = mixes_inverse(mixdata[mix_index].mix_inverse,mixdata[mix_index].mix_output_data);
            }
        }
        
        xQueueSend(mixesdataVal_Queue,report_data,0);
    }
}
