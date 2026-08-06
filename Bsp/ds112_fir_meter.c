#include "ds112_fir_meter.h"

#if DS_SENSOR == 112

#include "bsp_mlx90614.h"

void ds_init(void)
{
    mlx90614_init();
}

void ds_update(void)
{
    mlx90614_getVal(&dat_for_printf);
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf("temp:%.2fC\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif
