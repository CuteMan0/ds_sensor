#include "ds135_tur_meter.h"

#if DS_SENSOR == 135

#include "stc32g.h"
#include "adc_drive.h"
#include "STC32G_GPIO.h"

#include <math.h>

// 计算斜率 k
#define CALC_K(x1, x0) (190 / (x1 - x0))

// 计算截距 b（需要传入 k）
#define CALC_B(x0, k) ((10) - (k) * (x0))

#define Tr_ON Tr = 1
#define Tr_OFF Tr = 0

sbit Tr = P1 ^ 1; // 发射管控制端口

float ref_vol;
float adc_vol = 0.0f;
ADC_Handle_t adc0;

const float x[2] = {0.88f, 2.85f}; // 10NTU 和 200NTU 对应的电压值

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
    P1_MODE_OUT_PP(GPIO_Pin_1); // P1.1设置为推挽输出
    Tr_ON;
}

void ds_update(void)
{
    adc_vol = adc_get(&adc0);
#if 1
    dat_for_printf = CALC_K(x[1], x[0]) * adc_vol + CALC_B(x[0], CALC_K(x[1], x[0]));
#else
    dat_for_printf = adc_vol;
#endif
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf("tur:%.6f\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif