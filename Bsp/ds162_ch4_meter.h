#ifndef __DS162_CH4_METER_H
#define __DS162_CH4_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 162

#include "type_def.h"

void ds_init(void);
void ds_update(float *dat);

#endif
#endif /* __DS162_CH4_METER_H */