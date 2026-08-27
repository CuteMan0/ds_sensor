#ifndef __LUT_H__
#define __LUT_H__

#include "type_def.h"

typedef struct
{
    u16 x; // 输入量（ADC / mV / 等）
    u16 y; // 输出量（ppm / 温度 / 等）
} LUT_2D;

u16 LUT_BinaryInterp_u16(const LUT_2D *tab, u8 size, u16 x);

#endif