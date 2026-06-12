#ifndef __DS133_EC_MULTI_METER_H
#define __DS133_EC_MULTI_METER_H

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

void ds_init(void);
void ds_update(float *dat);

void ProcessCalibration(void);
void Led_Task(void);

#endif
#endif