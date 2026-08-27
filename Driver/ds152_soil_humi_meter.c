#include "ds152_soil_humi_meter.h"

#if DS_SENSOR == 152

#include "drv_sht40.h"

void ds_init(void)
{
    SHT40_init();
}

void ds_update(void)
{
    dat_for_printf = SHT40_read_humidity();
    avg_filter_update(&filter, dat_for_printf);
}

void ds_printf(void)
{
    printf_usb("Humi=%.2f%%\r\n", dat_for_printf);
}

void ds_calib(void)
{
    /* DS152 无校准需求 */
}

#endif