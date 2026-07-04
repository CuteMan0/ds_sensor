#include "ds112_fir_meter.h"

#if DS_SENSOR == 112

#include "bsp_mlx90614.h"

void ds_init(void)
{
    mlx90614_init();
}

void ds_update(float *dat)
{
    mlx90614_getVal(dat);
}

#endif
