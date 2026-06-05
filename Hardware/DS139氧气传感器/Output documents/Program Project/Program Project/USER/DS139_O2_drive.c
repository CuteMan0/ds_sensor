#include "DS139_O2_drive.h"

#include "STC32G_ADC.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

#include "avg_filter.h"

#define V_REF 3.3f
#define A0 15               // 短接传感器的ADC，用于消除电路噪声干扰
#define A1 566              // 空气中的ADC，用于基准
avg_filter_t O2_filter;    //滤波器
u16          O2_buffer[12];//滤波数组

void O2_Init(void)
{
    ADC_InitTypeDef ADC_InitSruct;
    
    P1_MODE_IN_HIZ(GPIO_Pin_0);		//P1.0设置为高阻输入
    
    ADC_InitSruct.ADC_SMPduty = 31;
    ADC_InitSruct.ADC_Speed = ADC_SPEED_2X16T;
    ADC_InitSruct.ADC_AdjResult = ADC_LEFT_JUSTIFIED;
    ADC_InitSruct.ADC_CsSetup = 0;
    ADC_InitSruct.ADC_CsHold = 1;
    
    ADC_Inilize(&ADC_InitSruct);
    ADC_PowerControl(ENABLE);
    
    AvgFilter_Init(&O2_filter,O2_buffer,sizeof(O2_buffer)/2);
}

float O2_Read(void)
{
    u16 adc_val = 0;
    float o2_val = 0.0f;
    
    adc_val = Get_ADCResult(ADC_CH0);
    adc_val = AvgFilter_Update(&O2_filter,adc_val);
    o2_val = (float)(adc_val - A0) / (float)(A1 - A0);
    o2_val *= 20.90f;
    
    o2_val = (o2_val > 0.0f) ? o2_val : 0.0f;
    o2_val = (o2_val < 100.0f) ? o2_val : 100.0f;
    
    return o2_val;
}