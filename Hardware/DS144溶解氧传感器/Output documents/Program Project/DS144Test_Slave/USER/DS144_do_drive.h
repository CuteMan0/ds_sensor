#ifndef	__DS132_EC_drive_H
#define	__DS132_EC_drive_H

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>


//========================================================================
// 函数: void Tur_Init(void)
// 描述: 浊度传感器的ADC和控制IO初始化.
// 参数: none.
// 返回: none.
void Do_Init(void);

//========================================================================
// 函数: float Tur_Read(void)
// 描述: 根据ADC采样转换获取浊度值.
// 参数: none.
// 返回: 浊度值.
float Do_Read(void);

#endif