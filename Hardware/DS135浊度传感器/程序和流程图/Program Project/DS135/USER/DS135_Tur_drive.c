#include "DS135_tur_drive.h"

#include "stc32g.h"
#include "STC32G_ADC.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

#include "math.h"

#define V_REF 3.3f

sbit Tr = P1^1; //发射管控制端口

float ref_vol;

#define Tr_ON   Tr = 1
#define Tr_OFF  Tr = 0

static float Avg_Filter(float in_data);

float adc_vol = 0.0f;

void Tur_Init(void)
{
    ADC_InitTypeDef ADC_InitSruct;

    P1_MODE_IN_HIZ(GPIO_Pin_0); // P1.0设置为高阻输入(ADC0)
    P1_MODE_OUT_PP(GPIO_Pin_1); // P1.1设置为推挽输出
    
    Tr_ON;
    
    ADC_InitSruct.ADC_SMPduty = 31;
    ADC_InitSruct.ADC_Speed = ADC_SPEED_2X16T;
    ADC_InitSruct.ADC_AdjResult = ADC_LEFT_JUSTIFIED;
    ADC_InitSruct.ADC_CsSetup = 0;
    ADC_InitSruct.ADC_CsHold = 1;

    ADC_Inilize(&ADC_InitSruct);
    ADC_PowerControl(ENABLE);
}

float Tur_Read(void)
{
    static u8 first_entry = 1;
    u16 adc_val = 0;
//    float adc_vol = 0.0f;
    float tur_val = 10.0f;
    

    
    adc_val = Get_ADCResult(ADC_CH0);
    adc_vol = (float)adc_val * V_REF / 4.096f;//mV
    adc_vol = Avg_Filter(adc_vol);
    if(adc_vol < (720+20))
    tur_val = 357.0f*log(adc_vol)-2250;//10-100
    else if(adc_vol > (720+20))
    tur_val = 314.0f*log(adc_vol)-1963;//100-200
    

    return tur_val;
}

#define AVG_NUMS 5
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