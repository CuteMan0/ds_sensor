#include "ds112_fir_meter.h"

#if DS_SENSOR == 112

#include "bsp_mlx90614.h"

void ds_init(void)
{
    mlx_init();
}

void ds_update(float *dat)
{
    mlx_getVal(dat);
}

#endif
