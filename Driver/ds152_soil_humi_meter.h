#ifndef __DS152_SOIL_HUMI_METER_H
#define __DS152_SOIL_HUMI_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 152

void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

#endif /* DS_SENSOR == 152 */

#endif /* __DS152_SOIL_HUMI_METER_H */