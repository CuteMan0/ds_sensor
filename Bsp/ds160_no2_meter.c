#include "ds160_no2_meter.h"

#if DS_SENSOR == 160

#include "adc_drive.h"

#define V_S 5.0f
#define R_L 0.47f

#define PLOY(x) (0.0028f * pow(x, 3) - 0.0565f * pow(x, 2) + 0.6004f * x - 0.0714f)

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(float *dat)
{
    float adc_vol, no2_val, rs, rs_sq, rs_cu = 0.0f;

    adc_vol = 2.0f * adc_get(&adc0);
    // Vo与Rs的关系
    rs = (V_S - adc_vol) / adc_vol * R_L;
    // Rs与NO2浓度关系
    // 计算 NO2 浓度：0.0028*x^3 - 0.0565*x^2 + 0.6004*x - 0.0714
    rs_sq = rs * rs;
    rs_cu = rs_sq * rs;
    no2_val = 0.0028f * rs_cu - 0.0565f * rs_sq + 0.6004f * rs - 0.0714f;

    // 限制最小浓度
    if (no2_val < 0.1f)
    {
        no2_val = 0.0f;
    }
    *dat = no2_val;
}
#endif