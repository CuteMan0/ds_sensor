#include "ds137_so2_meter.h"

#if DS_SENSOR == 137

#include "adc_drive.h"

#define SEN 0.6f // 0.6uA/ppm  (range: 0-150ppm)
#define RF 26.7f // 26.7kOhm

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(float *dat)
{
    float ppm = 0.0f;

    ppm = adc_get(&adc0) / (SEN * RF) * 1e3;

    *dat = (ppm > 0.0f) ? ppm : 0.0f;
}

#endif