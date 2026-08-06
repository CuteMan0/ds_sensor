#include "ds160_no2_meter.h"
// WSP-1110
#if DS_SENSOR == 160

#include "adc_drive.h"

#include "lut.h"

#include <math.h>

#define VCC 5.0f
#define RL 1000000.0f // 1M
#define R0 510000.0f  // 510K 空气标定电阻值

ADC_Handle_t adc0;

static const LUT_2D wsp1110_lut[] = {
    {1.31f, 0.1f},
    {2.9f, 0.3f},
    {4.1f, 0.5f},
    {6.1f, 0.7f},
    {9.3f, 1.0f},
    {23.1f, 2.0f},
    {42.0f, 3.0f}};

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(void)
{
    float ratio, rs = 0.0f;
    u16 ppm = 0;

    rs = RL * (0.5f * VCC / adc_get(&adc0) - 1.0f);
    ratio = rs / R0;

    if (ratio > 42.0f)
    {
        // 高浓度按照拟合曲线计算
        ppm = pow(ratio / 9.2, 1.0 / 1.18);
    }
    else
    {
        // 低浓度直接查表
        ppm = LUT_BinaryInterp_u16(wsp1110_lut,
                                   sizeof(wsp1110_lut) / sizeof(wsp1110_lut[0]),
                                   ratio);
    }

    dat_for_printf = (float)ppm;
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}
void ds_printf(void)
{
    printf("NO2:%.2f\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif