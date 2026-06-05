#include "DS144_do_drive.h"

#include "stc32g.h"
#include "STC32G_ADC.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

#define V_REF 3.3f
#define GAIN 50.0f

static float Avg_Filter(float in_data);

void Do_Init(void)
{
    ADC_InitTypeDef ADC_InitSruct;

    P1_MODE_IN_HIZ(GPIO_Pin_0); // P1.0设置为高阻输入(ADC0)
    P1_MODE_OUT_PP(GPIO_Pin_1); // P1.1设置为推挽输出
    
    ADC_InitSruct.ADC_SMPduty = 31;
    ADC_InitSruct.ADC_Speed = ADC_SPEED_2X16T;
    ADC_InitSruct.ADC_AdjResult = ADC_LEFT_JUSTIFIED;
    ADC_InitSruct.ADC_CsSetup = 0;
    ADC_InitSruct.ADC_CsHold = 1;

    ADC_Inilize(&ADC_InitSruct);
    ADC_PowerControl(ENABLE);
}

float Do_Read(void)
{
    u16 adc_val = 0;
    float adc_vol = 0.0f;
    float do_val = 0.0f;
    
    adc_val = Get_ADCResult(ADC_CH0);
    adc_vol = (float)adc_val * V_REF / 4096;
    adc_vol = Avg_Filter(adc_vol);   //滤波
    
    do_val = adc_vol / GAIN * 1000.0f;//mV
    if(do_val < 24.0f)
    {
        do_val = 0.291f * do_val;
        return do_val;
    }
    if(do_val > 34.0f)
    {
        do_val = 0.562f * do_val - 8.12f;
        return do_val;
    }
    else
    {
        do_val = 0.4f * do_val - 2.6f;
        return do_val;
    }
}

#define AVG_NUMS 20
volatile float dat[AVG_NUMS] = {0.0f};
static float Avg_Filter(float in_data)
{
    float sum_data = 0.0f;
    int i = 0;
    // 前移（左移）滑动窗口
    for (; i < AVG_NUMS - 1; i++) {
        dat[i] = dat[i + 1];
        sum_data += dat[i];
    }
    dat[AVG_NUMS - 1] = in_data;
    sum_data += dat[AVG_NUMS - 1];
    return (sum_data / AVG_NUMS);
}