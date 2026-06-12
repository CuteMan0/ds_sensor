#include "adc_drive.h"

#include "STC32G_ADC.h"
#include "STC32G_GPIO.h"

void adc_init(ADC_Handle_t *hadc, u8 channel, float vref)
{
    ADC_InitTypeDef ADC_InitSruct;

    if (!hadc)
        return;

    switch (channel)
    {
    case 0:
        P1_MODE_IN_HIZ(1 << 0);
        break;
    case 1:
        P1_MODE_IN_HIZ(1 << 1);
        break;
    case 2:
        P1_MODE_IN_HIZ(1 << 2);
        break;
    case 3:
        P1_MODE_IN_HIZ(1 << 3);
        break;
    case 4:
        P1_MODE_IN_HIZ(1 << 4);
        break;
    case 5:
        P1_MODE_IN_HIZ(1 << 5);
        break;
    case 6:
        P1_MODE_IN_HIZ(1 << 6);
        break;
    case 7:
        P1_MODE_IN_HIZ(1 << 7);
        break;
    default:
        return;
    }

    ADC_InitSruct.ADC_SMPduty = 31;
    ADC_InitSruct.ADC_Speed = ADC_SPEED_2X16T;
    ADC_InitSruct.ADC_AdjResult = ADC_LEFT_JUSTIFIED;
    ADC_InitSruct.ADC_CsSetup = 0;
    ADC_InitSruct.ADC_CsHold = 1;

    ADC_Inilize(&ADC_InitSruct);
    ADC_PowerControl(ENABLE);

    hadc->raw = 0;
    hadc->voltage = 0.0f;
    hadc->vref = vref;
    hadc->channel = channel;
}

float adc_get(ADC_Handle_t *hadc)
{
    if (!hadc)
        return -1.0f;

    hadc->raw = Get_ADCResult(hadc->channel);
    hadc->voltage = (float)hadc->raw * hadc->vref / MAX_ADC_RES;
    return hadc->voltage;
}