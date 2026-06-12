#ifndef __DS136_ORP_METER_H
#define __DS136_ORP_METER_H

#include "ds_sensor.h"

#if DS_SENSOR == 136

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    u8 polarity; // sign vol is positive -- 0 or negative -- 1
    float vol;
} ORP_Handle_t;

void ds_init(void);
void ds_update(float *dat);

#endif
#endif