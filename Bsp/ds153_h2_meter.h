#ifndef __DS153_H2_METER_H
#define __DS153_H2_METER_H

#include "ds_sensor.h"
#if DS_SENSOR == 153

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

void H2_Init(void);

void H2_Read(float *dat);

#endif
#endif