#include "DS145_dCO2_drive.h"

#include "STC32G_ADC.h"
#include "STC32G_GPIO.h"

#include "avg_filter.h"

#define V_REF 3.3f

avg_filter_t dCO2_filter;    //滤波器
u16          dCO2_buffer[20];//滤波数组

void dCO2_Init(void)
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
    
    AvgFilter_Init(&dCO2_filter,dCO2_buffer,sizeof(dCO2_buffer)/2);
}

float dCO2_Read(void)
{
    u16 adc_val = 0;
    float adc_vol = 0.0f;
    float dco2_val = 0.0f;
    
    adc_val = Get_ADCResult(ADC_CH0);
    adc_val = AvgFilter_Update(&dCO2_filter,adc_val);
    adc_vol = (float)adc_val * V_REF / 4096.0f;
    
    dco2_val = adc_vol * 3846.1f - 1538.4f;
    
    return (dco2_val > 0.0f) ? dco2_val : 0.0f ;
}
