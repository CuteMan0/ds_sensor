/**
 * @file lowpass_filter.h
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-11-5
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __LOWPASS_FILTER_H__
#define __LOWPASS_FILTER_H__

#include "type_def.h"

/*
lp_filter_t lp;
lowpass_filter_init(&lp, 0, 0.1f); // 初值0，系数alpha=0.1
y = LowPassFilter_Update(&lp, x);
*/

#if 0
typedef u16    lp_data_t;  // 输入数据类型
typedef float  lp_calc_t;  // 参与小数运算
#else
typedef float  lp_data_t;
typedef float  lp_calc_t;
#endif

typedef struct
{
    lp_calc_t last;    // 过往输出
    lp_calc_t alpha;   // 滤波系数 (0~1)
    u8 init_ok;        // 初始化标志
} lp_filter_t;

void lowpass_filter_init(lp_filter_t *f, lp_data_t init_value, lp_calc_t alpha);
lp_data_t lowpass_filter_update(lp_filter_t *f, lp_data_t new_sample);

#endif
