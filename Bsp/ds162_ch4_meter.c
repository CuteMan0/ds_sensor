#include "ds162_ch4_meter.h"
// MP-4
#if DS_SENSOR == 162

#include "adc_drive.h"
#include "lut.h"

ADC_Handle_t adc0;

const LUT_2D mp4_lut[] =
    {
        {650, 0},
        {2150, 1000},
        {2600, 2000},
        {2900, 3000},
        {3050, 4000},
        {3200, 5000},
        {3300, 6000},
        {3400, 7000},
        {3450, 8000},
        {3500, 9000},
        {3600, 10000},
        {3700, 12000},
        {3780, 14000}};

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(float *dat)
{
    u16 ch4_val, adc_vol_mv = 0.0f;

    adc_vol_mv = (u16)(adc_get(&adc0) * 2000);

    ch4_val = LUT_BinaryInterp_u16(mp4_lut,
                                   sizeof(mp4_lut) / sizeof(mp4_lut[0]),
                                   adc_vol_mv);

    if (ch4_val < 300)
    {
        ch4_val = 0;
    }

    *dat = (float)ch4_val;
}

#endif
