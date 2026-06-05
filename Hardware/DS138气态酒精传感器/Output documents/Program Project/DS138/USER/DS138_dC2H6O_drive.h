#ifndef	__DS138_dC2H6O_drive_H
#define	__DS138_dC2H6O_drive_H

#include "type_def.h"

//========================================================================
// 函数: void airC2H6O_Init(void)
// 描述: 气态酒精传感器的ADC初始化.
// 参数: none.
// 返回: none.
void airC2H6O_Init(void);

//========================================================================
// 函数: void airC2H6O_Read(void)
// 描述: 根据ADC的值转换出空气中的酒精含量.
// 参数: none.
// 返回: 酒精含量数值.
float airC2H6O_Read(void);

#endif