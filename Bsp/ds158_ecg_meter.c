#include "ds158_ecg_meter.h"

#if DS_SENSOR == 158

#include "adc_drive.h"

#define DC_REF 0.0f

ADC_Handle_t adc0;

const float x[2] = {0.88f, 2.85f}; // 10NTU 和 200NTU 对应的电压值

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(void)
{
    float ecg = 0.0f;
    ecg = adc_get(&adc0);
    dat_for_printf = ecg - DC_REF;
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf_usb("ECG:%uBPM\r\n", (u16)dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif