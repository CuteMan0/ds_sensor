#ifndef __PH_METER_H
#define __PH_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 131

#include "type_def.h"

//========================================================================
// 函数: void PH_Init(void)
// 描述: 酸碱度传感器的ADC初始化.
// 参数: none.
// 返回: none.
void pH_init(void);

//========================================================================
// 函数: void PH_Read(void)
// 描述: 根据ADC的值和酸碱度测量关系转换出pH值.
// 参数: none.
// 返回: 酸碱度pH值.
void pH_read(float *ph_val);

#endif
#endif