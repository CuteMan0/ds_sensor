#ifndef __DS151_SOIL_TEMP_METER_H
#define __DS151_SOIL_TEMP_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 151

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

#endif /* DS_SENSOR == 151 */

#endif /* __DS151_SOIL_TEMP_METER_H */