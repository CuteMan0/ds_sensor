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

void ds_update(float *dat)
{
    float ppm = 0.0f;

    ppm = adc_get(&adc0) / (SEN * RF * GAIN) * 1e3;

    *dat = (ppm > 0.0f) ? ppm : 0.0f;
}

#endif