#ifndef __DS145_dCO2_METER_H
#define __DS145_dCO2_METER_H

#include "ds_sensor.h"
#if DS_SENSOR == 145

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void ds_init(void);
void ds_update(float *dat);

#endif
#endif