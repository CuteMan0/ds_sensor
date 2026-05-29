#include "ds139_o2_meter.h"

#if DS_SENSOR == 139

#include "adc_drive.h"

#define V_REF 3.3f
#define A0 15               // 短接传感器的ADC，用于消除电路噪声干扰
#define A1 566              // 空气中的ADC，用于基准

ADC_Handle_t adc0;

void O2_Init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void O2_Read(float* dat)
{   
    float tmp = 0.0f;
    adc_get(&adc0);
    tmp = (float)(adc0.raw - A0) / (float)(A1 - A0) * 20.90f;
    
    tmp = (tmp > 0.0f) ? tmp : 0.0f;
    *dat = (tmp < 100.0f) ? tmp : 100.0f;
}

#endif