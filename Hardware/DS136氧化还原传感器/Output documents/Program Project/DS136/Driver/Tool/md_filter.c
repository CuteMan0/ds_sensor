/**
 * @file md_filter.c
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-11-6
 *
 * @copyright Copyright (c) 2025
 *
 */
 
#include "md_filter.h"

void md_filter_init(md_filter_t *f, mf_data_t *buf, u16 len)
{
    u16 i;

    if(!f || !buf || 0 == len)
        return;

    f->buffer = buf;
    f->len = len;
    f->index = 0;
    f->valid_count = 0;

    for(i = 0; i < len; i++)
        buf[i] = 0;
}

mf_out_t md_filter_update(md_filter_t *f, mf_data_t new_sample)
{
    u16 i, j;
    mf_data_t tmp;
    mf_data_t sorted[16];

    if(!f || !f->buffer || 0 == f->len)
        return 0;

    f->buffer[f->index] = new_sample;
    f->index++;
    if(f->index >= f->len)
        f->index = 0;

    if(f->valid_count < f->len)
        f->valid_count++;

    // 拷贝到排序数组
    for(i = 0; i < f->valid_count; i++)
        sorted[i] = f->buffer[i];

    // 简单插入排序（窗口小开销低）
    for(i = 1; i < f->valid_count; i++)
    {
        for(j = i; j > 0; j--)
        {
            if(sorted[j] < sorted[j - 1])
            {
                tmp = sorted[j];
                sorted[j] = sorted[j - 1];
                sorted[j - 1] = tmp;
            }else
                break;
        }
    }

    if(f->valid_count & 1)
        return f->buffer[f->valid_count / 2];
    else
        return (f->buffer[f->valid_count/2] + f->buffer[f->valid_count/2 - 1]) / 2;
}
