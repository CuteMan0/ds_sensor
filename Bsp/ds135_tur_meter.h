#ifndef __DS135_TUR_METER_H
#define __DS135_TUR_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 135

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void ds_init(void);
void ds_update(float *dat);

#endif
#endif