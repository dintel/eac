#include "delta.h"
#include "log.h"
#include <math.h>

uint8_t nw_change_encode(int prev,int next)
{
    PRINT_DEBUG("nw_change_encode: prev %d next %d enc %d",prev,next,(int)log2(next));
    return (uint8_t)log2(next);
}

int nw_change_decode(int prev, uint8_t delta)
{
    PRINT_DEBUG("nw_change_decode: prev %d delta %d dec %d",prev,delta,(int)pow(2,delta));
    return pow(2,delta);
}
