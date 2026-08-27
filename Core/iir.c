#include "iir.h"

void notch_fixed_init(NotchFilterFixed *nf,float fs,float f0,float r)
{
    float w;
    float b0,b1,b2;
    float a1,a2;

    w=2.0f*M_PI*f0/fs;

    b0=1.0f;
    b1=-2.0f*cos(w);
    b2=1.0f;

    a1=-2.0f*r*cos(w);
    a2=r*r;

    nf->b0=(q15_t)(b0*Q14_SCALE);
    nf->b1=(q15_t)(b1*Q14_SCALE);
    nf->b2=(q15_t)(b2*Q14_SCALE);

    nf->a1=(q15_t)(a1*Q14_SCALE);
    nf->a2=(q15_t)(a2*Q14_SCALE);

    nf->x1=0;
    nf->x2=0;
    nf->y1=0;
    nf->y2=0;
}

q15_t adc_to_q15(u16 adc_value)
{
    float normalized = ((float)adc_value - ADC_MID) / ADC_SCALE;
    // 限幅到 [-1, 1]
    if (normalized > 1.0f) normalized = 1.0f;
    if (normalized < -1.0f) normalized = -1.0f;
    // 转换为 Q15
    return (q15_t)(normalized * 32767.0f);
}

q15_t notch_fixed_process(NotchFilterFixed *nf, q15_t x)
{
    q31_t acc;
    q15_t y;

    acc = 0;

    acc += ((q31_t)nf->b0 * x)      >> 14;
    acc += ((q31_t)nf->b1 * nf->x1) >> 14;
    acc += ((q31_t)nf->b2 * nf->x2) >> 14;

    acc -= ((q31_t)nf->a1 * nf->y1) >> 14;
    acc -= ((q31_t)nf->a2 * nf->y2) >> 14;

    /* 饱和 */
    if(acc > 32767)
        acc = 32767;
    else if(acc < -32768)
        acc = -32768;

    y = (q15_t)acc;

    /* 更新状态 */
    nf->x2 = nf->x1;
    nf->x1 = x;

    nf->y2 = nf->y1;
    nf->y1 = y;

    return y;
}