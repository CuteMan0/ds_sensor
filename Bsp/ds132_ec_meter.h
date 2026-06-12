#ifndef __DS132_EC_METER_H
#define __DS132_EC_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 132

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

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