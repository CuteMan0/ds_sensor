#ifndef __DS138_AIRC2H6O_METER_H
#define __DS138_AIRC2H6O_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 138

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void airC2H6O_Init(void);

u8 airC2H6O_Read(float *val);

void ProcessCalibration(void);

#endif
#endif