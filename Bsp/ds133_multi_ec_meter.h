#ifndef __EC_MULTI_METER_H
#define __EC_MULTI_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 133

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum
{
    EC_RANGE_20MS = 0,
    EC_RANGE_2MS,
    EC_RANGE_0P2MS
} EC_Range_t;

#define RANGE_SWITCH_CNT 10

typedef void (*pfState)(void);
typedef enum eEvent
{
    EVT_NO_EVENT,
    EVT_OverRange,
    EVT_UnderRange
} eEvent;

typedef struct stStmRow
{
    pfState pSrcState;
    eEvent eEvt;
    pfState pDestState;
} StmRow_t;

//========================================================================
// 函数: void EC_Init(void)
// 描述: 电导率传感器的ADC和量程切换IO初始化.
// 参数: none.
// 返回: none.
void ec_init(void);

//========================================================================
// 函数: float EC_Read(void)
// 描述: 根据ADC和量程模式获取电导率数值.
// 参数: none.
// 返回: 电导率数值.
void ec_read(float *ec_val);

void ProcessCalibration(void);
void Led_Task(void);

#endif
#endif