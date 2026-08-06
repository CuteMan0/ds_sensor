#include "ds145_dco2_meter.h"

#if DS_SENSOR == 145

#include "STC32G_GPIO.h"

#include "adc_drive.h"

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(void)
{
    float dco2_val = 0.0f;

    dco2_val = adc_get(&adc0) * 3846.1f - 1538.4f; // 0~10000ppm 对应 0.4~3V

    dat_for_printf = (dco2_val > 0.0f) ? dco2_val : 0.0f;
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf("dCO2:%.2f\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif