#ifndef __DS165_170_ISE_METER_H
#define __DS165_170_ISE_METER_H

#include "ds_sensor.h"

#if (DS_SENSOR == 165 || DS_SENSOR == 166 || DS_SENSOR == 167 || DS_SENSOR == 168 || DS_SENSOR == 169 || DS_SENSOR == 170)

#include "type_def.h"

// 使用说明：
/*
校准步骤：
长按 → 进入校准模式，2号灯闪烁（等待第一次短按）

第一次短按 → 2号灯常亮（正在采样100ppm）

采样完成 → 3号灯闪烁（提示1000ppm校准，等待第二次短按）

第二次短按 → 3号灯常亮（正在采样1000ppm）

采样完成 → 3号灯闪烁6次后常亮（校准完成）
*/

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

void Led_Task(void);

#endif
#endif