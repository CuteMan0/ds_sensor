/*浊度传感器*/
#include "ds135_tur_meter.h"

#if DS_SENSOR == 135

#include "stc32g.h"
#include "adc_drive.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

#include "math.h"

#define K 0.766f
#define B -208.2f

#define Tr_ON Tr = 1
#define Tr_OFF Tr = 0

sbit Tr = P1 ^ 1; // 发射管控制端口

float ref_vol;
float adc_vol = 0.0f;
ADC_Handle_t adc0;

void tur_init(void)
{
    adc_init(&adc0, 0, 3.3f);
    P1_MODE_OUT_PP(GPIO_Pin_1); // P1.1设置为推挽输出
    Tr_ON;
    //    Tr_OFF;
}

void tur_read(float *tur_val)
{
    adc_vol = adc_get(&adc0) * 1000; // mV
    *tur_val = adc_vol;              // 一级信号输出电压
    //    *tur_val = K * *tur_val + B;
}

#endif