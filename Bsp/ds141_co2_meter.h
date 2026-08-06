#ifndef __DS141_CO2_METER_H
#define __DS141_CO2_METER_H

#include "ds_sensor.h"
#if DS_SENSOR == 141

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

#endif
#endif