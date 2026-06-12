#ifndef __DS138_AIRC2H6O_METER_H
#define __DS138_AIRC2H6O_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 138

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void ds_init(void);
u8 ds_update(float *dat);
void ProcessCalibration(void);

#endif
#endif