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

void ds_update(float *dat)
{
    float ecg = 0.0f;
    ecg = adc_get(&adc0);
    *dat = ecg - DC_REF;
}

#endif