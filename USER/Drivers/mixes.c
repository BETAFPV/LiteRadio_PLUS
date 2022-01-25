#include "mixes.h"
#include "switches.h"
#include "buzzer.h"
#include "tim.h"
#include "usbd_custom_hid_if.h"
#include "stmflash.h"
#include "radiolink.h"
#include "status.h"
#include "function.h"
#include "crsf.h"
static uint32_t mixesDelayTime;
UBaseType_t uxTaskGetStackHighWaterMarkdebug;
TaskHandle_t mixesTaskHandle;
QueueHandle_t mixesValQueue = NULL;
mixData_t mixData[8];
uint16_t mixesBuff[8];
uint8_t mixUpdateFlag;
uint16_t controlMode;
    
void Mixes_Init()
{   
    for(int i=0;i<8;i++)
    {
        Mixes_ChannelInit(i);
    }
}

void Mixes_Update()
{   
    for(int i=0;i<8;i++)
    {
        Mixes_ChannelUpdate(i);
    }
}

void Mixes_ChannelInit(uint8_t channel)
{
    STMFLASH_Read(MIX_CHANNEL_INFO_ADDR+channel*8,mixesBuff,4);   
    /*混控设置自检*/
    if((mixesBuff[0] > 8) || (mixesBuff[1] > 1) || (mixesBuff[2] > 100) || (mixesBuff[3] > 200))
    {
        mixesBuff[0] = channel;
        mixesBuff[1] = 0;
        mixesBuff[2] = 100;
        mixesBuff[3] = 100;
        STMFLASH_Write(MIX_CHANNEL_INFO_ADDR+channel*8,mixesBuff,4);  
        STMFLASH_Write(CACHE_MIX_CHANNEL_INFO_ADDR+channel*8,mixesBuff,4);  
    }

    mixData[channel].gimbalChannel = (uint8_t)mixesBuff[0];
    mixData[channel].reverse= (uint8_t)mixesBuff[1];
    mixData[channel].weight = (uint8_t)mixesBuff[2];
    mixData[channel].offset = (uint8_t)mixesBuff[3];
}

void Mixes_ChannelUpdate(uint8_t channel)
{
    STMFLASH_Read(CACHE_MIX_CHANNEL_INFO_ADDR+channel*8,mixesBuff,4);     
    mixData[channel].gimbalChannel = (uint8_t)mixesBuff[0];
    mixData[channel].reverse= (uint8_t)mixesBuff[1];
    mixData[channel].weight = (uint8_t)mixesBuff[2];
    mixData[channel].offset = (uint8_t)mixesBuff[3];
}

/*通道补偿运算*/
uint16_t Mixes_GimbalOffset(uint8_t offset, uint16_t gimbalValCurr)
{
    if(offset < 100)
    {
        gimbalValCurr = gimbalValCurr - 500 + map(offset,0,100,0,500);
    }
    else
    {
        gimbalValCurr = gimbalValCurr + map(offset,100,200,0,500);
    }
    return gimbalValCurr;
}

/*通道比例运算*/
uint16_t Mixes_GimbalWeight(uint8_t weight, uint16_t gimbalValCurr)
{
    if(gimbalValCurr > CHANNEL_OUTPUT_MID) 
    {
        gimbalValCurr = CHANNEL_OUTPUT_MID + (gimbalValCurr - CHANNEL_OUTPUT_MID)* (uint16_t)weight /100;
    }
    else		            
    {
        gimbalValCurr = CHANNEL_OUTPUT_MID - (CHANNEL_OUTPUT_MID - gimbalValCurr)* (uint16_t)weight /100;            
    }
    return gimbalValCurr;
}

/*通道反向运算*/
uint16_t Mixes_Gimbalreverse(uint8_t reverse, uint16_t gimbalValCurr,uint16_t* outputcode)
{
    if(reverse)
    {
        if(gimbalValCurr > CHANNEL_OUTPUT_MID) 
        {
            gimbalValCurr = CHANNEL_OUTPUT_MID - outputcode[gimbalValCurr - CHANNEL_OUTPUT_MID];
        }
        else
        {
            gimbalValCurr = CHANNEL_OUTPUT_MID + outputcode[CHANNEL_OUTPUT_MID - gimbalValCurr];            
        }
    }
    else
    {
        if(gimbalValCurr > CHANNEL_OUTPUT_MID) 
        {
            gimbalValCurr = CHANNEL_OUTPUT_MID + outputcode[gimbalValCurr - CHANNEL_OUTPUT_MID];

        }
        else   
        {
            gimbalValCurr = CHANNEL_OUTPUT_MID - outputcode[CHANNEL_OUTPUT_MID - gimbalValCurr];            
        }    
    }
    return gimbalValCurr;
}
/*开关通道反向操作*/
uint16_t Mixes_Switchreverse(uint8_t reverse, uint16_t gimbalValCurr)
{
    if(reverse)
    {
        if(gimbalValCurr > CHANNEL_OUTPUT_MID) 
        {
            gimbalValCurr = CHANNEL_OUTPUT_MID - (gimbalValCurr - CHANNEL_OUTPUT_MID);
        }
        else
        {
            gimbalValCurr = CHANNEL_OUTPUT_MID + (CHANNEL_OUTPUT_MID - gimbalValCurr);            
        }
    }
    return gimbalValCurr;
}

uint16_t Mixes_ValueLimit(uint16_t gimbalValCurr)
{
    if(gimbalValCurr > MIX_CHANNEL_OUTPUT_MAX)
    {
        gimbalValCurr = MIX_CHANNEL_OUTPUT_MAX;
    }
    else if(gimbalValCurr < MIX_CHANNEL_OUTPUT_MIN)
    {
        gimbalValCurr = MIX_CHANNEL_OUTPUT_MIN;
    }
    return gimbalValCurr;
}

//混控任务
uint16_t OutputCode[513];
void mixesTask(void* param)
{
    uint16_t JoystickDeadZonePercent = 0;
    
    STMFLASH_Read(JoystickDeadZonePercent_ADDR,&JoystickDeadZonePercent,1);
    if(JoystickDeadZonePercent<MinDeadZonePercent||JoystickDeadZonePercent>MaxDeadZonePercent)
    {
        JoystickDeadZonePercent = DeafultDeadZonePercent;
        STMFLASH_Write(JoystickDeadZonePercent_ADDR,&JoystickDeadZonePercent,1);
    }
    for(int i=0;i<513;i++)
    {
        if(i<JoystickDeadZonePercent*5)
        {
            OutputCode[i] = 0;
        }
        else
        {
            OutputCode[i] = (i-JoystickDeadZonePercent*5)+i*JoystickDeadZonePercent/100;
        }
    }
    for(int i=510;i<513;i++)
    {
        OutputCode[i] = 512;
    }
    
    
    uint8_t mixIndex;
//    uint16_t OutputCode[513] =      //摇杆ADC映射表
//    {
////        0,0,0,0,0,
////        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
////        21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
////        41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,
////        61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
////        81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
////        101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
////        121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,
////        141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,
////        161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,
////        181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,
////        201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,
////        221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,
////        241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,
////        261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,
////        281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,
////        301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,
////        321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,
////        341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,
////        361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,
////        381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,
////        401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,
////        421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,
////        441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,458,459,460,
////        461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,
////        481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,
////        500,500,500,500,500,500,500,
//		
//				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,5,7,9,11,13,15,17,19,
//        21,23,25,27,29,31,33,35,37,39,
//        41,43,45,47,49,51,53,55,57,59,
//        61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
//        81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
//        101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
//        121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,
//        141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,
//        161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,
//        181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,
//        201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,
//        221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,
//        241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,
//        261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,
//        281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,
//        301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,
//        321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,
//        341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,
//        361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,
//        381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,
//        401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,
//        421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,
//        441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,458,459,460,
//        461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,
//        481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,
//        501,502,503,504,505,506,507,
//        508,509,510,511,512,512,512,512,512,512,
//        512,512,

//    };
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
        508,509,510,511,512,512,512,512,512,512,
        512,512,512,512,512,512,512,512,512,512,
        512,512,512,512,512,512,512,512,512,512,
    };
    uint16_t mixesBuff[8];
    uint16_t gimbalVaBuff[4];
    uint16_t switchesValBuff[4];

    mixesDelayTime = Get_ProtocolDelayTime();
    STMFLASH_Read(CONFIGER_INFO_MODE_ADDR,&controlMode,1);
    /*模式自检*/
    if(1 < controlMode)
    {
        controlMode = 0;
        STMFLASH_Write(CONFIGER_INFO_MODE_ADDR,&controlMode,1);
    }
    mixesValQueue = xQueueCreate(20,sizeof(mixesBuff));
    Mixes_Init();
    while(1)
    {
        vTaskDelay(mixesDelayTime);  
        
        if(mixUpdateFlag == 0x01)
        {         
            Mixes_Update();
            mixUpdateFlag = 0x00;
        }
        
        xQueueReceive(gimbalValQueue,gimbalVaBuff,0);
        xQueueReceive(switchesValQueue,switchesValBuff,0);
        
        /*日本手模式1，美国手模式0*/
        if(controlMode == 1)
        {
            mixesBuff[AILERON] = gimbalVaBuff[AILERON];
            mixesBuff[ELEVATOR] = gimbalVaBuff[THROTTLE];
            mixesBuff[THROTTLE] = gimbalVaBuff[ELEVATOR];
            mixesBuff[RUDDER] = gimbalVaBuff[RUDDER];        
        }
        else
        {
            mixesBuff[AILERON] = gimbalVaBuff[AILERON];
            mixesBuff[ELEVATOR] = gimbalVaBuff[ELEVATOR];  
            mixesBuff[THROTTLE] = gimbalVaBuff[THROTTLE];
            mixesBuff[RUDDER] = gimbalVaBuff[RUDDER];            
        }
        
        mixesBuff[4] = switchesValBuff[0];
        mixesBuff[5] = switchesValBuff[1];
        mixesBuff[6] = switchesValBuff[2];
        mixesBuff[7] = switchesValBuff[3];   
        for(mixIndex = 0;mixIndex < 8;mixIndex++)
        {
            
            if(mixData[mixIndex].gimbalChannel < 4)/*选取的为摇杆通道值，设置混控*/
            {
                switch (mixData[mixIndex].gimbalChannel)
                {
                    case MIX_AILERON:
                    {
                        mixData[mixIndex].output = mixesBuff[AILERON];
                        break;
                    }
                    case MIX_ELEVATOR:
                    {
                        mixData[mixIndex].output = mixesBuff[ELEVATOR];
                        break;
                    }
                    case MIX_THROTTLE:
                    {
                        mixData[mixIndex].output = mixesBuff[THROTTLE];
                        break;
                    } 
                    case MIX_RUDDER:
                    {
                        mixData[mixIndex].output = mixesBuff[RUDDER];
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                if(mixData[mixIndex].gimbalChannel == MIX_THROTTLE)
                {
                    /*油门通道映射*/
                    mixData[mixIndex].output = Mixes_Gimbalreverse(mixData[mixIndex].reverse, mixData[mixIndex].output, THROTTLE_OutputCode);
                }
                else
                {
                    /*非油门通道映射*/
                    mixData[mixIndex].output = Mixes_Gimbalreverse(mixData[mixIndex].reverse, mixData[mixIndex].output, OutputCode);
                }

                /*通道补偿*/
                mixData[mixIndex].output = Mixes_GimbalOffset(mixData[mixIndex].offset, mixData[mixIndex].output);

                /*通道比例*/
                mixData[mixIndex].output = Mixes_GimbalWeight(mixData[mixIndex].weight, mixData[mixIndex].output);
                mixData[mixIndex].output = Mixes_ValueLimit(mixData[mixIndex].output);
            }
            if(mixData[mixIndex].gimbalChannel >= 4)/*选取的为开关通道值，不设置混控*/
            {
                mixData[mixIndex].output =  mixesBuff[mixData[mixIndex].gimbalChannel];
                mixData[mixIndex].output = Mixes_Switchreverse(mixData[mixIndex].reverse, mixData[mixIndex].output);
            }
        }
        
        mixesBuff[0] = mixData[0].output;
        mixesBuff[1] = mixData[1].output;
        mixesBuff[2] = mixData[2].output;
        mixesBuff[3] = mixData[3].output;
        mixesBuff[4] = mixData[4].output;
        mixesBuff[5] = mixData[5].output;
        mixesBuff[6] = mixData[6].output;        
        mixesBuff[7] = mixData[7].output;                      
        xQueueSend(mixesValQueue,mixesBuff,0);
        if(externalCRSFdata.lastConfigStatus == CONFIG_CRSF_ON && externalCRSFdata.configStatus == CONFIG_CRSF_ON)
        {
            xQueueSend(mixesValQueue,mixesBuff,0);    
        }
        if(internalCRSFdata.lastConfigStatus == CONFIG_CRSF_ON && internalCRSFdata.configStatus == CONFIG_CRSF_ON)
        {
            xQueueSend(mixesValQueue,mixesBuff,0);    
        }
    }
}


