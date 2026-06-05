#ifndef	__DS135_TUR_drive_H
#define	__DS135_TUR_drive_H

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>


extern float ref_vol;
extern float adc_vol;
//========================================================================
// 函数: void Tur_Init(void)
// 描述: 浊度传感器的ADC和控制IO初始化.
// 参数: none.
// 返回: none.
void Tur_Init(void);

//========================================================================
// 函数: float Tur_Read(void)
// 描述: 根据ADC采样转换获取浊度值.
// 参数: none.
// 返回: 浊度值.
float Tur_Read(void);

#endif