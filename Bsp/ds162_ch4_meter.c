#include "ds162_ch4_meter.h"

#if DS_SENSOR == 162

#include "adc_drive.h"

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(float *dat)
{
    float ch4_val = 0.0f;
    float adc_vol = adc_get(&adc0);

    if (adc_vol < 0.85f)
    {
        ch4_val = 0.0f;
    }
    else if (adc_vol < 2.12f)
    {
        ch4_val = 11801.1f * adc_vol - 1003.8f;

        // 限制输出范围（根据传感器规格）
        if (ch4_val < 0.0f)
            ch4_val = 0.0f;
        if (ch4_val > 10000.0f)
            ch4_val = 10000.0f; // 假设最大 10000ppm
    }
    else
    {
        float vol_sq = adc_vol * adc_vol;
        float vol_cu = vol_sq * adc_vol;
        ch4_val = 1939.0f * vol_cu -
                  10970.0f * vol_sq +
                  20293.0f * adc_vol - 10528.0f;

        // 限制输出范围
        if (ch4_val < 0.0f)
            ch4_val = 0.0f;
        if (ch4_val > 50000.0f)
            ch4_val = 50000.0f;
    }

    *dat = ch4_val;
}

#endif
