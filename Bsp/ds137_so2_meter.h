#ifndef __DS137_SO2_METER_H
#define __DS137_SO2_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 137

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void SO2_Init(void);

void SO2_Read(float *dat);

#endif
#endif