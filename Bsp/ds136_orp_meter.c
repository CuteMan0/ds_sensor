#include "ds136_orp_meter.h"

#if DS_SENSOR == 136

#include "stc32g.h"
#include "adc_drive.h"

ADC_Handle_t adc0;
ORP_Handle_t horp_handle;

void ORP_Init(void)
{
    adc_init(&adc0, 0, 3.3f);
    horp_handle.polarity = 0;
    horp_handle.vol = 0.0f;
}

void ORP_Read(float *dat)
{
    horp_handle.vol = 2010 - 1333.33f * adc_get(&adc0);

    if (horp_handle.vol < 0)
        horp_handle.polarity = 1;
    else
        horp_handle.polarity = 0;

#define CLAMP_SIMPLE(val, lo, hi) \
    (((val) < (lo)) ? (lo) : (((val) > (hi)) ? (hi) : (val)))

    *dat = CLAMP_SIMPLE(horp_handle.vol, -2000, 2000);
}

#endif