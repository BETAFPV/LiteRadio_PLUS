#include "xinput.h"



uint8_t isXboxMode = 0;



int32_t xinputMap(int32_t pos, int32_t i1, int32_t i2, int32_t o1, int32_t o2)
{
    return (pos-i1) * (o2-o1) / (i2-i1) + o1;
}


