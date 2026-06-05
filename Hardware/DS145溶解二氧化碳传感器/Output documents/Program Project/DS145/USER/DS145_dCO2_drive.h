#ifndef	__DS145_dCO2_drive_H
#define	__DS145_dCO2_drive_H

#include "type_def.h"

//========================================================================
// 函数: void dCO2_Init(void)
// 描述: 溶解二氧化碳传感器的ADC初始化.
// 参数: none.
// 返回: none.
void dCO2_Init(void);

//========================================================================
// 函数: void dCO2_Read(void)
// 描述: 根据ADC的值转换溶解二氧化碳含量.
// 参数: none.
// 返回: 酸碱度pH值.
float dCO2_Read(void);

#endif