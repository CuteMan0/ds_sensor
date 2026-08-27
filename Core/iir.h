#include "type_def.h"

#include <math.h>

#define M_PI 3.1415926f

typedef int16 q15_t;
typedef int32 q31_t;

#define Q14_SCALE   16384

#define ADC_MID  2048.0f   // 12位ADC中点
#define ADC_SCALE 2048.0f   // 归一化到 [-1, 1]

typedef struct
{
    q15_t b0;
    q15_t b1;
    q15_t b2;

    q15_t a1;
    q15_t a2;

    q15_t x1;
    q15_t x2;

    q15_t y1;
    q15_t y2;

}NotchFilterFixed;

/**
 * @brief 初始化定点陷波器
 * @param nf 陷波器结构体指针
 * @param fs 采样频率(Hz)
 * @param f0 陷波频率(Hz)
 * @param r 极半径(0.9-0.99)
 */
void notch_fixed_init(NotchFilterFixed *nf,float fs,float f0,float r);

/**
 * @brief ADC值转Q15值处理
 * @param adc_value ADC采样值
 * @return Q15格式输出
 */
q15_t adc_to_q15(u16 adc_value);

/**
 * @brief 定点陷波器处理
 * @param nf 陷波器结构体指针
 * @param x Q15格式输入
 * @return Q15格式输出
 */
q15_t notch_fixed_process(NotchFilterFixed *nf, q15_t x);