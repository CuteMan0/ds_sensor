#ifndef __DS144_dO_METER_H
#define __DS144_dO_METER_H

#include "ds_sensor.h"
#if DS_SENSOR == 144

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void DO_Init(void);

void DO_Read(float *dat);

void ProcessCalibration(void);
void Led_Task(void);

#endif
#endif