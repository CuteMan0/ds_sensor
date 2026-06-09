#include "ds145_dco2_meter.h"

#if DS_SENSOR == 145

#include "STC32G_GPIO.h"

#include "adc_drive.h"

ADC_Handle_t adc0;

void dCO2_Init(void)
{
    P1_MODE_IN_HIZ(GPIO_Pin_0); // P1.0设置为推挽输出
    adc_init(&adc0, 0, 3.3f);
}

void dCO2_Read(float *dat)
{
    float dco2_val = 0.0f;

    dco2_val = adc_get(&adc0) * 3846.1f - 1538.4f; // 0~10000ppm 对应 0.4~3V

    *dat = (dco2_val > 0.0f) ? dco2_val : 0.0f;
}

#endif