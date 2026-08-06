#ifndef __DS162_CH4_METER_H
#define __DS162_CH4_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 162

#include "type_def.h"

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

#endif
#endif /* __DS162_CH4_METER_H */