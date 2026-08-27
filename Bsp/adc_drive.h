#ifndef __ADC_DRIVE_H
#define __ADC_DRIVE_H

#include "type_def.h"

typedef struct
{
    u8 channel;    // ADC 通道编号
    u16 raw;       // 原始 ADC 采样值
    float vref;    // 参考电压
    float voltage; // 计算后的电压值
} ADC_Handle_t;

#define MAX_ADC_RES 4095

void adc_init(ADC_Handle_t *hadc, u8 channel, float vref);
float adc_get(ADC_Handle_t *hadc);

#endif