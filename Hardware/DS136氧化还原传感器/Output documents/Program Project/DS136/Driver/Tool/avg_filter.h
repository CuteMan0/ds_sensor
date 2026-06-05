/**
 * @file avg_filter.h
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-9-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __AVG_FILTER_H__
#define __AVG_FILTER_H__

#include "type_def.h"

/*
#define NUM_BUF_AVG 16
avg_filter_t filter;
avgf_data_t  buffer[NUM_BUF_AVG];
avg_filter_init(&filter, buffer, NUM_BUF_AVG);

y = avg_filter_update(&filter, x);
*/

#if 0
typedef u8 avgf_data_t;
typedef u32 avgf_sum_t;
typedef u16 avgf_out_t;
#else
typedef float avgf_data_t;
typedef double avgf_sum_t;
typedef double avgf_out_t;
#endif

typedef struct
{
    avgf_data_t *buffer; // 数据缓存指针
    u16 len;               // 总窗口大小
    u16 index;             // 当前写入位置
    u16 valid_count;       // 有效样本数量
    avgf_sum_t sum;      // 当前窗口总和
} avg_filter_t;

void avg_filter_init(avg_filter_t *filter, avgf_data_t *buffer, u16 len);
avgf_out_t avg_filter_update(avg_filter_t *filter, avgf_data_t new_sample);

#endif
