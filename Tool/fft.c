/**
 * @file fft.h
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-9-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "fft.h"
#include <math.h>

float xdata sin_lut[MAX_FFT_N / 2];
float xdata cos_lut[MAX_FFT_N / 2];

/* 位反转表（用于蝶形交换，节省计算时间） */
u16 xdata bitrev[MAX_FFT_N];

static u16 bit_reverse(u16 x, u16 bits)
{
    u16 i;
    u16 r;

    r = 0;
    for (i = 0; i < bits; i++)
    {
        r = (r << 1) | (x & 1);
        x >>= 1;
    }
    return r;
}

void fft_init(void)
{
    u16 i;
    u16 bits;
    float angle;

    /* 计算 LUT */
    for (i = 0; i < MAX_FFT_N / 2; i++)
    {
        angle = -2.0f * 3.1415926f * (float)i / (float)MAX_FFT_N;
        cos_lut[i] = cos(angle);
        sin_lut[i] = sin(angle);
    }

    /* 预计算 bit-reversal  */
    bits = 0;
    i = MAX_FFT_N;
    while (i > 1)
    {
        bits++;
        i >>= 1;
    }

    for (i = 0; i < MAX_FFT_N; i++)
        bitrev[i] = bit_reverse(i, bits);
}

/* 真实 FFT (Cooley-Tukey) */
void fft_run(float *input, complex_t *output)
{
    u16 i;
    u16 j;
    u16 step;
    u16 half;
    u16 a;
    u16 b;
    u16 lut_step;
    float wr;
    float wi;
    float tr;
    float ti;
    float xr;
    float xi;

    /* 位反转排列 */
    for (i = 0; i < MAX_FFT_N; i++)
    {
        j = bitrev[i];
        output[i].real = input[j];
        output[i].imag = 0.0f;
    }

    /* 蝶形运算 */
    step = 2;
    while (step <= MAX_FFT_N)
    {
        half = step >> 1;
        lut_step = MAX_FFT_N / step;

        for (a = 0; a < half; a++)
        {
            wr = cos_lut[a * lut_step];
            wi = sin_lut[a * lut_step];

            for (b = a; b < MAX_FFT_N; b += step)
            {
                j = b + half;

                /* 读取 */
                xr = output[b].real;
                xi = output[b].imag;
                tr = output[j].real;
                ti = output[j].imag;

                /* 旋转乘法 */
                output[j].real = tr * wr - ti * wi;
                output[j].imag = tr * wi + ti * wr;

                /* 蝶形结果 */
                output[b].real = xr + output[j].real;
                output[b].imag = xi + output[j].imag;
                output[j].real = xr - output[j].real;
                output[j].imag = xi - output[j].imag;
            }
        }

        step <<= 1;
    }
}

void fft_abs(complex_t *freq, float *mag)
{
    u16 i;
    float r;
    float im;
    float v;

    for (i = 0; i < MAX_FFT_N; i++)
    {
        r = freq[i].real;
        im = freq[i].imag;
        v = r * r + im * im;

        /* 防止 NaN */
        if (v < 0 || v != v)
            v = 0;

        mag[i] = sqrt(v);
    }
}
