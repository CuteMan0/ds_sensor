#ifndef __DS134_CHROMA_METER_H
#define __DS134_CHROMA_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 135

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

extern float ref_vol;
extern float adc_vol;
//========================================================================
// 函数: void tur_Init(void)
// 描述: 浊度传感器的ADC和控制IO初始化.
// 参数: none.
// 返回: none.
void tur_init(void);

//========================================================================
// 函数: float tur_Read(void)
// 描述: 根据ADC采样转换获取浊度值.
// 参数: 浊度值数据指针.
// 返回: .
void tur_read(float *tur_val);

#endif
#endif