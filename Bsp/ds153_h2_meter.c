#include "ds153_h2_meter.h"

#if DS_SENSOR == 153

#include "adc_drive.h"

#define SEN 2.0f // 2nA /ppm
#define GAIN 0.25f
#define RF 153.33f // 153.33kohm = R9//Rt

ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(float *dat)
{
    float ppm = 0.0f;

    ppm = adc_get(&adc0) / (SEN * RF * GAIN) * 1e6;

    *dat = (ppm > 0.0f) ? ppm : 0.0f;
}

#endif