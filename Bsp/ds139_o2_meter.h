#ifndef	__DS139_O2_METER_H
#define	__DS139_O2_METER_H

#include "ds_sensor.h"
#if DS_SENSOR == 139

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

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
void O2_Read(float* dat);

#endif
#endif