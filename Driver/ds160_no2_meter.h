#ifndef __DS160_NO2_METER_H
#define __DS160_NO2_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 160

#include "type_def.h"

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

#endif
#endif /* __DS160_NO2_METER_H */