#ifndef __DS_SENSOR_H
#define __DS_SENSOR_H

#define DS_SENSOR 133

#if DS_SENSOR == 112
#include "bsp_mlx90614.h"
#define ds_init() mlx_init()
#define ds_update(x) mlx_getVal(x)
#endif

#if DS_SENSOR == 131
#include "ds131_ph_meter.h"
#define ds_init() pH_init()
#define ds_update(x) pH_read(x)
#endif

#if DS_SENSOR == 132
#include "ds132_ec_meter.h"
#define ds_init() ec_init()
#define ds_update(x) ec_read(x)
#endif

#if DS_SENSOR == 133
#include "ds133_multi_ec_meter.h"
#define ds_init() ec_init()
#define ds_update(x) ec_read(x)
#endif

#if DS_SENSOR == 135
#include "ds135_tur_meter.h"
#define ds_init() tur_init()
#define ds_update(x) tur_read(x)
#endif

#if DS_SENSOR == 136
#include "ds136_orp_meter.h"
#define ds_init() ORP_Init()
#define ds_update(x) ORP_Read(x)
#endif

#if DS_SENSOR == 138
#include "ds138_airC2H6O_meter.h"
#define ds_init() airC2H6O_Init()
#define ds_update(x) airC2H6O_Read(x)
#endif

#if DS_SENSOR == 139
#include "ds139_o2_meter.h"
#define ds_init() O2_Init()
#define ds_update(x) O2_Read(x)
#endif

#endif