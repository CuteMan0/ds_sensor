#include "lut.h"

u16 LUT_BinaryInterp_u16(const LUT_2D *tab, u8 size, u16 x)
{
    u8 low, high, mid;
    u16 x1, x2, y1, y2;
    u32 y;

    low = 0;
    high = size - 1;

    // 边界处理
    if (x <= tab[0].x)
        return tab[0].y;

    if (x >= tab[high].x)
        return tab[high].y;

    // 二分查找区间
    while ((high - low) > 1)
    {
        mid = (low + high) >> 1;

        if (x < tab[mid].x)
            high = mid;
        else
            low = mid;
    }

    // 插值区间：low ~ high
    x1 = tab[low].x;
    x2 = tab[high].x;
    y1 = tab[low].y;
    y2 = tab[high].y;

    if (x2 == x1)
        return y1;

    // 线性插值
    y = y1 +
        (u32)(x - x1) * (y2 - y1) /
            (x2 - x1);

    return (u16)y;
}