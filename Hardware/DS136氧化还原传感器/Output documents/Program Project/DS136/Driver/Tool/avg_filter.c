/**
 * @file avg_filter.c
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-9-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "avg_filter.h"

void avg_filter_init(avg_filter_t *filter, avgf_data_t *buffer, u16 len)
{
    u16 i;

    if (!filter || !buffer || 0 == len)
        return;

    filter->buffer = buffer;
    filter->len = len;
    filter->index = 0;
    filter->sum = 0;
    filter->valid_count = 0;

    for (i = 0; i < len; i++)
        buffer[i] = 0;
}

avgf_out_t avg_filter_update(avg_filter_t *filter, avgf_data_t new_sample)
{
    if (!filter || !filter->buffer || 0 == filter->len)
        return 0;

    filter->sum -= filter->buffer[filter->index];
    filter->buffer[filter->index] = new_sample;
    filter->sum += new_sample;

    filter->index = (filter->index + 1) % filter->len;

    if (filter->valid_count < filter->len)
        filter->valid_count++;

    return (filter->sum / filter->valid_count);
}