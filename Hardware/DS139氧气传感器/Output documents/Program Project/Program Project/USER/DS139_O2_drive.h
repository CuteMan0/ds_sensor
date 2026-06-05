#ifndef	__DS145_O2_drive_H
#define	__DS145_O2_drive_H

#include "type_def.h"

//========================================================================
// 函数: void O2_Init(void)
// 描述: 氧气传感器的ADC初始化.
// 参数: none.
// 返回: none.
void O2_Init(void);

//========================================================================
// 函数: void O2_Read(void)
// 描述: 根据ADC的值转换氧气含量.
// 参数: none.
// 返回: 酸碱度pH值.
float O2_Read(void);

#endif