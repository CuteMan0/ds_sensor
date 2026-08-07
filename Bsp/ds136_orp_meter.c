#include "ds136_orp_meter.h"

#if DS_SENSOR == 136

#include "stc32g.h"
#include "adc_drive.h"

ADC_Handle_t adc0;
ORP_Handle_t horp_handle;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
    horp_handle.polarity = 0;
    horp_handle.vol = 0.0f;
}

void ds_update(void)
{
    horp_handle.vol = 2010 - 1333.33f * adc_get(&adc0);

    if (horp_handle.vol < 0)
        horp_handle.polarity = 1;
    else
        horp_handle.polarity = 0;

#define CLAMP_SIMPLE(val, lo, hi) \
    (((val) < (lo)) ? (lo) : (((val) > (hi)) ? (hi) : (val)))

    dat_for_printf = CLAMP_SIMPLE(horp_handle.vol, -2000, 2000);
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf_usb("ORG:%.1fmV\r\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif