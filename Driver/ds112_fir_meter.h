#ifndef __DS112_FIR_METER_H
#define __DS112_FIR_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 112

#include "type_def.h"

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

#endif
#endif