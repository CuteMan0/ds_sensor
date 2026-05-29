#include "ds131_ph_meter.h"

#if DS_SENSOR == 131

#include "adc_drive.h"
#include "STC32G_GPIO.h"

#define V_REF 1.25f

#define K_1 -11.4856f // 酸斜率--delta PH /delta Vol
#define B_1 6.951f    // 截距
#define K_2 -12.0996f // 碱斜率--delta PH /delta Vol
#define B_2 6.956f    // 截距

#define OFFSET 0.00f

ADC_Handle_t adc0;

void pH_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void pH_read(float *ph_val)
{
    u16 adc_vol = 0;
    float ph_vol = 0.0f;
    float tmp = 0.0f;

    adc_vol = adc_get(&adc0);
    ph_vol = adc_vol - V_REF;

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

    *ph_val = tmp < 0.0f ? 0.0f : (tmp > 14.0f ? 14.0f : tmp);
}

#endif