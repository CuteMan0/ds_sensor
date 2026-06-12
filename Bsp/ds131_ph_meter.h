#ifndef __DS131_PH_METER_H
#define __DS131_PH_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 131

#include "type_def.h"

void ds_init(void);
void ds_update(float *dat);

#endif
#endif