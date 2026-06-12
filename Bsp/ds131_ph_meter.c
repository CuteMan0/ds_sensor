#include "ds131_ph_meter.h"

#if DS_SENSOR == 131

#include "adc_drive.h"

#define V_OFFSET 1.25f

#define K_1 -11.4856f // 酸斜率--delta PH /delta Vol
#define B_1 6.951f    // 截距
#define K_2 -12.0996f // 碱斜率--delta PH /delta Vol
#define B_2 6.956f    // 截距

#define OFFSET 0.00f

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(float *dat)
{
    float ph_vol = 0.0f;
    float tmp = 0.0f;

    ph_vol = adc_get(&adc0) - V_OFFSET;

    // 电压信号转换至PH值 begin
    if (ph_vol > 0)
    {
        tmp = K_1 * (ph_vol - OFFSET) + B_1;
    }
    else
    {
        tmp = K_2 * (ph_vol - OFFSET) + B_2;
    }
    // 电压信号转换至PH值   end

    *dat = tmp < 0.0f ? 0.0f : (tmp > 14.0f ? 14.0f : tmp);
}

#endif