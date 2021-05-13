#include "mixes.h"
#include "switches.h"
#include "buzzer.h"
TaskHandle_t mixesTaskHandle;
QueueHandle_t mixesdataVal_Queue = NULL;

//初始化混控的参数
void mixes_init(mixdata_t* mixdata)
{
    mixdata[0].GimbalChannel=mix_RUDDER;
    mixdata[1].GimbalChannel=mix_THROTTLE;
    mixdata[2].GimbalChannel=mix_AILERON;
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
    
    mixdata[0].mix_weight = 100;
    mixdata[1].mix_weight = 100;
    mixdata[2].mix_weight = 100;
    mixdata[3].mix_weight = 100;
    mixdata[4].mix_weight = 100;
    mixdata[5].mix_weight = 100;
    mixdata[6].mix_weight = 100;
    mixdata[7].mix_weight = 100;
    
    mixdata[0].mix_offset = 0;
    mixdata[1].mix_offset = 0;
    mixdata[2].mix_offset = 0;
    mixdata[3].mix_offset = 0;
    mixdata[4].mix_offset = 0;
    mixdata[5].mix_offset = 0;
    mixdata[6].mix_offset = 0;
    mixdata[7].mix_offset = 0;    
}


//摇杆通道补偿操作
uint16_t mixes_gimbal_offset(int16_t offset, uint16_t gimbal_val_curr)
{
    if(offset < 0)
    {
        uint16_t offset_abs;
        offset_abs = (uint16_t) - offset;
        gimbal_val_curr -= offset_abs;
    }
    else
    {
        gimbal_val_curr += offset;
    }
    return gimbal_val_curr;
}

//摇杆通道比例操作
uint16_t mixes_gimbal_weight(uint8_t weight, uint16_t gimbal_val_curr)
{
    if(gimbal_val_curr > CHANNEL_OUTPUT_MID) 
    {
        gimbal_val_curr = CHANNEL_OUTPUT_MID + (gimbal_val_curr - CHANNEL_OUTPUT_MID)* (uint16_t)weight /100;
    }
    else		            
    {
        gimbal_val_curr = CHANNEL_OUTPUT_MID - (CHANNEL_OUTPUT_MID - gimbal_val_curr)* (uint16_t)weight /100;            
    }
    return gimbal_val_curr;
}

//摇杆通道反向操作
uint16_t mixes_gimbal_inverse(uint8_t inverse, uint16_t gimbal_val_curr,uint16_t* outputcode)
{
    if(inverse)
    {
        if(gimbal_val_curr > CHANNEL_OUTPUT_MID) 
        {
            gimbal_val_curr = CHANNEL_OUTPUT_MID - outputcode[gimbal_val_curr - CHANNEL_OUTPUT_MID];
        }
        else		            
        {
            gimbal_val_curr = CHANNEL_OUTPUT_MID + outputcode[CHANNEL_OUTPUT_MID - gimbal_val_curr];            
        }
    }
    else
    {
        if(gimbal_val_curr > CHANNEL_OUTPUT_MID) 
        {
            gimbal_val_curr = CHANNEL_OUTPUT_MID + outputcode[gimbal_val_curr - CHANNEL_OUTPUT_MID];

        }
        else		            
        {
            gimbal_val_curr = CHANNEL_OUTPUT_MID - outputcode[CHANNEL_OUTPUT_MID - gimbal_val_curr];            
        }    
    }
    return gimbal_val_curr;
}
//开关通道反向操作
uint16_t mixes_sw_inverse(uint8_t inverse, uint16_t gimbal_val_curr)
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

//混控任务
void mixesTask(void* param)
{
    mixdata_t mixdata[8];
    static uint8_t mixes_data[8];
    uint16_t report_data[8];
    uint16_t gimbal_val_buff[4];
    uint16_t switches_val_buff[4];
    mixesdataVal_Queue = xQueueCreate(20,sizeof(report_data));
    uint16_t OutputCode[513] =      //摇杆ADC映射表
    {
        0,0,0,0,0,
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
        21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,
        61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
        81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
        101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
        121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,
        141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,
        161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,
        181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,
        201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,
        221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,
        241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,
        261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,
        281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,
        301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,
        321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,
        341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,
        361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,
        381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,
        401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,
        421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,
        441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,458,459,460,
        461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,
        481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,
        501,502,503,504,505,506,507,
    };
    uint16_t THROTTLE_OutputCode[513] =      //油门摇杆ADC映射表
    {
        0,0,0,0,0,0,1,3,5,7,9,11,13,15,17,19,
        21,23,25,27,29,31,33,35,37,39,
        41,43,45,47,49,51,53,55,57,59,
        61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
        81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
        101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
        121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,
        141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,
        161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,
        181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,
        201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,
        221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,
        241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,
        261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,
        281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,
        301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,
        321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,
        341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,
        361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,
        381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,
        401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,
        421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,
        441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,458,459,460,
        461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,
        481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,
        501,502,503,504,505,506,507,
        507,507,507,507,507,507,507,507,507,507,
        507,507,507,507,507,507,507,507,507,507,
        507,507,507,507,507,507,507,507,507,507,
    };
    mixes_init(mixdata);
    while(1)
	{
		vTaskDelay(9);  
        xQueueReceive(gimbalVal_Queue,gimbal_val_buff,0);
        xQueueReceive(switchesVal_Queue,switches_val_buff,0);
        uint8_t mix_index = 0;
        for(mix_index = 0;mix_index < 8;mix_index++)
        {
            mixdata[mix_index].mix_output_data =  report_data[mixdata[mix_index].GimbalChannel];
            if(mixdata[mix_index].GimbalChannel < 4)
            {
                
                if(mixdata[mix_index].GimbalChannel == mix_THROTTLE)
                {
                    //油门映射
                    mixdata[mix_index].mix_output_data = mixes_gimbal_inverse(mixdata[mix_index].mix_inverse, mixdata[mix_index].mix_output_data, THROTTLE_OutputCode);
                }
                else
                {
                    //非油门映射
                    mixdata[mix_index].mix_output_data = mixes_gimbal_inverse(mixdata[mix_index].mix_inverse, mixdata[mix_index].mix_output_data, OutputCode);
                }
                //通道补偿
                mixdata[mix_index].mix_output_data = mixes_gimbal_offset(mixdata[mix_index].mix_offset, mixdata[mix_index].mix_output_data);
                //通道比例
                mixdata[mix_index].mix_output_data = mixes_gimbal_weight(mixdata[mix_index].mix_weight, mixdata[mix_index].mix_output_data);
                
            }
            if(mixdata[mix_index].GimbalChannel >= 4)
            {
                mixdata[mix_index].mix_output_data = mixes_sw_inverse(mixdata[mix_index].mix_inverse, mixdata[mix_index].mix_output_data);
            }
        }
        
        report_data[0] = mixdata[0].mix_output_data;
        report_data[1] = mixdata[1].mix_output_data;
        report_data[2] = mixdata[2].mix_output_data;
        report_data[3] = mixdata[3].mix_output_data;
        report_data[4] = mixdata[4].mix_output_data;
        report_data[5] = mixdata[5].mix_output_data;
        report_data[6] = mixdata[6].mix_output_data;        
        report_data[7] = mixdata[7].mix_output_data;              
        
        xQueueSend(mixesdataVal_Queue,report_data,0);
    }
}
