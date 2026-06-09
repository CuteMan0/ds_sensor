#ifndef __DS145_dCO2_METER_H
#define __DS145_dCO2_METER_H

#include "ds_sensor.h"
#if DS_SENSOR == 145

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void dCO2_Init(void);

void dCO2_Read(float *dat);

#endif
#endif