#include "ds164_cl2_meter.h"

#if DS_SENSOR == 164

#include "adc_drive.h"

#define SEN 0.6f // 0.6uA/ppm (range: 0-100ppm)
#define GAIN 1.0f
#define RF 26.7f // 26.7kohm

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(void)
{
    float ppm = 0.0f;

    ppm = adc_get(&adc0) / (SEN * RF * GAIN) * 1e3;

    dat_for_printf = (ppm > 0.0f) ? ppm : 0.0f;
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf_usb("CL2:%.2fppm\r\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif