#include "ds141_co2_meter.h"

#if DS_SENSOR == 141

void ds_init(void)
{
}

void ds_update(void)
{
}

void ds_printf(void)
{
    printf("未在此工程实现", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif