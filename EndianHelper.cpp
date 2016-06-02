#include "EndianHelper.h"
#include "stdint.h"

double byteswap(double v)
{
    union {
        uint64_t i;
        double  d;
    } conv;
    conv.d = v;
    conv.i = (conv.i & 0x00000000FFFFFFFF) << 32 | (conv.i & 0xFFFFFFFF00000000) >> 32;
    conv.i = (conv.i & 0x0000FFFF0000FFFF) << 16 | (conv.i & 0xFFFF0000FFFF0000) >> 16;
    conv.i = (conv.i & 0x00FF00FF00FF00FF) << 8  | (conv.i & 0xFF00FF00FF00FF00) >> 8;
    return conv.d;
}

