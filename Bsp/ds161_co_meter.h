#ifndef __DS161_CO_METER_H
#define __DS161_CO_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 161

#include "type_def.h"

extern volatile u32 sys_tick;

void ds_init(void);
void ds_update(float *dat);

#endif
#endif /* __DS161_CO_METER_H */