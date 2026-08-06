#ifndef __DS151_SOIL_TEMP_METER_H
#define __DS151_SOIL_TEMP_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 151

#include "type_def.h"

void ds_init(void);
void ds_update(float *dat);

#endif
#endif