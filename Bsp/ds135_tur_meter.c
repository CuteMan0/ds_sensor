#include "ds135_tur_meter.h"

#if DS_SENSOR == 135

#include "stc32g.h"
#include "adc_drive.h"
#include "STC32G_GPIO.h"

#include "math.h"

#define K 98.181f
#define B -110.47f

#define Tr_ON Tr = 1
#define Tr_OFF Tr = 0

sbit Tr = P1 ^ 1; // 发射管控制端口

float ref_vol;
float adc_vol = 0.0f;
ADC_Handle_t adc0;

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
    P1_MODE_OUT_PP(GPIO_Pin_1); // P1.1设置为推挽输出
    Tr_ON;
}

void ds_update(float *dat)
{
    adc_vol = adc_get(&adc0);
#if 1
    *dat = K * adc_vol + B;
#else
    *dat = adc_vol;
#endif
}

#endif