#include "ds163_nh4_meter.h"

#if DS_SENSOR == 163

#include "adc_drive.h"
#include "led_drive.h"

#define V_O 1.55f // ¡„µ„µÁ—π

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(float *dat)
{
    float adc_vol = 0.0f;
    float nh3_val = 0.0f;

    adc_vol = 2.0f * adc_get(&adc0);

    if (adc_vol < V_O)
    {
        nh3_val = 0.0f;
    }
    else if (adc_vol < 1.95f)
    {
        nh3_val = 6.667f * adc_vol - 10.3f;
    }
    else
    {
        // ?? powf ???????????
        float vol_sq = adc_vol * adc_vol;
        float vol_cu = vol_sq * adc_vol;
        float vol_qu = vol_cu * adc_vol;

        nh3_val = 24.968f * vol_qu -
                  259.6f * vol_cu +
                  993.77f * vol_sq -
                  1638.4f * adc_vol + 974.68f;
    }

    if (nh3_val < 0.1f)
    {
        nh3_val = 0.0f;
    }
    *dat = nh3_val;
}

#endif
