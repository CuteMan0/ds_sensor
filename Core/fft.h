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

#ifndef __FFT_H_
#define __FFT_H_

#include "type_def.h"

#ifndef MAX_FFT_N
#define MAX_FFT_N 256
#endif

typedef struct
{
    float real;
    float imag;
} complex_t;

void fft_init(void);
void fft_run(float *input, complex_t *output);
void fft_abs(complex_t *freq, float *mag);

#endif
