#ifndef __DS131_PH_METER_H
#define __DS131_PH_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 131

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

#endif
#endif