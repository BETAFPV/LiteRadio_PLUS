#include "frsky_d16.h"
#include "cc2500.h"
#include "function.h"

void frskyd16Task(void* param)
{
    CC2500_Init();
    while(1)
	{
        vTaskDelay(5);
        
    }
}


