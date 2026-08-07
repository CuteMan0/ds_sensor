#include "ds151_soil_temp_meter.h"

#if DS_SENSOR == 151

#include "bsp_ds18b20.h"

static float temp;
static u8 conv_pending = 0;
static u8 printf_pending = 1;

void ds_init(void)
{
    ds18b20_init();
}

void ds_update(void)
{
    u8 ret;
    if (!conv_pending)
    {
        ds18b20_start_conversion(DS18B20_RES_12BIT);
        conv_pending = 1;
    }
    else
    {
        ret = ds18b20_read_temperature(&temp);
        if (ret == DS18B20_OK)
        {
            dat_for_printf = temp;
            avg_filter_update(&filter, dat_for_printf);
            conv_pending = 0;
            printf_pending = 0;
        }
    }
    task_delay_ms(750);
}

void ds_printf(void)
{
    if (!printf_pending)
    {
        printf("temp:%.2fC\n", dat_for_printf);
        printf_pending = 1;
    }
}

void ds_calib(void)
{
    /* DS151 无校准需求 */
}

#endif
