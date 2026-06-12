#ifndef __DS163_NH4_METER_H
#define __DS163_NH4_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 163

#include "type_def.h"

void ds_init(void);
void ds_update(float *dat);

#endif
#endif /* __DS163_NH4_METER_H */