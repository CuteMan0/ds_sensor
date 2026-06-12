#ifndef __DS112_FIR_METER_H
#define __DS112_FIR_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 112

#include "type_def.h"

void ds_init(void);
void ds_update(float *dat);

#endif
#endif