#include "ds139_o2_meter.h"

#if DS_SENSOR == 139

#include "adc_drive.h"

#define A0 15  // 短接传感器的ADC，用于消除电路噪声干扰
#define A1 566 // 空气中的ADC，用于基准

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(void)
{
    float tmp = 0.0f;
    adc_get(&adc0);
    tmp = (float)(adc0.raw - A0) / (float)(A1 - A0) * 20.90f;

    tmp = (tmp > 0.0f) ? tmp : 0.0f;
    dat_for_printf = (tmp < 100.0f) ? tmp : 100.0f;
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf("O2:%.2f%%\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif