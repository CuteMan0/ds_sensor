/**
 * @file lowpass_filter.c
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-11-5
 *
 * @copyright Copyright (c) 2025
 *
 */
 
#include "lowpass_filter.h"

void lowpass_filter_init(lp_filter_t *f, lp_data_t init_value, lp_calc_t alpha)
{
    if(!f)
        return;

    f->last   = (lp_calc_t)init_value;
    f->alpha  = alpha;
    f->init_ok = 1;
}

lp_data_t lowpass_filter_update(lp_filter_t *f, lp_data_t new_sample)
{
    lp_calc_t out;

    if(!f)
        return 0;

    if(!f->init_ok)
    {
        // 第一次运行自动初始化
        f->last    = (lp_calc_t)new_sample;
        f->init_ok = 1;
    }

    out = f->last + f->alpha * ((lp_calc_t)new_sample - f->last);
    f->last = out;

    return (lp_data_t)out;
}
